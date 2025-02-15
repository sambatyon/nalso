/**
 * @file feedforward.cpp
 *
 * @date Oct 10, 2009
 * @author Alexander Rojas <alexander.rojas@gmail.com>
 */

#include "nalso/networks/feedforward.hh"

#include <sstream>

#include "nalso/utils/utils.hh"

namespace nalso {

namespace neural {

FeedForwardNeuralNetwork::FeedForwardNeuralNetwork() {
  std::vector<NeuralNodePtr> tmp;
  nodes.push_back(tmp);
}

FeedForwardNeuralNetwork::~FeedForwardNeuralNetwork() {
  for (unsigned int i = 0; i < nodes.size(); i++) {
    nodes[i].clear();
  }
  nodes.clear();
}

void FeedForwardNeuralNetwork::addNode(NeuralNodePtr node,
                                       int subNetwork /*= 0*/) {
  // if the neuron is of type INPUT, then we create the equivalent NeuralEnd
  // object we connect the new end to the given node and add it to the inputs
  // std::vector
  if ((node->getType() & INPUT) == INPUT) {
    // This is done so there's no problem when creating the connection with the
    // real output.
    (*node).setType((*node).getType() & ~INPUT);
    // IMPORTANT the id of the inputs and outputs networks include the
    // subnetwork they belong to as a prefix
    std::stringstream ss;
    ss << "w" << subNetwork << "-" << node->getId();
    // we delete the indicator of and input unit to facilitate the fixpoint
    // method
    std::string id = ss.str();
    std::string sub = id.substr(id.length() - 2, 2);
    std::string comp("_i");
    if (sub == comp) {
      id.erase(id.end() - 2, id.end());
    }
    NeuralEndPtr end(new NeuralEnd(id, true));
    ProxyElem tmp(ss.str(), end);
    inputs.insert(tmp);
    NeuralConnection::connect(end, node, 1);
  }

  // if the neuron is of type OUTPUT, then we create the equivalent NeuralEnd
  // object we connect the given node to the new end and add it to the outputs
  // std::vector
  if ((node->getType() & OUTPUT) == OUTPUT) {
    // This is done so there's no problem when creating the connection with the
    // real output.
    (*node).setType((*node).getType() & ~OUTPUT);
    // IMPORTANT the id of the inputs and outputs networks include the
    // subnetwork they belong to as a prefix
    std::stringstream ss;
    ss << "w" << subNetwork << "-" << node->getId();
    // we delete the indicator of and output unit to facilitate the fixpoint
    // method
    std::string id = ss.str();
    std::string sub = id.substr(id.length() - 2, 2);
    std::string comp("_o");
    if (sub == comp) id.erase(id.end() - 2, id.end());
    NeuralEndPtr end(new NeuralEnd(ss.str(), false));
    ProxyElem tmp(id, end);
    outputs.insert(tmp);
    NeuralConnection::connect(node, end, 1);
  }

  node->setSubnetwork(subNetwork);
  nodes[subNetwork].push_back(
      node);  // finally the node is added to the proper subnetwork
}

bool FeedForwardNeuralNetwork::connectNodes(NeuralConnectionPtr source,
                                            NeuralConnectionPtr dest,
                                            double weight /*=1*/) {
  return NeuralConnection::connect(source, dest, weight);
}

bool FeedForwardNeuralNetwork::connectNodes(std::string sourceId, std::string destId,
                                            double weight /*=1*/,
                                            int subNetSource /*= 0*/,
                                            int subNetDest /*= 0*/) {
  if (subNetSource > (int)nodes.size() || subNetDest > (int)nodes.size()) {
    return false;
  }

  if (subNetSource < 0 || subNetDest < 0) {
    return false;
  }

  NeuralNodePtr source, dest;
  // search all layers for the source unit
  for (unsigned int j = 0; j < nodes[subNetSource].size(); j++) {
    if (nodes[subNetSource][j]->getId() == sourceId) {
      source = nodes[subNetSource][j];
      break;
    }
  }

  // search all layers for the source unit
  for (unsigned int j = 0; j < nodes[subNetDest].size(); j++) {
    if (nodes[subNetDest][j]->getId() == destId) {
      dest = nodes[subNetDest][j];
      break;
    }
  }

  if (!(source.get() && dest.get())) {
    return false;
  }

  return connectNodes(source, dest, weight);
}

ParamsMap FeedForwardNeuralNetwork::evaluate(ParamsMap& input) {
  ParamsMap::iterator it;
  for (it = input.begin(); it != input.end(); it++) {
    if (inputs.find((*it).first) != inputs.end()) {
      inputs[(*it).first]->setOutputValue((*it).second);
    }
  }

  std::map<std::string, double> res;
  for (auto oit = outputs.begin(); oit != outputs.end(); oit++) {
    std::pair<std::string, double> resElem((*oit).first,
                                 (*oit).second->outputValue(true));
    res.insert(resElem);
  }
  reset();
  return res;
}

void FeedForwardNeuralNetwork::reset() {
  for (auto it = outputs.begin(); it != outputs.end(); it++) {
    (*(*it).second).reset();
  }

  for (auto it = inputs.begin(); it != inputs.end(); it++) {
    (*(*it).second).reset();
  }

  for (unsigned int i = 0; i < nodes.size(); i++) {
    for (unsigned int j = 0; j < nodes[i].size(); j++) {
      (*nodes[i][j]).reset();
    }
  }
}

#ifdef DEBUG
std::string FeedForwardNeuralNetwork::debugString() {
  std::string res =
      "digraph hopfield{\n\tnode [shape=plaintext, fontsize=10];\n\tedge "
      "[arrowhead=none];\n";

  res += "\t{\n\t\trank=same;\n";
  for (auto input = inputs.begin(); input != inputs.end(); input++) {
    res += "\t\t" + utils::removeMinus((*input).first) + ";\n";
  }
  res += "\t}\n";

  res += "\t{\n\t\trank=same;\n";
  for (auto output = outputs.begin(); output != outputs.end(); output++) {
    res += "\t\t" + utils::removeMinus((*output).first) + ";\n";
  }
  res += "\t}\n";

  for (auto output = outputs.begin(); output != outputs.end(); output++){
    res += (*output).second->debugString();
  }
  res += "}";
  return res;
}
#endif

}  // namespace neural
}  // namespace nalso
