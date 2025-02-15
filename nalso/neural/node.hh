#pragma once
/**
 * @file node.h
 *
 * @brief Definition of the different kinds of nodes.
 *
 * Defines all the basic nodes classes using in the feed forward neural
 * networks.
 *
 * @date Oct 10, 2009
 * @author Alexander Rojas <alexander.rojas@gmail.com>
 */
#include <ctime>
#include <memory>
#include <random>
#include <string>
#include <utility>
#include <vector>

#include "method.hh"

namespace nalso {
namespace neural {

class NeuralConnection;
/// Pointer to a NeuralConnection object
/// @see NeuralConnection
typedef std::shared_ptr<NeuralConnection> NeuralConnectionPtr;

/**
 * Bitwise flags that describe the behavior of the nodes.
 */
enum ConnectionType {
  UNCONNECTED = 0,
  PURE_INPUT = 1,
  PURE_OUTPUT = 2,
  INPUT = 4,
  OUTPUT = 8,
  CONNECTED = 16
};

/**
 * @brief Defines the behavior of all types of nodes.
 *
 * An abstract class that defines the basic behavior for all kinds of nodes,
 * either input/output ones or hidden nodes. Every node class must inherit from
 * this class.
 */
class NeuralConnection {
 protected:
  /** List of incoming connections */
  std::vector<NeuralConnectionPtr> inputList;
  /** List of outgoing connections */
  std::vector<NeuralConnectionPtr> outputList;

  /** The value stored in the i-th element of these vector tells the index of
   * this unit in the outputList of the i-th node in the inputList */
  std::vector<int> inputNums;
  /** The value stored in the i-th element of these vector tells the index of
   * this unit in the inputList of the i-th node in the outputList */
  std::vector<int> outputNums;

  double unitValue, unitError;

  bool weightsUpdated; /** keeps track of which units have updated its weights
                          already after each iteration */

  std::string id; /** Unique identifier of the node inside a network */

  /**
   * Describes the behavior of this node using the values described in
   * ConnectionType
   *
   * @see ConnectionType
   */
  int type;

 public:
  NeuralConnection(std::string _id);
  virtual ~NeuralConnection();

  /**
   * Getter of the id attribute.
   *
   * @return the unique id of the node
   */
  std::string getId() { return id; }
  /**
   * Getter of the type attribute.
   *
   * @return the type of the node.
   */
  int getType() { return type; }
  /**
   * Setter of the type attribute.
   *
   * @param _type the new value for the type attribute.
   */
  void setType(int _type) { type = _type; }
  /**
   * Returns the number of outgoing connections of this neuron.
   *
   * @return the number of outgoing connections of this neuron.
   */
  unsigned int getNumOutputs() { return outputList.size(); }
  /**
   * Returns the number of incoming connections of this node.
   *
   * @return the number of incoming connections of this node.
   */
  unsigned int getNumInputs() { return inputList.size(); }

  /**
   * Offers access to the elements outputList vector.
   *
   * @param index The index of the requiered neural connection.
   *
   * @return A pointer to the neural connection at position index
   */
  NeuralConnectionPtr getOutput(int index) { return outputList[index]; }
  /**
   * Offers access to the elements inputList vector.
   *
   * @param index The index of the required neural connection in the inputList
   * vector.
   *
   * @return A pointer to the neural connection at position index.
   */
  NeuralConnectionPtr getInput(int index) { return inputList[index]; }

  /**
   * Compares the memory addresses of this and the other connections.
   *
   * @param other The other connection to be compared.
   *
   * @return true if the two objects are actually the same object (share the
   * same address).
   */
  bool operator==(NeuralConnection& other) const;
  /**
   * Returns the negation of the equality operator.
   *
   * @param other The other connection to be compared.
   *
   * @return false if the two objects are actually the same object (share the
   * same address).
   */
  bool operator!=(NeuralConnection& other) const { return !(*this == other); };

  /**
   * Call this to reset the unit for another run. It is expected by that this
   * unit will call the reset functions of all input units to it. It is also
   * expected that this will not be done if the unit has already been reset
   * (or atleast appears to be).
   */
  virtual void reset() = 0;
  /**
   * Call this to get the output value of this unit.
   *
   * @param calculate True if the value should be calculated.
   *
   * @return The output value, or NaN, if the value has not been calculated.
   */
  virtual double outputValue(bool calculate) = 0;
  /**
   * Call this to get the error value of this unit.
   *
   * @param calculate True if the value should be calculated.
   *
   * @return The error value, or NaN, if the value has not been calculated.
   */
  virtual double errorValue(bool calculate) = 0;
  /**
   * Call this to save the current values of the node incoming weights.
   */
  virtual void saveWeights() = 0;
  /**
   * Call this to restore the previously saved incoming weights.
   */
  virtual void restoreWeights() = 0;

