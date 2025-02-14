/*
 * @file nnabductionalgo.h
 *
 * @date Oct 23, 2009
 * @author Alexander Rojas <alexander.rojas@gmail.com>
 */

#ifndef NETWORKBUILDER_H_
#define NETWORKBUILDER_H_

#include <set>

#include "logicstructs/program.hh"
#include "networks/neuralnetwork.hh"

namespace nalso {

namespace algorithms {
using namespace neural;
using namespace logicStructs;
using namespace std;

/**
 * An interface that every algorithm that generates a network from a logic
 * program should implement. It provides two methods, the first builds a network
 * from a logic program while the second builds a network from a set of clauses.
 *
 * @author Alexander Rojas <alexander.rojas@gmail.com>
 */
class NNBuilderAlgo {
 public:
  virtual NeuralNetworkPtr buildNetwork(ProgramPtr pr) = 0;
  virtual NeuralNetworkPtr buildNetwork(set<ClausePtr> pr) = 0;
};

}  // namespace algorithms

}  // namespace nalso

#endif /* NETWORKBUILDER_H_ */
