/*
 * @file end.cpp
 *
 * @date Oct 10, 2009
 * @author Alexander Rojas <alexander.rojas@gmail.com>
 */

#include "node.hh"

namespace nalso {

namespace neural {

NeuralEnd::NeuralEnd(string _id, bool _input /*= true*/)
    : NeuralConnection(_id), input(_input), expected(NAN) {
  if (input)
    type = PURE_INPUT;
  else
    type = PURE_OUTPUT;
}

double NeuralEnd::outputValue(bool compute) {
  if (isnan(unitValue) && compute) {
    if (input)
      return 0;
    else {  // unit is an output
      unitValue = 0;
      for (unsigned int i = 0; i < inputList.size(); i++)
        unitValue += (*inputList[i]).outputValue(true);
    }
  }
  return unitValue;
}

double NeuralEnd::errorValue(bool compute) {
  if (!isnan(unitValue) && isnan(unitError) && compute) {
    if (input) {
      unitError = 0;
      for (unsigned int i = 0; i < outputList.size(); i++)
        unitError += outputList[i]->errorValue(true);
    } else
      unitError = expected - unitValue;
  }
  return unitError;
}

void NeuralEnd::reset() {
  if (!isnan(unitValue) || !isnan(unitError)) {
    unitValue = input ? unitValue : NAN;
    unitError = NAN;
    weightsUpdated = false;
    for (unsigned int i = 0; i < inputList.size(); i++) inputList[i]->reset();
  }
}

void NeuralEnd::saveWeights() {
  for (int i = 0; i < (int)inputList.size(); i++) inputList[i]->saveWeights();
}

void NeuralEnd::restoreWeights() {
  for (int i = 0; i < (int)inputList.size(); i++)
    inputList[i]->restoreWeights();
}
}  // namespace neural
}  // namespace nalso
