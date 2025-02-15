/*
 * cilp.cpp
 *
 *  Created on: Oct 23, 2009
 *      Author: diego
 */

#include "cilp.hh"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <vector>

namespace nalso {
namespace algorithms {

Cilp::~Cilp() {}

void Cilp::computeParams(std::set<logic::ClausePtr>& clauses, double& amin, double& w,
                         double beta, std::map<std::string, int>& mus) {
  std::vector<int> ks;

  // count how many times each propositional variable appears as the head of a
  // clause
  for (auto it = clauses.begin(); it != clauses.end(); it++) {
    std::string head = *(**it).getHead();
    if (mus.find(head) == mus.end()) {
      mus[head] = 1;
    } else {
      mus[head]++;
    }

    // keep track of the size of the body of each clause
    ks.push_back((**it).getBody().size());
  }

  // mix the length body count with the number of clauses per different head.
  for (auto it = mus.begin(); it != mus.end(); it++) {
    ks.push_back((*it).second);
  }

  // compute the maximum element of the ks vector
  int maxksmus = *std::max_element(ks.begin(), ks.end());

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

void Cilp::computeParams(std::set<logic::ClausePtr>& clauses, double& amin, double& w,
                         double beta) {
  std::map<std::string, int> mus;
  computeParams(clauses, amin, w, beta, mus);
}

std::set<logic::BoolVarPtr> Cilp::getAtoms(std::set<logic::ClausePtr>& cls) {
  std::set<logic::BoolVarPtr> res;
  // now we add all the clauses atoms if they're not already in the list
  for (auto clIt = cls.begin(); clIt != cls.end(); clIt++) {
    // first we check the head.
    if (!utils::contains(res, *(**clIt).getHead())) {
      res.insert((**clIt).getHead());
    }

    // now we add the variables in the body
    for (auto bodyIt = (**clIt).getBody().begin(); bodyIt != (**clIt).getBody().end(); bodyIt++) {
      if (!utils::contains(res, *(**bodyIt).getVar())) {
        res.insert((**bodyIt).getVar());
      }
    }
  }

  return res;
}

neural::NeuralNetworkPtr Cilp::buildNetwork(logic::ProgramPtr pr) {
  return buildNetwork(pr->getClauses());
}

neural::NeuralNetworkPtr Cilp::buildNetwork(std::set<logic::ClausePtr> cls) {
  // we compute the parameters.
  std::map<std::string, int> mus;
  computeParams(cls, amin, w, beta, mus);

  std::shared_ptr<neural::FeedForwardNeuralNetwork> res(new neural::FeedForwardNeuralNetwork);

  // definition of the activation methods to be used
  neural::NeuralMethodPtr linear(new neural::LinearMethod);
  neural::NeuralMethodPtr bipolar(new neural::BipolarSemilinearMethod(beta));

  std::set<logic::BoolVarPtr> atoms = getAtoms(cls);

  // first we create an input and output unit for each variable that appears in
  // the program
  for (auto it = atoms.begin(); it != atoms.end(); it++) {
    std::string str = **it;
    str += "_i";
    neural::NeuralNodePtr input(new neural::NeuralNode(str, linear));
    input->setType(neural::INPUT);
    input->setLayer(0);

    res->addNode(input, 0);

    str = **it;
    str += "_o";
    neural::NeuralNodePtr output(new neural::NeuralNode(str, linear));
    output->setType(neural::OUTPUT);
    output->setLayer(0);

    res->addNode(output, 0);
  }

  // Now we add the hidden nodes. There's one for each clause
  int counter = 0;
  for (auto it = cls.begin(); it != cls.end(); it++) {
    std::stringstream name;
    name << "h_" << counter;
    int k = (**it).getBody().size();

    neural::NeuralNodePtr hidden(new neural::NeuralNode(name.str(), bipolar));
    hidden->setLayer(1);
    hidden->setBias(((1 + amin) * (k - 1) * w) / 2);

    res->addNode(hidden);

    // we connect the newly added node to the nodes that represent the other
    // literals in the clause.
    for (auto litit = (*it)->getBody().begin(); litit != (*it)->getBody().end(); litit++) {
      std::string source = *(**litit).getVar();
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