  /**
   * Call this to get the weight value on a particular connection.
   *
   * @param _index The connection number to get the weight for, -1 if The
   * threshold weight should be returned.
   *
   * @return This function will default to return 1. If overridden, it should
   *         return the value for the specified connection or if -1 then it
   *         should return the threshold value. If no value exists for the
   *         specified connection, NaN will be returned.
   */
  virtual double weightValue(int _index) { return 1; }

  /**
   * Call this function to update the weight values at this unit. After the
   * weights have been updated at this unit, All the input connections will
   * then be called from this to have their weights updated.
   *
   * @param _learning The learning Rate to use.
   *
   * @param _momentum The momentum to use.
   */
  virtual void updateWeights(double _learning, double _momentum);

  /**
   * Use this to get easy access to the inputs. It is not advised to change
   * the entries in this list (use the connecting and disconnecting functions
   * to do that)
   * @return A reference to the inputs list.
   */
  std::vector<NeuralConnectionPtr>& getInputs() { return inputList; }

  /**
   * Use this to get easy access to the outputs. It is not advised to change
   * the entries in this list (use the connecting and disconnecting functions
   * to do that)
   * @return a reference to the outputs list.
   */
  std::vector<NeuralConnectionPtr>& getOutputs() { return outputList; }

  /**
   * Use this to get easy access to the input numbers. It is not advised to
   * change the entries in this list (use the connecting and disconnecting
   * functions to do that)
   *
   * @return A reference to the input nums list.
   */
  std::vector<int>& getInputNums() { return inputNums; }

  /**
   * Use this to get easy access to the output numbers. It is not advised to
   * change the entries in this list (use the connecting and disconnecting
   * functions to do that)
   *
   * @return The outputs list.
   */
  std::vector<int>& getOutputNums() { return outputNums; }

  /**
   * This method will remove all the inputs to this unit. In doing so it
   * will also terminate the connections at the other end.
   */
  virtual void removeAllInputs();

  /**
   * This method will remove all outputs to this unit. In doing so it will
   * also terminate the connections at the other end.
   */
  void removeAllOutputs();

  /**
   * @brief connects two nodes.
   *
   * Creates a link between the node s and the node t with weight
   * <code>weight</code> if such a connection is possible, for example,
   * connecting an output neural end to a hidden neuron is not possible.
   *
   * Invoking this method will update the inputList vector for s and the
   * outputList vector of t.
   *
   * The default value for weight is NaN and the method can be called without
   * the parameter in connections involving neural ends.
   *
   * @param s The source of the connection. After a successfully connection,
   * this node will have one more node (t) in the outputList.
   *
   * @param t The destination of the connection. After a successfully
   * connection, this node will have one more node (s) in the inputList.
   *
   * @param weight The weight of the connection between s and t
   *
   * @return true if the connection was created. False otherwise.
   */
  static bool connect(NeuralConnectionPtr s, NeuralConnectionPtr t,
                      double weight = NAN);
  /**
   * Disconnects two previously connected nodes if that connection exists. It
   * updates the values of the outputList of s and the inputList of t
   *
   * @param s The source node of the connection to be deleted.
   *
   * @param t the destination node of the connection to be deleted.
   *
   * @return true if the connection was deleted. false if the connection
   * couldn't be deleted or if it didn't exists.
   */
  static bool disconnect(NeuralConnectionPtr s, NeuralConnectionPtr t);

  /**
   * Updates the value of the input num vector at position index.
   *
   * @param index The index of the element to be updated.
   *
   * @param value The new value for inputNums[index]
   */
  void changeInputNum(int index, int value);
  /**
   * Updates the value of the output num vector at position index.
   *
   * @param index The index of the element to be updated.
   *
   * @param value The new value for outputNums[index]
   */
  void changeOutputNum(int index, int value);

  /**
   * Returns the index of the layer the neuron belongs to. It is deprecated
   * since this version supports no layers.
   *
   * @return The layer this node belongs to.
   *
   * @deprecated
   */
  virtual int getLayer() { return 0; }
  /**
   * Returns the index of the subnetwork the neuron belongs to. It is deprecated
   * since this version supports no subnetworks.
   *
   * @return The subnetwork this node belongs to.
   *
   * @deprecated
   */
  virtual int getSubnetwork() { return 0; }

