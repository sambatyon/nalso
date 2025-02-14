/*
 * cilp.cpp
 *
 *  Created on: Oct 23, 2009
 *      Author: diego
 */

#include "cilp.hh"

namespace nalso {

namespace algorithms {

Cilp::~Cilp() {}

void Cilp::computeParams(set<ClausePtr>& clauses, double& amin, double& w,
                         double beta, map<string, int>& mus) {
  vector<int> ks;

  // count how many times each propositional variable appears as the head of a
  // clause
  for (set<ClausePtr>::iterator it = clauses.begin(); it != clauses.end();
       it++) {
    string head = *(**it).getHead();
    if (mus.find(head) == mus.end())
      mus[head] = 1;
    else
      mus[head]++;

    // keep track of the size of the body of each clause
    ks.push_back((**it).getBody().size());
  }

  // mix the length body count with the number of clauses per different head.
  for (map<string, int>::iterator it = mus.begin(); it != mus.end(); it++)
    ks.push_back((*it).second);

  // compute the maximum element of the ks vector
  int maxksmus = *max_element(ks.begin(), ks.end());

  // Compute the A_min value, i.e. the minimal value where a unit is consider to
  // evaluate to true
  // the nan check is for some networks that use compute its own amin.
  amin = amin != NAN
             ? amin
             : ((double)maxksmus - 1.0) / ((double)maxksmus + 1.0) + 0.000001;
  // computes the w, which is the standar weight for the units.
  w = (2 / beta) *
      ((log(1 + amin) - log(1 - amin)) / (maxksmus * (amin - 1) + amin + 1));
}

void Cilp::computeParams(set<ClausePtr>& clauses, double& amin, double& w,
                         double beta) {
  map<string, int> mus;
  computeParams(clauses, amin, w, beta, mus);
}

set<BoolVarPtr> Cilp::getAtoms(set<ClausePtr>& cls) {
  set<BoolVarPtr> res;
  set<BoolVarPtr>::iterator it, inputIt;
  // now we add all the clauses atoms if they're not already in the list
  for (set<ClausePtr>::iterator clIt = cls.begin(); clIt != cls.end(); clIt++) {
    // first we check the head.
    if (!utils::isInSet(res, *(**clIt).getHead(), inputIt))
      res.insert((**clIt).getHead());

    // now we add the variables in the body
    for (set<LiteralPtr>::iterator bodyIt = (**clIt).getBody().begin();
         bodyIt != (**clIt).getBody().end(); bodyIt++)
      if (!utils::isInSet(res, *(**bodyIt).getVar(), it))
        res.insert((**bodyIt).getVar());
  }

  return res;
}

NeuralNetworkPtr Cilp::buildNetwork(ProgramPtr pr) {
  return buildNetwork(pr->getClauses());
}

NeuralNetworkPtr Cilp::buildNetwork(set<ClausePtr> cls) {
  // we compute the parameters.
  map<string, int> mus;
  computeParams(cls, amin, w, beta, mus);

  shared_ptr<FeedForwardNeuralNetwork> res(new FeedForwardNeuralNetwork);

  // definition of the activation methods to be used
  NeuralMethodPtr linear(new LinearMethod);
  NeuralMethodPtr bipolar(new BipolarSemilinearMethod(beta));

  set<BoolVarPtr> atoms = getAtoms(cls);

  // first we create an input and output unit for each variable that appears in
  // the program
  for (set<BoolVarPtr>::iterator it = atoms.begin(); it != atoms.end(); it++) {
    string str = **it;
    str += "_i";
    NeuralNodePtr input(new NeuralNode(str, linear));
    input->setType(INPUT);
    input->setLayer(0);

    res->addNode(input, 0);

    str = **it;
    str += "_o";
    NeuralNodePtr output(new NeuralNode(str, linear));
    output->setType(OUTPUT);
    output->setLayer(0);

    res->addNode(output, 0);
  }

  // Now we add the hidden nodes. There's one for each clause
  int counter = 0;
  for (set<ClausePtr>::iterator it = cls.begin(); it != cls.end(); it++) {
    stringstream name;
    name << "h_" << counter;
    int k = (**it).getBody().size();

    NeuralNodePtr hidden(new NeuralNode(name.str(), bipolar));
    hidden->setLayer(1);
    hidden->setBias(((1 + amin) * (k - 1) * w) / 2);

    res->addNode(hidden);

    // we connect the newly added node to the nodes that represent the other
    // literals in the clause.
    for (set<LiteralPtr>::iterator litit = (*it)->getBody().begin();
         litit != (*it)->getBody().end(); litit++) {
      string source = *(**litit).getVar();
      source += "_i";
      res->connectNodes(source, hidden->getId(),
                        (*litit)->isNegated() ? -w : w);
      counter++;
    }
  }

  return res;
}

}  // namespace algorithms

}  // namespace nalso
