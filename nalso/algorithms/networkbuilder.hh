#pragma once
/*
 * @file nnabductionalgo.h
 *
 * @date Oct 23, 2009
 * @author Alexander Rojas <alexander.rojas@gmail.com>
 */

#include "nalso/logicstructs/program.hh"
#include "nalso/networks/neuralnetwork.hh"

#include <set>

namespace nalso {
namespace algorithms {
/**
 * An interface that every algorithm that generates a network from a logic
 * program should implement. It provides two methods, the first builds a network
 * from a logic program while the second builds a network from a set of clauses.
 *
 * @author Alexander Rojas <alexander.rojas@gmail.com>
 */
class NNBuilderAlgo {
 public:
  virtual neural::NeuralNetworkPtr buildNetwork(logic::ProgramPtr pr) = 0;
  virtual neural::NeuralNetworkPtr buildNetwork(std::set<logic::ClausePtr> pr) = 0;
};

}  // namespace algorithms
}  // namespace nalso
