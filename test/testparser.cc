/*
 * @file testparser.cpp
 *
 *  @date Oct 10, 2009
 *  @author Alexander Rojas <alexander.rojas@gmail.com>
 */

#include "testparser.hh"

#include <cppunit/TestCaller.h>

#include <fstream>
#include <string>

namespace test {

void TestParser::testSmallPrologParser() {
  ifstream source("test.pl");
  SmallPrologParser parser(source);

  // cout << (string)(*parser.parseProgram());
}

CppUnit::TestSuite* TestParser::suite() {
  // using namespace CppUnit;
  CppUnit::TestSuite* suiteOfTests = new CppUnit::TestSuite("TestParser");
  suiteOfTests->addTest(new CppUnit::TestCaller<TestParser>(
      "testSmallPrologParser", &TestParser::testSmallPrologParser));
  return suiteOfTests;
}
}  // namespace test
