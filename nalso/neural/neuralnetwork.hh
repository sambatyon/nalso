#pragma once
/**
 * @file neuralnetwork.h
 *
 * @brief definition of an abstract neural network
 *
 * The basic neural network object.
 *
 * @date Oct 10, 2009
 *
 * @author Alexander Rojas <alexander.rojas@gmail.com>
 */

#include <cmath>
#include <map>
#include <memory>
#include <string>

namespace nalso {
/**
 * @brief Container of all the neural networks related classes.
 */
namespace neural {
/**
 * Represents a set of pairs such that the key is the label of an input or
 * output node and the value is the value of that node.
 */
typedef std::map<std::string, double> ParamsMap;

/**
 * @brief Represents an abstract neural network.
 *
 * Since the project uses at least to kinds of different neural networks and we
 * expect to have an unified framework where each algorithm behaves in a similar
 * way. This class provides the reference for this framework.
 *
 * Implementations are only asked to implement the method evaluate which
 * receives a map<string, double> and return a map<string, double> whose string
 * attribute are the labels of the inputs and output nodes respectively.
 *
 * @author Alexander Rojas
 */
class NeuralNetwork {
 public:
  virtual ~NeuralNetwork() {};

  /**
   * For each pair p in the input parameters map, and assuming there's an input
   * node with label p.first, sets the value p.second to the input node labeled
   * p.first. After setting the value, computes the outputs values which are
   * stores in the returned parameters map such that the node with label
   * result.first contains the value result.second
   *
   * @param input A map of doubles which uses strings as keys, they keys must
   * have a corresponding input node, to which the value will be set to the one
   * in input[key]
   *
   * @return A map which uses the labels of the output neurons as strings and
   * the values represent the output of the neuron.
   */
  virtual ParamsMap evaluate(ParamsMap& input) = 0;

  /**
   * This method computes the fixpoint of a network starting with the ParamsMap
   * input. It should be the standard call for each use of hopfield neural
   * networks. The fixpoint is computing by feeding the network with the given
   * parameters, taking the output and then applying the output as an input and
   * repeating this process until each element in the output ParamsMap is equal
   * (or the absolute value of their difference is less or equal to the
   * tolerance parameter) to its equivalent element in the input (i.e. a
   * fixpoint is reached).
   *
   * @remarks Only works when the input layer and the output one have nodes with
   * the same labels.
   *
   * @param input A parameters map (a pair <string, double>) containing the set
   * of initial values to feed the network.
   *
   * @param tolerance The minimum acceptable difference to consider two elements
   * equal.
   *
   * @return The ParamsMap containing the fixpoint of the network.
   */
  virtual ParamsMap findFixPoint(ParamsMap input, double tolerance = 0) {
    ParamsMap newSet = input;
    ParamsMap oldSet;
    do {
      oldSet = newSet;
      newSet = evaluate(oldSet);
    } while (!NeuralNetwork::areEqualParameters(newSet, oldSet, tolerance));

    return newSet;
  }

  /**
   * Compare wether two ParamsMap are equal. Two ParamsMap's are considered to
   * be equal iff the absolute value of the difference between the elements that
   * share the same key are less than the tolerance factor.
   *
   * @remark Since it is the absolute value that is compare the order in which
   * the params are passed does not matter.
   *
   * @remark In order to improve speed, it is assumed that both paramsMaps
   * contain the same keyset.
   *
   * @param first One of the two params map to be compared.
   *
   * @param second One of the two params map to be compared.
   *
   * @return true if the absolute value of the difference of each to elements
   * with the same key is less than the tolerance value, false otherwise.
   */
  static bool areEqualParameters(ParamsMap first, ParamsMap second,
                                 double tolerance = 0) {
    for (ParamsMap::iterator it = first.begin(); it != first.end(); it++)
      if (std::abs((*it).second - second[(*it).first]) > tolerance)
        return false;
    return true;
  }

#ifdef DEBUG
  virtual std::string debugString() = 0;
#endif
};

typedef std::shared_ptr<NeuralNetwork> NeuralNetworkPtr;

}  // namespace neural
}  // namespace nalso
