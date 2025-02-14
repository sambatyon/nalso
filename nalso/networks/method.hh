/**
 * @file method.h
 *
 * @brief Definition of different activation functions.
 *
 * Contains definitions of the most common (or at least here used) activation
 * functions plus the main interface used by the nodes.
 *
 * @date Oct 10, 2009
 * @author Alexander Rojas <alexander.rojas@gmail.com>
 */

#ifndef METHOD_H_
#define METHOD_H_

#include <cmath>
#include <vector>

#include "node.hh"

namespace nalso {

namespace neural {

class NeuralNode;

/**
 * @brief Interface for the activation and error functions.
 *
 * An interface that declared the required methods used by the classes in order
 * to compute activation functions and error functions over nodes.
 *
 * @author Alexander Rojas
 */
class NeuralMethod {
 public:
  /**
   * Computes the output value of the given node.
   *
   * @param node the node for which the output will be computed.
   *
   * @return The activation value of the node passed as parameter.
   */
  virtual double outputValue(NeuralNode& node) = 0;
  /**
   * This function computes what the error value of the given node.
   *
   * @param node The node for which the error value will be computed.
   *
   * @return The error value of the given node.
   */
  virtual double errorValue(NeuralNode& node) = 0;
  /**
   * This function will calculate what the change in weights should be and
   * also update them.
   *
   * @param node The node to update the weights for.
   *
   * @param learn The learning rate to use.
   *
   * @param momentum The momentum to use.
   */
  virtual void updateWeights(NeuralNode& node, double learn,
                             double momentum) = 0;
};

/**
 * @brief Linear activation function.
 *
 * A normal linear activation function. Returns the bias plus the the weighted
 * sum of incomming connections.
 */
class LinearMethod : public NeuralMethod {
 public:
  virtual ~LinearMethod() {}

  double outputValue(NeuralNode& node);
  double errorValue(NeuralNode& node);
  void updateWeights(NeuralNode& node, double learn, double momentum);
};

/**
 * @brief sigmoid activation function.
 *
 * A sigmoidal activation function, very common in neural networks. It returns
 * 1 / (1 + exp(-value)) where value is the sum of the weighted incoming
 * connections plus the bias value.
 */
class SigmoidMethod : public NeuralMethod {
 public:
  virtual ~SigmoidMethod() {};

  double outputValue(NeuralNode& node);
  double errorValue(NeuralNode& node);
  void updateWeights(NeuralNode& node, double learn, double momentum);
};

/**
 * @brief step activation function.
 *
 * A normal step activation function, returns 1 if the sum of the weighted value
 * is greater than the bias and zero otherwise.
 */
class StepMethod : public NeuralMethod {
 public:
  virtual ~StepMethod() {}

  double outputValue(NeuralNode& node);
  /**
   * Not implemented and return NaN since the Step function is not a
   * differentiable function, which is a requisite to compute the error of a
   * node.
   *
   * @return NaN
   */
  double errorValue(NeuralNode& node) { return NAN; };
  void updateWeights(NeuralNode& node, double learn, double momentum);
};

/**
 * @brief Bipolar semilinear activation function.
 *
 * The bipolar semilinear method has the same shape of a sigmoidal function but
 * is modified so it can take values between -1 and 1. It also has a parameter
 * that defines how sharp the function should be. High values of beta resemble
 * closely a step function (only that this is differentiable) low values of beta
 * resemble a linear activation function.
 *
 * The exact formula is:
 * 2 / (1 + exp(-beta * sum))) - 1
 */
class BipolarSemilinearMethod : public NeuralMethod {
 private:
  double beta;

 public:
  BipolarSemilinearMethod(double beta_ = 1.0) : beta(beta_) {}
  virtual ~BipolarSemilinearMethod() {};

  double outputValue(NeuralNode& node);
  double errorValue(NeuralNode& node);
  void updateWeights(NeuralNode& node, double learn, double momentum);
};

}  // namespace neural

}  // namespace nalso

#endif /* METHOD_H_ */
