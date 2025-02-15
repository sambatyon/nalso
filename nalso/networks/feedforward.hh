#pragma once
/**
 * @file feedforward.h
 *
 * @brief Feed forward neural network declaration.
 *
 * Contains the declaration of the feed forward neural network class as well as
 * the needed auxiliary definitions.
 *
 * @date Oct 10, 2009
 * @author Alexander Rojas <alexander.rojas@gmail.com>
 */

#include <map>
#include <vector>

#include "nalso/networks/method.hh"
#include "nalso/networks/neuralnetwork.hh"
#include "nalso/networks/node.hh"

namespace nalso {
namespace neural {

typedef std::pair<std::string, NeuralEndPtr> ProxyElem;

/**
 * @brief A feed forward neural network
 *
 * Provides a fast and easy to use implementation of a Feed Forward neural
 * network. Plus it can have many subnetworks which are to some extend
 * independent from each others. The code is based on the neural network class
 * used in weka but stripped down of all the data management. It doesn't
 * implemente either the training phase althoug it would be relatively easy to
 * do.
 *
 * @todo Learning algorithms such as backpropagation
 */
class FeedForwardNeuralNetwork : public NeuralNetwork {
 private:
  std::map<std::string, NeuralEndPtr> inputs, outputs;
  std::vector<std::vector<NeuralNodePtr> > nodes;

 public:
  /**
   * Creates an empty feed forward neural network.
   */
  FeedForwardNeuralNetwork();
  virtual ~FeedForwardNeuralNetwork();

  /**
   * Adds the given node to the specified subnetwork. If the node passed has a
   * type input, then a NeuralEnd object is added to the inputs, similarly if
   * the node has type output, then a NeuralEnd object is added to the outputs
   * map. The label of this newly created NeuralEnd is
   * w$subnetwork-$node.getId(), for example a neural node in subnetwork 3 with
   * id myNode of type input will create a NeuralEnd in the input map with id
   * w3-myNode.
   *
   * @param node A pointer to the node to be added.
   *
   * @param subNetwork The subnetwork where the node will be added
   */
  void addNode(NeuralNodePtr node, int subNetwork = 0);

  /**
   * Adds an input/output neural end to the network.
   *
   * @param node The node to be added.
   *
   * @param input if true the node is considered to be an input node, if false,
   * it is considered to be an output node.
   */
  void addEndNode(NeuralEndPtr node, bool input = true) {
    if (input) {
      inputs.insert(make_pair((*node).getId(), node));
    } else {
      outputs.insert(make_pair((*node).getId(), node));
    }
  }

  /**
   * Allocates space for a new subnetwork.
   */
  void allocateSubnetwork() {
    std::vector<NeuralNodePtr> tmp;
    nodes.push_back(tmp);
  }

  /**
   * Creates a connection between the given units with a weight value passed as
   * parameter.
   *
   * @param source A pointer to the source node.
   *
   * @param dest A pointer to the destination node.
   *
   * @param weight The weight of the connection.
   *
   * @return true if the connection was successfully created, false if it
   * couldn't be created.
   */
  bool connectNodes(NeuralConnectionPtr source, NeuralConnectionPtr dest,
                    double weight = 1);
  /**
   * Search for the units identified with the given names and connects them with
   * the given weight value. Since different subnetworks can have units with the
   * same name, the subnetwork of the nodes must be known. If not, the method
   * will search only in the default subnetwork.
   */
  bool connectNodes(std::string sourceId, std::string destId, double weight = 1,
                    int subNetSource = 0, int subNetDest = 0);

  virtual ParamsMap evaluate(ParamsMap& input);

  /**
   * Returns a reference to the std::vector that represents the subnetwork i.
   *
   * @param index of the network to access
   *
   * @return a reference to the std::vector that contains the nodes of the subnetwork
   * at index.
   */
  std::vector<NeuralNodePtr>& operator[](unsigned int index) {
    return nodes[index];
  }
  /**
   * Return the number of subnetworks allocated in the network.
   *
   * @return number of subnetworks allocated in this network.
   */
  unsigned int noSubNN() { return nodes.size(); }

  /**
   * calls the reset value of all nodes in the network. Useful when more than
   * one input is going to be evaluated.
   */
  void reset();

#ifdef DEBUG
  virtual std::string debugString();
#endif
};

}  // namespace neural
}  // namespace nalso
