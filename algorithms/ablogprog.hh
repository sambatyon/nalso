/**
 * @file ablogprog.h
 *
 * @date Oct 27, 2009
 * @author Alexander Rojas <alexander.rojas@gmail.com>
 */

#ifndef ABLOGPROG_H_
#define ABLOGPROG_H_

#include <cmath>
#include <map>
#include <sstream>

#include "cilp.hh"

namespace nalso {

namespace algorithms {
using namespace std;

/**
 * @brief Implementation of the Abductive Logic Programming algorithm
 *
 * Implements the Abductive Logic Programming algorithm to generate a neural
 * network that is able to solve abduction problems. The implementation is based
 * on the description by Garzes et. al.
 *
 * It extends Cilp because this is only an specialization of that algorithm.
 *
 * @author Alexander Rojas <alexander.rojas@gmail.com>
 */
class AbLogProg : public Cilp {
 private:
  map<string, pair<LiteralPtr, LiteralPtr> > generateAuxVars(ProgramPtr pr);

 protected:
  unsigned int clockSize;

 public:
  /**
   * Creates an instance of an AbLogPog object which uses the given beta for the
   * activation functions and the clockSize given.
   *
   * @param _beta The "sharpness" of the activation function.
   *
   * @param _clockSize The number of clauses that will be used to implement the
   * clock. If no value is given it will use three times the number of clauses
   * in the program.
   */
  AbLogProg(double _beta, unsigned int _clockSize = NAN)
      : Cilp(_beta), clockSize(_clockSize) {};
  virtual ~AbLogProg() {};

  virtual NeuralNetworkPtr buildNetwork(ProgramPtr pr);
  virtual NeuralNetworkPtr buildNetwork(set<ClausePtr> pr);
};

}  // namespace algorithms

}  // namespace nalso

#endif /* ABLOGPROG_H_ */
