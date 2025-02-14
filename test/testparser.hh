/*
 * @file testparser.h
 *
 *  @date Oct 10, 2009
 *  @author Alexander Rojas <alexander.rojas@gmail.com>
 */

#ifndef TESTPARSER_H_
#define TESTPARSER_H_

#include <cppunit/TestFixture.h>
#include <cppunit/TestSuite.h>
#include <logicstructs/program.hh>
#include <parsers/alparser.hh>
#include <parsers/splparser.hh>

namespace test {
using namespace std;
using namespace nalso::parsers;

class TestParser : public CppUnit::TestFixture {
 public:
  TestParser() {};
  virtual ~TestParser() {};

  void testSmallPrologParser();

  static CppUnit::TestSuite* suite();
};

}  // namespace test

#endif /* TESTPARSER_H_ */
