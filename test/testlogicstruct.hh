/*
 * testlogicstruct.h
 *
 *  Created on: Sep 30, 2009
 *      Author: Alexander Rojas <alexander.rojas@gmail.com>
 */

#ifndef TESTLOGICSTRUCT_H_
#define TESTLOGICSTRUCT_H_

#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>

#include "logicstructs/program.hh"

namespace test {
using namespace nalso;
using namespace logicStructs;
class TestLogicStruct : public CppUnit::TestFixture {
 public:
  TestLogicStruct();
  virtual ~TestLogicStruct();

  void testAtomConstructors();
  void testAtomConversion();
  void testAtomAssignment();

  void testLiteralConstructors();
  void testLiteralConversion();
  void testLiteralAssignment();

  static CppUnit::TestSuite* suite();
};

}  // namespace test

#endif /* TESTLOGICSTRUCT_H_ */
