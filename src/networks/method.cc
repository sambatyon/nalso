/*
 * @file method.cpp
 *
 * @date Oct 10, 2009
 * @author Alexander Rojas <alexander.rojas@gmail.com>
 */

#include "method.hh"

namespace nalso {

namespace neural {

double LinearMethod::outputValue(NeuralNode& node) {
  vector<double>& weights = node.getWeights();
  vector<NeuralConnectionPtr>& inputs = node.getInputs();
  double value = weights[0];
  for (unsigned int i = 0; i < inputs.size(); i++)
    value += inputs[i]->outputValue(true) * weights[i + 1];
  return value;
}

double LinearMethod::errorValue(NeuralNode& node) {
  vector<NeuralConnectionPtr>& outputs = node.getOutputs();
  vector<int>& oNums = node.getOutputNums();
  double error = 0;

  for (unsigned int i = 0; i < outputs.size(); i++)
    error += outputs[i]->errorValue(true) * outputs[i]->weightValue(oNums[i]);

  return error;
}

void LinearMethod::updateWeights(NeuralNode& node, double learn,
                                 double momentum) {
  vector<NeuralConnectionPtr>& inputs = node.getInputs();
  vector<double>& cWeights = node.getChangeInWeights();
  vector<double>& weights = node.getWeights();

  double learnTimesError = learn * node.errorValue(false);

  double c = learnTimesError + momentum * cWeights[0];
  weights[0] += c;
  cWeights[0] = c;

  for (unsigned int i = 1; i < inputs.size() + 1; i++) {
    c = learnTimesError * inputs[i - 1]->outputValue(false);
    c += momentum * cWeights[i];

    weights[i] += c;
    cWeights[i] = c;
  }
}

double SigmoidMethod::outputValue(NeuralNode& node) {
  vector<double>& weights = node.getWeights();
  vector<NeuralConnectionPtr>& inputs = node.getInputs();
  double value = weights[0];
  for (unsigned int i = 0; i < inputs.size(); i++)
    value += inputs[i]->outputValue(true) * weights[i + 1];

  if (value < -45)
    value = 0;
  else if (value > 45)
    value = 1;
  else
    value = 1 / (1 + exp(-value));

  return value;
}

double SigmoidMethod::errorValue(NeuralNode& node) {
  vector<NeuralConnectionPtr>& outputs = node.getOutputs();
  vector<int>& oNums = node.getOutputNums();
  double error = 0;

  for (unsigned int i = 0; i < outputs.size(); i++)
    error += outputs[i]->errorValue(true) * outputs[i]->weightValue(oNums[i]);

  double value = node.outputValue(false);
  error *= value * (1 - value);
  return error;
}

void SigmoidMethod::updateWeights(NeuralNode& node, double learn,
                                  double momentum) {
  vector<NeuralConnectionPtr>& inputs = node.getInputs();
  vector<double>& cWeights = node.getChangeInWeights();
  vector<double>& weights = node.getWeights();

  double learnTimesError = learn * node.errorValue(false);

  double c = learnTimesError + momentum * cWeights[0];
  weights[0] += c;
  cWeights[0] = c;

  for (unsigned int i = 1; i < inputs.size() + 1; i++) {
    c = learnTimesError * inputs[i - 1]->outputValue(false);
    c += momentum * cWeights[i];

    weights[i] += c;
    cWeights[i] = c;
  }
}

double StepMethod::outputValue(NeuralNode& node) {
  vector<double>& weights = node.getWeights();
  vector<NeuralConnectionPtr>& inputs = node.getInputs();
  double value = 0;  // weights[0];
  for (unsigned int i = 0; i < inputs.size(); i++)
    value += inputs[i]->outputValue(true) * weights[i + 1];

  // LOG4CXX_DEBUG(logger, node.getId() << ": " << value);

  return value >= weights[0] ? 1.0 : 0.0;
}

void StepMethod::updateWeights(NeuralNode& node, double learn,
                               double momentum) {
  vector<NeuralConnectionPtr>& inputs = node.getInputs();
  vector<double>& cWeights = node.getChangeInWeights();
  vector<double>& weights = node.getWeights();

  double learnTimesError = learn * node.errorValue(false);

  double c = learnTimesError + momentum * cWeights[0];
  weights[0] += c;
  cWeights[0] = c;

  for (unsigned int i = 1; i < inputs.size() + 1; i++) {
    c = learnTimesError * inputs[i - 1]->outputValue(false);
    c += momentum * cWeights[i];

    weights[i] += c;
    cWeights[i] = c;
  }
}

double BipolarSemilinearMethod::outputValue(NeuralNode& node) {
  vector<double>& weights = node.getWeights();
  vector<NeuralConnectionPtr>& inputs = node.getInputs();
  double value = weights[0];
  for (unsigned int i = 0; i < inputs.size(); i++) {
    value += inputs[i]->outputValue(true) * weights[i + 1];
  }

  if (value < -45)
    value = 0;
  else if (value > 45)
    value = 1;
  else
    value = (2 / (1 + exp(-beta * value))) - 1;

  return value;
}

double BipolarSemilinearMethod::errorValue(NeuralNode& node) {
  vector<NeuralConnectionPtr>& outputs = node.getOutputs();
  vector<int>& oNums = node.getOutputNums();
  double error = 0;

  for (unsigned int i = 0; i < outputs.size(); i++)
    error += outputs[i]->errorValue(true) * outputs[i]->weightValue(oNums[i]);

  double value = node.outputValue(false);
  error *= 2 * beta * value * (1 - value);
  return error;
}

void BipolarSemilinearMethod::updateWeights(NeuralNode& node, double learn,
                                            double momentum) {
  vector<NeuralConnectionPtr>& inputs = node.getInputs();
  vector<double>& cWeights = node.getChangeInWeights();
  vector<double>& weights = node.getWeights();

  double learnTimesError = learn * node.errorValue(false);

  double c = learnTimesError + momentum * cWeights[0];
  weights[0] += c;
  cWeights[0] = c;

  for (unsigned int i = 1; i < inputs.size() + 1; i++) {
    c = learnTimesError * inputs[i - 1]->outputValue(false);
    c += momentum * cWeights[i];

    weights[i] += c;
    cWeights[i] = c;
  }
}

}  // namespace neural

}  // namespace nalso
