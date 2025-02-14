/*
 * testlogicstruct.cpp
 *
 *  Created on: Sep 30, 2009
 *      Author: Alexander Rojas <alexander.rojas@gmail.com>
 */

#include "testlogicstruct.h"

#include <cppunit/TestCaller.h>
#include <string.h>

namespace test {

TestLogicStruct::TestLogicStruct() {
  // TODO Auto-generated constructor stub
}

TestLogicStruct::~TestLogicStruct() {
  // TODO Auto-generated destructor stub
}

void TestLogicStruct::testAtomConstructors() {
  BoolVar p((char*)"p");
  string sq = "q";
  BoolVar q(sq);
  BoolVar pp(p);
  BoolVar qq((char*)"q");

  CPPUNIT_ASSERT(p == pp);
  CPPUNIT_ASSERT(p != q);
  CPPUNIT_ASSERT(q.getName() == qq.getName());
}

void TestLogicStruct::testAtomConversion() {
  BoolVar p((char*)"p");
  string sp = p;  // explicit castin cannot be used with string
  char* cp = (char*)((string)p).c_str();
  CPPUNIT_ASSERT(sp == "p");
  CPPUNIT_ASSERT(0 == strcmp(cp, "p"));
}

void TestLogicStruct::testAtomAssignment() {
  BoolVar p((char*)"p"), q((char*)"q"), pp((char*)"p");
  CPPUNIT_ASSERT(p != q);
  p = q;
  CPPUNIT_ASSERT(p == q);
  p = "p";
  CPPUNIT_ASSERT(p == pp);
  string tmp = "q";
  p = tmp;
  CPPUNIT_ASSERT(p == q);
}

void TestLogicStruct::testLiteralConstructors() {
  BoolVar p((char*)"p");
  Literal lp(p);
  string slp = lp;
  CPPUNIT_ASSERT(slp == "p");
  CPPUNIT_ASSERT(&p != lp.getVar().get());

  BoolVarPtr pptr(new BoolVar(p));
  Literal lpp(pptr);
  CPPUNIT_ASSERT(pptr.get() == lpp.getVar().get());
}
void TestLogicStruct::testLiteralConversion() {}
void TestLogicStruct::testLiteralAssignment() {}

CppUnit::TestSuite* TestLogicStruct::suite() {
  CppUnit::TestSuite* suiteOfTests = new CppUnit::TestSuite("LogicStructs");
  suiteOfTests->addTest(new CppUnit::TestCaller<TestLogicStruct>(
      "testAtomConstructors", &TestLogicStruct::testAtomConstructors));
  suiteOfTests->addTest(new CppUnit::TestCaller<TestLogicStruct>(
      "testAtomConversion", &TestLogicStruct::testAtomConversion));
  suiteOfTests->addTest(new CppUnit::TestCaller<TestLogicStruct>(
      "testAtomAssignment", &TestLogicStruct::testAtomAssignment));

  suiteOfTests->addTest(new CppUnit::TestCaller<TestLogicStruct>(
      "testLiteralConstructors", &TestLogicStruct::testLiteralConstructors));
  suiteOfTests->addTest(new CppUnit::TestCaller<TestLogicStruct>(
      "testLiteralConversion", &TestLogicStruct::testLiteralConversion));
  suiteOfTests->addTest(new CppUnit::TestCaller<TestLogicStruct>(
      "testLiteralAssignment", &TestLogicStruct::testLiteralAssignment));
  return suiteOfTests;
}

}  // namespace test