 protected:
  /**
   * This will connect the specified unit to be an input to this unit.
   *
   * @param i The node to be added to the inputList vector.
   *
   * @param n connection number for this connection.
   *
   * @param weight the weight value of the connection.
   *
   * @return True if the connection was made, false otherwise.
   */
  virtual bool connectInput(NeuralConnectionPtr i, int n, double weight = NAN);

  /**
   * This will connect the specified unit to be an output to this unit.
   *
   * @param o The unit to be added to the outputList.
   *
   * @param n It's connection number for this connection.
   *
   * @return True if the connection was made, false otherwise.
   */
  bool connectOutput(NeuralConnectionPtr o, int n);

  /**
   * This will disconnect the input with the specific connection number From
   * this node (only on this end however).
   *
   * @param i The unit to disconnect.
   *
   * @param n The connection number at the other end, -1 if all the
   *            connections to this unit should be severed.
   *
   * @return True if the connection was removed, false if the connection was
   *         not found.
   */
  virtual bool disconnectInput(NeuralConnectionPtr i, int n);

  /**
   * This will disconnect the output with the specific connection number From
   * this node (only on this end however).
   *
   * @param o The unit to disconnect.
   *
   * @param n The connection number at the other end, -1 if all the
   *            connections to this unit should be severed.
   *
   * @return True if the connection was removed, false if the connection was
   *         not found.
   */
  bool disconnectOutput(NeuralConnectionPtr o, int n);

#ifdef DEBUG
 public:
  string debugString();
#endif
};

/**
 * @brief An input or output node.
 *
 * NeuralEnds are a special kind of nodes which makes no real computation and
 * are used only a proxies to feed and read values to and from the network.
 */
class NeuralEnd : public NeuralConnection {
 private:
  bool input;
  double expected;

 public:
  /**
   * Creates a new neural end with the given id. If _input is true, then this
   * neural end will behave like a input node, it will behave like an output
   * node otherwise.
   *
   * @param _id the unique identifier for this node.
   *
   * @param _input true if the new node is to be an input node, false if it is
   * to behave like an output one.
   */
  NeuralEnd(std::string _id, bool _input = true);
  virtual ~NeuralEnd() {}

  /**
   * For output nodes, this makes the whole network to compute the values fed in
   * the input units. For input nodes makes not real sense and just returns 0.
   *
   * @param compute if true compute recursively the value of the units connected
   * to this.
   */
  double outputValue(bool compute);
  /**
   * Computes the value of an output node (make no sense for input nodes).
   *
   * @param compute if true compute recursively the value of the units connected
   * to this.
   */
  double errorValue(bool compute);

  /**
   * @see NeuralConnection#reset()
   */
  void reset();
  /**
   * @see NeuralConnection#saveWeights()
   */
  void saveWeights();
  /**
   * @see NeuralConnection#restoreWeights()
   */
  void restoreWeights();

  /**
   * For an input unit unit sets the value that it should return when asked for
   * the output value.
   *
   * @param _outputValue The value of the unit.
   */
  void setOutputValue(double _outputValue) {
    if (input) unitValue = _outputValue;
  }
  /**
   * Sets the expected value for output units. Needed to compute the unit error.
   *
   * @param _expected expected value of the unit.
   */
  void setExpectedValue(double _expected) {
    if (!input) expected = _expected;
  }
  /**
   * Return the expected value of the unit if the unit is an output unit, NaN in
   * other case.
   *
   * @return NaN if it is an input node, expected otherwise
   */
  double getExpectedValue() { return input ? NAN : expected; }
};

/// Pointer to a neuralEnd object
/// @see NeuralEnd
typedef std::shared_ptr<NeuralEnd> NeuralEndPtr;

class NeuralMethod;
/// Pointer to a NeuralMethod object
/// @see nalso::neural::NeuralMethod
typedef std::shared_ptr<NeuralMethod> NeuralMethodPtr;

struct Random {
  std::mt19937 generator;
  std::uniform_real_distribution<> dist;

