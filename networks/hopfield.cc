/**
 * @file hopfield.cpp
 *
 * @date Oct 16, 2009
 * @author Alexander Rojas <alexander.rojas@gmail.com>
 */

#include "hopfield.hh"

namespace nalso {

namespace neural {

void HopfieldNode::computeNextPotential() {
  nextPotential = 0;
  // iterate over every node
  for (list<HyperConnectionPtr>::iterator hcit = inputs.begin();
       hcit != inputs.end(); hcit++) {
    // weight * (V_1 * ... * V_n) where i != this
    double mult = (**hcit).first;
    // CHECK THIS
    if ((**hcit).second.size() == 1 && (*(**hcit).second.begin()).get() == this)
      mult = 0;
    for (list<HopfieldNodePtr>::iterator nit = (**hcit).second.begin();
         nit != (**hcit).second.end(); nit++)
      if ((*nit).get() != this) mult *= (**nit).outputValue();
    nextPotential += mult;
  }

  nextPotential += potential;
}

void HopfieldNeuralNetwork::connectNodes(vector<HopfieldNodePtr> _nodes,
                                         double weight /* = 1*/) {
#ifdef DEBUG
  connections.push_back(make_pair(weight, _nodes));
#endif
  HyperConnectionPtr conn(new HyperConnection);
  (*conn).first = weight;
  for (unsigned int i = 0; i < _nodes.size(); i++)
    (*_nodes[i]).addToHyperConnection(conn, _nodes[i]);
}

void HopfieldNeuralNetwork::connectNodes(vector<string> _nodeNames,
                                         double weight /* = 1*/) {
  vector<HopfieldNodePtr> _nodes;
  for (unsigned int i = 0; i < _nodeNames.size(); i++)
    if (nodes.find(_nodeNames[i]) != nodes.end())
      _nodes.push_back(nodes[_nodeNames[i]]);

  connectNodes(_nodes, weight);
}

ParamsMap HopfieldNeuralNetwork::evaluate(ParamsMap& input) {
  ParamsMap::iterator it;
  for (it = input.begin(); it != input.end(); it++)
    if (nodes.find((*it).first) != nodes.end())
      (*nodes[(*it).first]).setInitialPotential((*it).second);

  map<string, double> res;
  for (map<string, HopfieldNodePtr>::iterator nit = nodes.begin();
       nit != nodes.end(); nit++)
    res.insert(make_pair((*nit).first, (*(*nit).second).outputValue()));
  return res;
}

ParamsMap HopfieldNeuralNetwork::findFixPoint(ParamsMap input,
                                              double tolerance /* = 0*/) {
  // first set the inputs as the initial potential set.
  ParamsMap::iterator it;
  for (it = input.begin(); it != input.end(); it++)
    if (nodes.find((*it).first) != nodes.end())
      (*nodes[(*it).first]).setInitialPotential((*it).second);

  ParamsMap outOld, outNew;
  map<string, HopfieldNodePtr>::iterator nit;
  // evaluate the network with the initial potential
  for (map<string, HopfieldNodePtr>::iterator nit = nodes.begin();
       nit != nodes.end(); nit++)
    outNew.insert(make_pair((*nit).first, (*(*nit).second).outputValue()));

  do {
    outOld = outNew;  // the new becomes old
    outNew.clear();

    // compute the next potential value
    for (nit = nodes.begin(); nit != nodes.end(); nit++)
      (*(*nit).second).computeNextPotential();

    // update potential
    for (nit = nodes.begin(); nit != nodes.end(); nit++)
      (*(*nit).second).updatePotential();

    // evaluate the network with the new potential
    for (map<string, HopfieldNodePtr>::iterator nit = nodes.begin();
         nit != nodes.end(); nit++)
      outNew.insert(make_pair((*nit).first, (*(*nit).second).outputValue()));
  } while (!NeuralNetwork::areEqualParameters(outNew, outOld, tolerance));

  return outNew;
}

#ifdef DEBUG
std::string HopfieldNeuralNetwork::debugString() {
  list<string> colors;
  list<string> colorstmp;
  colors.push_back("coral");
  colors.push_back("beige");
  colors.push_back("darkorange");
  colors.push_back("darkgoldenrod");
  colors.push_back("chartreuse");
  colors.push_back("aquamarine");
  colors.push_back("aliceblue");
  colors.push_back("blueviolet");
  colors.push_back("crimson");
  colors.push_back("brown");
  colors.push_back("orange");
  colors.push_back("gold");
  colors.push_back("darkgreen");
  colors.push_back("cyan");
  colors.push_back("blue");
  colors.push_back("darkorchid");
  colors.push_back("darksalmon");
  colors.push_back("burlywood");
  colors.push_back("orangered");
  colors.push_back("goldenrod");
  colors.push_back("darkolivegreen");
  colors.push_back("darkturquoise");
  colors.push_back("cadetblue");
  colors.push_back("magenta");
  colors.push_back("deeppink");
  colors.push_back("chocolate");
  colors.push_back("greenyellow");
  colors.push_back("darkseagreen");
  colors.push_back("lightcyan");
  colors.push_back("cornflowerblue");
  colors.push_back("mediumorchid");
  colors.push_back("firebrick");
  colors.push_back("darkkhaki");
  colors.push_back("lightgoldenrod");
  colors.push_back("forestgreen");
  colors.push_back("mediumaquamarine");
  colors.push_back("darkslateblue");
  colors.push_back("mediumpurple");

  string res =
      "digraph hopfield{\n\tnode [shape=plaintext, fontsize=10];\n\tedge "
      "[arrowhead=none];\n";
  for (list<pair<double, vector<HopfieldNodePtr> > >::iterator link =
           connections.begin();
       link != connections.end(); link++) {
    string current_color = colors.front();
    colorstmp.push_back(current_color);
    colors.pop_front();
    if (colors.empty()) colors = colorstmp;
    if ((*link).second.size() == 1)
      res += "\t" + (*link).second[0]->getId() + ":s->" +
             (*link).second[0]->getId() + ":n [color=" + current_color + "];\n";
    else {
      string first = (*link).second[0]->getId();
      string last =
          first;  // = (*link).second[(*link).second.size() - 1]->getId();
      for (unsigned int i = 1; i < (*link).second.size(); i++) {
        res += "\t" + last + "->" + (*link).second[i]->getId() +
               " [color=" + current_color + "];\n";
        last = (*link).second[i]->getId();
      }
      res += "\t" + last + "->" + first + " [color=" + current_color + "];\n";
    }
  }
  res += "}";
  return res;
}
#endif

}  // namespace neural

}  // namespace nalso
