/*
 * @file connection.cpp
 *
 * @date Oct 10, 2009
 * @author Alexander Rojas <alexander.rojas@gmail.com>
 */

#include "node.hh"
#include "utils/utils.hh"

namespace nalso {

namespace neural {

NeuralConnection::NeuralConnection(string _id)
    : unitValue(NAN),
      unitError(NAN),
      weightsUpdated(false),
      id(_id),
      type(UNCONNECTED) {}

NeuralConnection::~NeuralConnection() {}

bool NeuralConnection::operator==(NeuralConnection& other) const {
  return this == &other;
}

void NeuralConnection::updateWeights(double _learning, double _momentum) {
  // the action the subclasses should perform is upto them
  // but if they coverride they should make a call to this to
  // call the method for all their inputs.

  if (!weightsUpdated) {
    for (unsigned int noa = 0; noa < inputList.size(); noa++) {
      inputList[noa]->updateWeights(_learning, _momentum);
    }
    weightsUpdated = true;
  }
}

bool NeuralConnection::connectInput(NeuralConnectionPtr i, int n,
                                    double weight) {
  // checks that another connections does not exists
  for (unsigned int noa = 0; noa < inputList.size(); noa++)
    if (i == inputList[noa]) return false;

  inputList.push_back(i);
  inputNums.push_back(n);
  return true;
}

bool NeuralConnection::connectOutput(NeuralConnectionPtr o, int n) {
  for (unsigned int noa = 0; noa < outputList.size(); noa++)
    if (o == outputList[noa]) return false;

  outputList.push_back(o);
  outputNums.push_back(n);
  return true;
}

bool NeuralConnection::disconnectInput(NeuralConnectionPtr i, int n) {
  int loc = -1;
  bool removed = false;
  do {
    // looks if the node appears in the input list. if so, saves its index
    loc = -1;
    for (unsigned int noa = 0; noa < inputList.size(); noa++) {
      // if the two units are the same and inputNums is the same (or not
      // setterd)
      if (i.get() == inputList[noa].get() && (n == -1 || n == inputNums[noa])) {
        loc = noa;
        break;
      }
    }

    if (loc >= 0) {
      vector<NeuralConnectionPtr>::iterator ilit = inputList.begin();
      vector<int>::iterator init = inputNums.begin();
      for (int i = 0; i < loc; i++) {
        ilit++;
        init++;
      }
      if (!(ilit == inputList.end())) {
        inputList.erase(ilit);
        inputNums.erase(init);
      }
      for (int noa = loc; noa < (int)inputList.size(); noa++) {
        // set the other end to have the right connection number.
        inputList[noa]->changeOutputNum(inputNums[noa], noa);
      }
      removed = true;
    }
  } while (n == -1 && loc != -1);
  return removed;
}

void NeuralConnection::removeAllInputs() {
  // TODO: Take care with deleting the bias
  // this command will simply remove any connections this node has
  // with the other in 1 go, rather than seperately.
  for (int noa = 0; noa < (int)inputList.size(); noa++)
    inputList[noa]->disconnectOutput(NeuralConnectionPtr(this), -1);

  // now reset the inputs.
  inputList.clear();
  setType(getType() & (~INPUT));
  if (getNumOutputs() == 0) setType(getType() & (~CONNECTED));
  inputNums.clear();
}

void NeuralConnection::changeInputNum(int index, int value) {
  if (index >= (int)inputList.size() || index < 0) return;

  inputNums[index] = value;
}

void NeuralConnection::changeOutputNum(int index, int value) {
  if (index >= (int)outputList.size() || index < 0) return;

  outputNums[index] = value;
}

bool NeuralConnection::disconnectOutput(NeuralConnectionPtr o, int n) {
  int loc = -1;
  bool removed = false;
  do {
    loc = -1;
    for (unsigned int noa = 0; noa < outputList.size(); noa++) {
      if (o.get() == outputList[noa].get() &&
          (n == -1 || n == outputNums[noa])) {
        loc = noa;
        break;
      }
    }

    if (loc >= 0) {
      vector<NeuralConnectionPtr>::iterator ilit = outputList.begin();
      vector<int>::iterator init = outputNums.begin();
      for (int i = 0; i < loc; i++) {
        ilit++;
        init++;
      }
      if (!(ilit == outputList.end())) {
        outputList.erase(ilit);
        outputNums.erase(init);
      }
      for (int noa = loc; noa < (int)outputList.size(); noa++) {
        // set the other end to have the right connection number.
        outputList[noa]->changeInputNum(outputNums[noa], noa);
      }
      removed = true;
    }
    // LOG4CXX_DEBUG(ncLogger, "disconnectOutput stuck on the while loop");
  } while (n == -1 && loc != -1);
  return removed;
}

void NeuralConnection::removeAllOutputs() {
  // this command will simply remove any connections this node has
  // with the other in 1 go, rather than seperately.
  for (int noa = 0; noa < (int)outputList.size(); noa++)
    outputList[noa]->disconnectOutput(NeuralConnectionPtr(this), -1);

  // now reset the inputs.
  outputList.clear();
  outputNums.clear();
  setType(getType() & (~OUTPUT));
  if (getNumInputs() == 0) setType(getType() & (~CONNECTED));
}

bool NeuralConnection::connect(NeuralConnectionPtr s, NeuralConnectionPtr t,
                               double weight) {
  if (!s.get() || !t.get()) return false;

  // this ensures that there is no existing connection between these
  // two units already. This will also cause the current weight there to
  // be lost
  disconnect(s, t);
  // WARN: this could be a problem if Hopfield networks need to be implemented
  if (s == t) return false;
  if ((t->getType() & PURE_INPUT) == PURE_INPUT)
    return false;  // target is an input node.

  if ((s->getType() & PURE_OUTPUT) == PURE_OUTPUT)
    return false;  // source is an output node

  if ((s->getType() & PURE_INPUT) == PURE_INPUT &&
      (t->getType() & PURE_OUTPUT) == PURE_OUTPUT)
    return false;  // there is no actual working node in use

  if ((t->getType() & PURE_OUTPUT) == PURE_OUTPUT && t->getNumInputs() > 0)
    return false;  // more than 1 node is trying to feed a particular output

  if ((t->getType() & PURE_OUTPUT) == PURE_OUTPUT &&
      (s->getType() & OUTPUT) == OUTPUT)
    return false;  // an output node already feeding out a final answer

  if (!s->connectOutput(t, t->getNumInputs())) return false;

  if (!t->connectInput(s, s->getNumOutputs() - 1, weight)) {
    s->disconnectOutput(t, t->getNumInputs());
    return false;
  }

  // now ammend the type.
  if ((s->getType() & PURE_INPUT) == PURE_INPUT)
    t->setType(t->getType() | INPUT);
  else if ((t->getType() & PURE_OUTPUT) == PURE_OUTPUT)
    s->setType(s->getType() | OUTPUT);

  t->setType(t->getType() | CONNECTED);
  s->setType(s->getType() | CONNECTED);
  return true;
}

bool NeuralConnection::disconnect(NeuralConnectionPtr s,
                                  NeuralConnectionPtr t) {
  if (!s.get() || !t.get()) return false;

  bool stat1 = s->disconnectOutput(t, -1);
  bool stat2 = t->disconnectInput(s, -1);
  if (stat1 && stat2) {
    if ((s->getType() & PURE_INPUT) == PURE_INPUT) {
      t->setType(t->getType() & (~INPUT));
    } else if ((t->getType() & (PURE_OUTPUT)) == PURE_OUTPUT) {
      s->setType(s->getType() & (~OUTPUT));
    }
    if (s->getNumInputs() == 0 && s->getNumOutputs() == 0) {
      s->setType(s->getType() & (~CONNECTED));
    }
    if (t->getNumInputs() == 0 && t->getNumOutputs() == 0) {
      t->setType(t->getType() & (~CONNECTED));
    }
  }
  return stat1 && stat2;
}

#ifdef DEBUG
string NeuralConnection::debugString() {
  // vector<NeuralConnectionPtr> inputList;
  if (inputList.empty()) return "";

  string res;
  for (unsigned int i = 0; i < inputList.size(); i++) {
    res += "\t" + utils::removeMinus(inputList[i]->getId()) + "->" + id + ";\n";
    res += inputList[i]->debugString();
  }

  return res;
}
#endif

}  // namespace neural

}  // namespace nalso
