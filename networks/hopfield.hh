/**
 * @file hopfield.h
 *
 * Contains the definition of the elements needed for a Hopfield network
 * including the network itself.
 *
 * @date Oct 16, 2009
 * @author Alexander Rojas <alexander.rojas@gmail.com>
 */

#ifndef HOPFIELD_H_
#define HOPFIELD_H_

#include <boost/shared_ptr.hpp>
#include <cmath>
#include <list>
#include <map>
#include <vector>

#include "neuralnetwork.hh"

namespace nalso {

namespace neural {
using namespace boost;
using namespace std;

class HopfieldNode;
typedef shared_ptr<HopfieldNode> HopfieldNodePtr;

/**
 * A hyper connection is a pair (doube, list of nodes) whose first parameter is
 * the weight of the connection and the second is a list of pointers to the
 * involved nodes.
 */
typedef pair<double, list<HopfieldNodePtr> > HyperConnection;
typedef shared_ptr<HyperConnection> HyperConnectionPtr;

/**
 * @brief A continuous hyper degree hopfield node
 *
 * A Hopfield node which is continuous and uses hyper connections.
 *
 * @author Alexander Rojas
 */
class HopfieldNode {
 private:
  string id;
  list<HyperConnectionPtr> inputs;
  double potential;
  double nextPotential;
  shared_ptr<double> coolingFactor;

 public:
  /**
   * Creates a hopfield node with the given id and a pointer to a double so the
   * cooling factor can be managed from the network itself.
   *
   * @param _id the unique identifier of the node.
   * @param _coolingFactor A pointer to a double which indicates how fast the
   * neuron should slow down. It is a pointer, so the cooling ration can be
   * managed from the network itself.
   */
  HopfieldNode(string _id, shared_ptr<double> _coolingFactor)
      : id(_id),
        potential(NAN),
        nextPotential(NAN),
        coolingFactor(_coolingFactor) {};

  /**
   * Creates a hopfield node with the given id and a pointer to a double so the
   * cooling factor can be managed from the network itself.
   *
   * @param _id the unique identifier of the node.
   * @param _coolingFactor A pointer to a double which indicates how fast the
   * neuron should slow down. It is a pointer, so the cooling ration can be
   * managed from the network itself.
   */
  HopfieldNode(string _id) : id(_id), potential(NAN), nextPotential(NAN) {};

  /**
   * Creates a hopfield node with the given id and a pointer to a double so the
   * cooling factor can be managed from the network itself.
   *
   * @param _id the unique identifier of the node.
   * @param _coolingFactor A pointer to a double which indicates how fast the
   * neuron should slow down. It is a pointer, so the cooling ration can be
   * managed from the network itself.
   */
  // HopfieldNode(const char* _id, shared_ptr<double> _coolingFactor):id(_id),
  // potential(NAN), nextPotential(NAN), coolingFactor(_coolingFactor){};

  /**
   * Returns the output value of the unit, which is the sigmoid(potential /
   * coolingFactor)
   */
  double outputValue() { return 1 / (1 + exp(-potential / *coolingFactor)); }

  /**
   * Compute the next value for the potential of the unit. Which is the addition
   * of each connection weight times the output of the units in this connection
   */
  void computeNextPotential();

  /**
   * Add this unit to a hyper connection, adding a pointer to itself to the
   * given connection and putting the connection into the connection list.
   *
   * @param conn The connection where this unit will be added.
   *
   * @param _this a pointer to this which is managed from outside in order to
   * avoid pointer count problems.
   */
  void addToHyperConnection(HyperConnectionPtr conn,
                            HopfieldNodePtr _this = HopfieldNodePtr()) {
    if (_this.get())
      (*conn).second.push_back(_this);
    else
      (*conn).second.push_back(HopfieldNodePtr(this));
    inputs.push_back(conn);
  };

  /**
   * Sets the value of the potential. Used as the input of the network when
   * looking for a fixpoint
   *
   * @param _potential The potential value of the network.
   */
  void setInitialPotential(double _potential) { potential = _potential; };

  /**
   * Sets the potential value to nextPotential and nextPotential to null. The
   * computation of the potential does not automatically update the value in
   * order to allow all units to update its potential before it is updated in
   * all. It is a way to simulate the simultaneous update of the model proposed
   * by hopfield.
   */
  void updatePotential() {
    potential = nextPotential;
    nextPotential = NAN;
  }

  /**
   * Returns the value of the computed next potential
   *
   * @return nextPotential value
   */
  double getNextPotential() { return nextPotential; }
  /**
   * Getter of the potential value
   *
   * @return the value hold in potential
   */
  double getPotential() { return potential; }
  string getId() { return id; }
  shared_ptr<double> getCoolingFactor() { return coolingFactor; }
  void setCoolingFactor(shared_ptr<double> _factor) { coolingFactor = _factor; }
};

/**
 * @brief Hopfield network implementation
 *
 * Provides a fast and very simple and small implementation of a continuous high
 * order hopfield neural network. It implements the method evaluate but it makes
 * not much sense. One should use the overrieded method findFixPoint which
 * applies the Hopfield stabilization algorithm.
 *
 * @author Alexander Rojas
 */
class HopfieldNeuralNetwork : public nalso::neural::NeuralNetwork {
 private:
#ifdef DEBUG
  list<pair<double, vector<HopfieldNodePtr> > > connections;
#endif
  map<string, HopfieldNodePtr> nodes;
  shared_ptr<double> coolingFactor;

 public:
  HopfieldNeuralNetwork(double initCool = 1)
      : coolingFactor(new double(initCool)) {};

  /**
   * Adds a node to the network and sets its cooling factor pointer.
   *
   * @param _node the node to be added.
   */
  void addNode(HopfieldNodePtr _node) {
    (*_node).setCoolingFactor(coolingFactor);
    nodes.insert(make_pair((*_node).getId(), _node));
  }

  /**
   * Creates a hyperedge that connect all the nodes in _nodes with a weight
   * value weight.
   *
   * @param _nodes A vector of pointers to the nodes that will be part of the
   * new hyperedge
   *
   * @param weight the weight of the hyperedge
   */
  void connectNodes(vector<HopfieldNodePtr> _nodes, double weight = 1);

  /**
   * Search for the nodes with the labels contained in the vector _nodeNames. If
   * a node with a given label is not found, then it is ignored without
   * prompting an error.
   *
   * @param _nodeNames A vector containing the ids of the nodes that will be
   * part of the new hyperedge
   *
   * @param weight the weight of the connection.
   */
  void connectNodes(vector<string> _nodeNames, double weight = 1);

  virtual ParamsMap evaluate(ParamsMap& input);

  virtual ParamsMap findFixPoint(ParamsMap input, double tolerance = 0);

  void setCooling(double cooling) { *coolingFactor = cooling; }
  double getCooling() { return *coolingFactor; }

#ifdef DEBUG
  virtual std::string debugString();
#endif
};

}  // namespace neural

}  // namespace nalso

#endif /* HOPFIELD_H_ */
