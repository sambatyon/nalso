/*
 * TestNeuralNetworks.h
 *
 *  Created on: Oct 13, 2009
 *      Author: diego
 */

#ifndef TESTNETWORKS_H_
#define TESTNETWORKS_H_

#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>

#include <fstream>
#include <map>
#include <vector>

#include "networks/feedforward.hh"
#include "networks/hopfield.hh"
#include "networks/method.hh"
#include "networks/node.hh"

namespace test {
using namespace std;
using namespace nalso::neural;

class TestNeuralNetworks : public CppUnit::TestFixture {
 public:
  TestNeuralNetworks();
  virtual ~TestNeuralNetworks();

  void testFeedForwardNeuralNetwork();
  void testFixPointOperatorFeedForward();

  void testHopfield();
  void testFixPointHopfield();
  void testFixPointHopfield2();
  void testFixPointHopfield3();
  void testFixPointHopfield4();
  void testFixPointHopfield5();
  void testFixPointHopfield6();

  static CppUnit::TestSuite* suite();
};

}  // namespace test

#endif /* TESTNETWORKS_H_ */
