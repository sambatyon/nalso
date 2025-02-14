/**
 * @file cilp.h
 *
 * Definition of the CILP algorithm for transforming logic programs into neural
 * networks
 *
 * @date Oct 23, 2009
 * @author Alexander Rojas <alexander.rojas@gmail.com>
 */

#ifndef CILP_H_
#define CILP_H_

#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "networkbuilder.hh"
#include "networks/feedforward.hh"
#include "utils/utils.hh"

namespace nalso {

namespace algorithms {
using namespace std;
using namespace neural;

/**
 * Implements the CILP algorithm as proposed by Artur S. d'Avila Garcez.
 *
 * Implements the two main methods of the NNBuilder algorithm.
 */
class Cilp : public NNBuilderAlgo {
 protected:
  double beta;
  double amin, w;

  /**
   * Compute the parameters needed by the CiLP algorithm.
   *
   * @param[in] clauses The program to be transformed into a neural network
   *
   * @param[out] amin The minimum value in which a neuron is said to evaluate to
   * true.
   *
   * @param[out] w The standard weight value between two nodes.
   *
   * @param[in] beta The beta value of the activation function of the nodes.
   *
   * @param[out] mus an array containing the number of clauses for each head in
   * the program. only contains values greather than zero
   */
  static void computeParams(set<ClausePtr>& clauses, double& amin, double& w,
                            double beta, map<string, int>& mus);
  /**
   * Compute the parameters needed by the CiLP algorithm.
   *
   * @param[in] clauses The program to be transformed into a neural network
   *
   * @param[out] amin The minimum value in which a neuron is said to evaluate to
   * true.
   *
   * @param[out] w The standard weight value between two nodes.
   *
   * @param[in] beta The beta value of the activation function of the nodes.
   *
   */
  static void computeParams(set<ClausePtr>& clauses, double& amin, double& w,
                            double beta);

  static set<BoolVarPtr> getAtoms(set<ClausePtr>& cls);

 public:
  Cilp(double _beta = 1, double _amin = NAN)
      : beta(_beta), amin(_amin), w(NAN) {};
  virtual ~Cilp();

  virtual NeuralNetworkPtr buildNetwork(ProgramPtr pr);
  virtual NeuralNetworkPtr buildNetwork(set<ClausePtr> pr);
};

}  // namespace algorithms

}  // namespace nalso

#endif /* CILP_H_ */
