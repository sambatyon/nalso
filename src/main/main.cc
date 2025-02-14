/*
 * main.cpp
 *
 *  Created on: Sep 30, 2009
 *      Author: Alexander Rojas <alexander.rojas@gmail.com>
 */

#include <iostream>

// #include "logicstructs/program.hh"

#ifdef DEBUG
#include <cppunit/ui/text/TestRunner.hh>

#include "test/testlogicstruct.hh"
#include "test/testnetworks.hh"
#include "test/testparser.hh"
#else
#endif

int main(int args, char** argv) {
  std::cout << "Hello world" << std::endl;
#ifdef DEBUG
  CppUnit::TextUi::TestRunner runner;
  runner.addTest(test::TestLogicStruct::suite());
  runner.addTest(test::TestParser::suite());
  runner.addTest(test::TestNeuralNetworks::suite());
  runner.run();
#else
#endif
  return 0;
}