  double operator()() {
    return dist(generator);
  }
};
typedef std::shared_ptr<Random> RandomPtr;

/**
 * @brief Hidden nodes.
 *
 * The nodes that realize the actual computation inside a neural network are the
 * neural nodes. These ones contain a complement to the input list, meaning the
 * weights, used to compute the weighted sum of inputs. It also uses neural
 * methods to compute its output.
 *
 * @author Alexander Rojas
 */
class NeuralNode : public NeuralConnection {
 private:
  /**
   * Vector used to store the weights of the incoming connections to this node.
   * Special case is the element at position 0 which is the bias value.
   * Therefore, the size of this vector is always one more than the size of the
   * inputList vector.
   */
  std::vector<double> weights;
  std::vector<double> bestWeights, changeInWeights;

  RandomPtr random;

  NeuralMethodPtr method;

  int layer, subnetwork;

 public:
  /**
   * Creates a NeuralNode object with the given identifier and method.
   *
   * @param _id Unique identifier for the neuron inside the network.
   *
   * @param _method Activation function for the current unit.
   */
  NeuralNode(std::string _id, NeuralMethodPtr _method);
  /**
   * Creates a NeuralNode object with the given identifier and method.
   *
   * @param _id Unique identifier for the neuron inside the network.
   *
   * @param _method Activation function for the current unit.
   *
   * @param _random Random generator object to initialize weights and bias when
   * training
   */
  NeuralNode(std::string _id, NeuralMethodPtr _method, RandomPtr _random);
  virtual ~NeuralNode() {}

  /**
   * Setter of the method attribute.
   *
   * @param _method A pointer to an activation function object.
   */
  void setMethod(NeuralMethodPtr _method) { method = _method; }
  /**
   * Getter of the method attribute.
   *
   * @return a pointer to the method object.
   */
  NeuralMethodPtr getMethod() { return method; }

  /**
   * If the parameter compute is true, computes the new output value for the
   * node. If it is false, returns what its already save.
   *
   * @param compute specifies if the output value should be computed or if the
   * previously computed value should be used.
   *
   * @return The output value of this unit.
   */
  double outputValue(bool compute);
  /**
   * If the parameter compute is true, computes the new error value for the
   * node. If it is false, returns what its already save.
   *
   * @param compute specifies if the error value should be computed or if the
   * previously computed value should be used.
   *
   * @return The error value of this unit.
   */
  double errorValue(bool compute);
  void saveWeights();
  void restoreWeights();
  void reset();

  /**
   * Returns the weight of the connection between the node in the inputList at
   * index <code>index</code> and this unit.
   *
   * @remark When the parameter index is equal zero, it will return the element
   * at weights[1], i.e. it does not take into account the bias. The value of
   * index, then, is between zero and 1 - size of the incoming connections.
   *
   * @return the weight of the connection between the incoming node at position
   * index and this node.
   */
  double weightValue(int index);
  /**
   * Getter for the weights attribute.
   *
   * @see weights
   *
   * @return a reference to the weights attribute.
   */
  std::vector<double>& getWeights() { return weights; }
  /**
   * Getter for the changeInWeights attribute.
   *
   * @return a reference to the changeInWeights attribute.
   */
  std::vector<double>& getChangeInWeights() { return changeInWeights; }

  void updateWeights(double learning, double momentum);

  void removeAllInputs();

  /**
   * Returns the value of weights at position zero. which is the bias.
   *
   * @return the bias value of the node.
   */
  double getBias() { return weights[0]; }
  /**
   * setter of the bias property
   *
   * @param bias the new value for the bias of the node.
   */
  void setBias(double bias) { weights[0] = bias; }

  /**
   * Setter for the layer property.
   *
   * @param _layer The layer this node belongs to.
   *
   * @deprecated
   */
  void setLayer(int _layer) { layer = _layer; }
  /**
   * Getter for the layer property.
   *
   * @return _layer The layer this node belongs to.
   *
   * @deprecated
   */
  int getLayer() { return layer; }

  /**
   * Setter for the subnetwork property.
   *
   * @param _subnetwork The subnetwork this node belongs to.
   *
   * @deprecated
   */
  void setSubnetwork(int _subnetwork) { subnetwork = _subnetwork; }
  /**
   * Getter for the subnetwork property.
   *
   * @return The subnetwork this node belongs to.
   *
   * @deprecated
   */
  int getSubnetwork() { return subnetwork; }

 protected:
  bool connectInput(NeuralConnectionPtr i, int n, double weight = NAN);
  bool disconnectInput(NeuralConnectionPtr i, int n);
};

/// Pointer to a neural node object
/// @see NeuralNode
typedef std::shared_ptr<NeuralNode> NeuralNodePtr;

}  // namespace neural
}  // namespace nalso
