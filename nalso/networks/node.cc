/*
 * @file node.cpp
 *
 * @date Oct 10, 2009
 * @author Alexander Rojas <alexander.rojas@gmail.com>
 */

#include "node.hh"

namespace nalso {

namespace neural {

NeuralNode::NeuralNode(string _id, NeuralMethodPtr _method)
    : NeuralConnection(_id), method(_method), layer(0), subnetwork(0) {
  mt19937 generator(static_cast<unsigned int>(std::time(0)));
  uniform_real<> uni_dist(0, 1);
  random.reset(new Random(generator, uni_dist));

  weights.push_back(0);
  changeInWeights.push_back(0);
  bestWeights.push_back(weights[0]);
}

NeuralNode::NeuralNode(string _id, NeuralMethodPtr _method, RandomPtr _random)
    : NeuralConnection(_id),
      random(_random),
      method(_method),
      layer(0),
      subnetwork(0) {
  weights.push_back(0);
  changeInWeights.push_back(0);
  bestWeights.push_back(weights[0]);
}

double NeuralNode::outputValue(bool calculate) {
  if (isnan(unitValue) && calculate) unitValue = (*method).outputValue(*this);

  return unitValue;
}

double NeuralNode::errorValue(bool calculate) {
  if (!isnan(unitValue) && isnan(unitError) && calculate)
    unitError = method->errorValue(*this);

  return unitError;
}

void NeuralNode::saveWeights() {
  bestWeights = weights;

  for (int i = 0; i < (int)inputList.size(); i++) inputList[i]->saveWeights();
}

void NeuralNode::restoreWeights() {
  weights = bestWeights;

  for (int i = 0; i < (int)inputList.size(); i++)
    inputList[i]->restoreWeights();
}

void NeuralNode::reset() {
  if (!isnan(unitValue) || !isnan(unitError)) {
    unitValue = NAN;
    unitError = NAN;
    weightsUpdated = false;
    for (unsigned int i = 0; i < inputList.size(); i++) inputList[i]->reset();
  }
}

double NeuralNode::weightValue(int index) {
  if (index >= (int)inputList.size() || index < -1) return NAN;
  // weights[0] is the bias
  return weights[index + 1];
}

void NeuralNode::updateWeights(double learning, double momentum) {
  if (!weightsUpdated && !isnan(unitError))
    method->updateWeights(*this, learning, momentum);

  NeuralConnection::updateWeights(learning, momentum);
}

bool NeuralNode::connectInput(NeuralConnectionPtr i, int n,
                              double weight /*= NAN*/) {
  if (!NeuralConnection::connectInput(i, n)) return false;

  if (isnan(weight))
    weights.push_back(((*random)()) * 0.1 - .05);
  else
    weights.push_back(weight);

  changeInWeights.push_back(0);
  return true;
}

bool NeuralNode::disconnectInput(NeuralConnectionPtr i, int n) {
  int loc = -1;
  bool removed = false;
  do {
    loc = -1;
    for (unsigned int noa = 0; noa < inputList.size(); noa++) {
      if (i == inputList[noa] && (n == -1 || n == inputNums[noa])) {
        loc = noa;
        break;
      }
    }

    if (loc >= 0) {
      vector<NeuralConnectionPtr>::iterator ilit = inputList.begin();
      vector<int>::iterator init = inputNums.begin();
      vector<double>::iterator wit = weights.begin(),
                               chwit = changeInWeights.begin();
      // since the weights vector has one more element for the bias than the
      // input list
      wit++;
      chwit++;
      for (int i = 0; i < loc; i++) {
        ilit++;
        init++;
        wit++;
        chwit++;
      }
      if (!(ilit == inputList.end())) {
        inputList.erase(ilit);
        inputNums.erase(init);
        weights.erase(wit);
        changeInWeights.erase(chwit);
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

void NeuralNode::removeAllInputs() {
  NeuralConnection::removeAllInputs();

  double oldBias = weights[0];
  double oldChange = changeInWeights[0];

  weights.clear();
  changeInWeights.clear();

  weights.push_back(oldBias);
  changeInWeights.push_back(oldChange);
}

}  // namespace neural
}  // namespace nalso
