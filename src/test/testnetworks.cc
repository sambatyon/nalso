/*
 * TestNeuralNetworks.cpp
 *
 *  Created on: Oct 13, 2009
 *      Author: diego
 */

#include "testnetworks.h"

#include <cppunit/TestCaller.h>

namespace test {

TestNeuralNetworks::TestNeuralNetworks() {}

TestNeuralNetworks::~TestNeuralNetworks() {}

void TestNeuralNetworks::testFeedForwardNeuralNetwork() {
  NeuralMethodPtr lin(new LinearMethod);
  NeuralMethodPtr sig(new SigmoidMethod);

  /*NeuralEndPtr nep(new NeuralEnd("p", true));
  NeuralEndPtr neq(new NeuralEnd("q", true));
  NeuralEndPtr nepxq(new NeuralEnd("pxq", true));*/

  NeuralNodePtr nnp(new NeuralNode("p", lin));
  (*nnp).setBias(0);
  (*nnp).setType(INPUT);
  NeuralNodePtr nnq(new NeuralNode("q", lin));
  (*nnq).setBias(0);
  (*nnq).setType(INPUT);
  NeuralNodePtr nnpxq(new NeuralNode("pxq", sig));
  (*nnpxq).setBias(0);
  (*nnpxq).setType(OUTPUT);

  NeuralNodePtr h0(new NeuralNode("h0", sig));
  (*h0).setBias(0);
  NeuralNodePtr h1(new NeuralNode("h1", sig));
  (*h1).setBias(0);

  FeedForwardNeuralNetwork network;
  /*network.addEndNode(nep);
  network.addEndNode(neq);
  network.addEndNode(nepxq, false);*/

  network.addNode(nnp);
  network.addNode(nnq);
  network.addNode(h0);
  network.addNode(h1);
  network.addNode(nnpxq);

  /*network.connectNodes(nep, nnp);
  network.connectNodes(neq, nnq);
  network.connectNodes(nnpxq, nepxq);*/

  network.connectNodes("p", "h0", 3);
  network.connectNodes("p", "h1", 2);
  network.connectNodes("q", "h0", -3);
  network.connectNodes("q", "h1", -2);
  network.connectNodes("h0", "pxq", 3);
  network.connectNodes("h1", "pxq", -3);

  ofstream out("feed.dot");
  out << network.debugString();
  out.flush();
  out.close();

  ParamsMap par1;
  par1["w0-p"] = 0;
  par1["w0-q"] = 0;

  par1 = network.evaluate(par1);
  CPPUNIT_ASSERT(par1["w0-pxq"] <= 0.5);

  ParamsMap par2;
  par2["w0-p"] = 1;
  par2["w0-q"] = 0;

  par2 = network.evaluate(par2);
  CPPUNIT_ASSERT(par2["w0-pxq"] > 0.5);
}

void TestNeuralNetworks::testFixPointOperatorFeedForward() {}

void TestNeuralNetworks::testHopfield() {
  HopfieldNeuralNetwork network;

  map<string, HopfieldNodePtr> nodeset;
  nodeset["p"].reset(new HopfieldNode(string("p")));
  nodeset["q"].reset(new HopfieldNode(string("q")));
  nodeset["r"].reset(new HopfieldNode(string("r")));
  nodeset["s"].reset(new HopfieldNode(string("s")));
  nodeset["t"].reset(new HopfieldNode(string("t")));
  nodeset["u"].reset(new HopfieldNode(string("u")));
  nodeset["G"].reset(new HopfieldNode(string("G")));

  for (map<string, HopfieldNodePtr>::iterator it = nodeset.begin();
       it != nodeset.end(); it++)
    network.addNode((*it).second);

  vector<string> link;
  link.push_back("t");
  network.connectNodes(link, -1000);

  link.clear();
  link.push_back("t");
  link.push_back("p");
  link.push_back("q");
  link.push_back("s");
  network.connectNodes(link, 1000);

  link.clear();
  link.push_back("s");
  network.connectNodes(link, -1000);

  link.clear();
  link.push_back("r");
  link.push_back("p");
  link.push_back("s");
  network.connectNodes(link, 1000);

  link.clear();
  link.push_back("q");
  link.push_back("u");
  link.push_back("s");
  network.connectNodes(link, 1000);

  link.clear();
  link.push_back("r");
  link.push_back("p");
  link.push_back("q");
  link.push_back("s");
  link.push_back("u");
  network.connectNodes(link, -1000);

  link.clear();
  link.push_back("G");
  link.push_back("u");
  link.push_back("t");
  network.connectNodes(link, 1000);

  link.clear();
  link.push_back("p");
  network.connectNodes(link, -100);

  link.clear();
  link.push_back("q");
  network.connectNodes(link, -150);

  link.clear();
  link.push_back("u");
  network.connectNodes(link, -75);

  link.clear();
  link.push_back("G");
  network.connectNodes(link, -2000);

  ParamsMap question;
  question["p"] = 0;
  question["q"] = 0;
  question["r"] = 0;
  question["s"] = 0;
  question["t"] = 0;
  question["u"] = 0;
  question["G"] = 0;

  question = network.evaluate(question);

  for (ParamsMap::iterator it = question.begin(); it != question.end(); it++)
    cout << (*it).first << ": " << (*it).second << endl;
}

void TestNeuralNetworks::testFixPointHopfield() {
  HopfieldNeuralNetwork network;

  map<string, HopfieldNodePtr> nodeset;
  nodeset["p"].reset(new HopfieldNode(string("p")));
  nodeset["q"].reset(new HopfieldNode(string("q")));
  nodeset["r"].reset(new HopfieldNode(string("r")));
  nodeset["s"].reset(new HopfieldNode(string("s")));
  nodeset["t"].reset(new HopfieldNode(string("t")));
  nodeset["u"].reset(new HopfieldNode(string("u")));
  nodeset["G"].reset(new HopfieldNode(string("G")));

  for (map<string, HopfieldNodePtr>::iterator it = nodeset.begin();
       it != nodeset.end(); it++)
    network.addNode((*it).second);

  vector<string> link;
  link.push_back("t");
  network.connectNodes(link, -1000);

  link.clear();
  link.push_back("t");
  link.push_back("p");
  link.push_back("q");
  link.push_back("s");
  network.connectNodes(link, 1000);

  link.clear();
  link.push_back("s");
  network.connectNodes(link, -1000);

  link.clear();
  link.push_back("s");
  link.push_back("p");
  link.push_back("r");
  network.connectNodes(link, 1000);

  link.clear();
  link.push_back("s");
  link.push_back("q");
  link.push_back("u");
  network.connectNodes(link, 1000);

  link.clear();
  link.push_back("s");
  link.push_back("p");
  link.push_back("r");
  link.push_back("q");
  link.push_back("u");
  network.connectNodes(link, -1000);

  link.clear();
  link.push_back("G");
  link.push_back("t");
  link.push_back("u");
  network.connectNodes(link, 1000);

  link.clear();
  link.push_back("p");
  network.connectNodes(link, -100);

  link.clear();
  link.push_back("q");
  network.connectNodes(link, -150);

  link.clear();
  link.push_back("u");
  network.connectNodes(link, -75);

  link.clear();
  link.push_back("G");
  network.connectNodes(link, 2000);

  ParamsMap question;
  question["p"] = 0;
  question["q"] = 0;
  question["r"] = 0;
  question["s"] = 0;
  question["t"] = 0;
  question["u"] = 0;
  question["G"] = 0;

  question = network.findFixPoint(question);

  for (ParamsMap::iterator it = question.begin(); it != question.end(); it++)
    cout << (*it).first << ": " << (*it).second << endl;
}

void TestNeuralNetworks::testFixPointHopfield2() {
  HopfieldNeuralNetwork network;

  map<string, HopfieldNodePtr> nodeset;
  nodeset["p"].reset(new HopfieldNode(string("p")));
  nodeset["q"].reset(new HopfieldNode(string("q")));
  nodeset["s"].reset(new HopfieldNode(string("s")));
  nodeset["t"].reset(new HopfieldNode(string("t")));
  nodeset["G"].reset(new HopfieldNode(string("G")));

  for (map<string, HopfieldNodePtr>::iterator it = nodeset.begin();
       it != nodeset.end(); it++)
    network.addNode((*it).second);

  vector<string> link;
  link.push_back("s");
  network.connectNodes(link, -1000);

  link.clear();
  link.push_back("p");
  link.push_back("q");
  link.push_back("s");
  network.connectNodes(link, 1000);

  link.clear();
  link.push_back("G");
  link.push_back("s");
  network.connectNodes(link, 1000);

  link.clear();
  link.push_back("p");
  network.connectNodes(link, -100);

  link.clear();
  link.push_back("q");
  network.connectNodes(link, -100);

  link.clear();
  link.push_back("G");
  network.connectNodes(link, 3000);

  link.clear();
  link.push_back("t");
  link.push_back("q");
  network.connectNodes(link, -1000);

  link.clear();
  link.push_back("t");
  network.connectNodes(link, -100);

  ParamsMap question;
  question["p"] = 0;
  question["q"] = 0;
  question["s"] = 0;
  question["t"] = 0;
  question["G"] = 0;

  question = network.findFixPoint(question);

  for (ParamsMap::iterator it = question.begin(); it != question.end(); it++)
    cout << (*it).first << ": " << (*it).second << endl;
}

void TestNeuralNetworks::testFixPointHopfield3() {
  HopfieldNeuralNetwork network;

  map<string, HopfieldNodePtr> nodeset;
  nodeset["p"].reset(new HopfieldNode(string("p")));
  nodeset["q"].reset(new HopfieldNode(string("q")));
  nodeset["qp"].reset(new HopfieldNode(string("qp")));
  nodeset["r"].reset(new HopfieldNode(string("r")));
  nodeset["rp"].reset(new HopfieldNode(string("rp")));
  nodeset["s"].reset(new HopfieldNode(string("s")));

  for (map<string, HopfieldNodePtr>::iterator it = nodeset.begin();
       it != nodeset.end(); it++)
    network.addNode((*it).second);

  vector<string> link;
  link.push_back("p");
  network.connectNodes(link, -100);

  link.clear();
  link.push_back("p");
  link.push_back("r");
  network.connectNodes(link, 50);

  link.clear();
  link.push_back("p");
  link.push_back("s");
  network.connectNodes(link, 100);

  link.clear();
  link.push_back("p");
  link.push_back("s");
  link.push_back("r");
  network.connectNodes(link, -100);

  link.clear();
  link.push_back("q");
  network.connectNodes(link, -725);

  link.clear();
  link.push_back("q");
  link.push_back("r");
  network.connectNodes(link, -75);

  link.clear();
  link.push_back("rp");
  network.connectNodes(link, 75);

  link.clear();
  link.push_back("q");
  link.push_back("rp");
  network.connectNodes(link, -75);

  link.clear();
  link.push_back("qp");
  link.push_back("rp");
  network.connectNodes(link, -75);

  link.clear();
  link.push_back("q");
  link.push_back("r");
  link.push_back("rp");
  network.connectNodes(link, 75);

  link.clear();
  link.push_back("q");
  link.push_back("r");
  link.push_back("qp");
  network.connectNodes(link, 75);

  link.clear();
  link.push_back("q");
  link.push_back("r");
  link.push_back("qp");
  link.push_back("rp");
  network.connectNodes(link, -75);

  link.clear();
  link.push_back("r");
  network.connectNodes(link, -1000);

  link.clear();
  link.push_back("r");
  link.push_back("rp");
  network.connectNodes(link, 1000);

  link.clear();
  link.push_back("q");
  link.push_back("qp");
  network.connectNodes(link, 1000);

  link.clear();
  link.push_back("s");
  link.push_back("p");
  network.connectNodes(link, 400);

  link.clear();
  link.push_back("s");
  network.connectNodes(link, 200);

  ParamsMap question;
  question["p"] = 0;
  question["q"] = 0;
  question["qp"] = 0;
  question["r"] = 0;
  question["rp"] = 0;
  question["s"] = 0;

  question = network.findFixPoint(question);

  for (ParamsMap::iterator it = question.begin(); it != question.end(); it++)
    cout << (*it).first << ": " << (*it).second << endl;
}

void TestNeuralNetworks::testFixPointHopfield4() {
  HopfieldNeuralNetwork network;

  map<string, HopfieldNodePtr> nodeset;
  nodeset["p"].reset(new HopfieldNode(string("p")));
  nodeset["q"].reset(new HopfieldNode(string("q")));
  nodeset["r"].reset(new HopfieldNode(string("r")));
  nodeset["a"].reset(new HopfieldNode(string("a")));
  nodeset["ap"].reset(new HopfieldNode(string("ap")));
  nodeset["G"].reset(new HopfieldNode(string("G")));

  for (map<string, HopfieldNodePtr>::iterator it = nodeset.begin();
       it != nodeset.end(); it++)
    network.addNode((*it).second);

  vector<string> link;
  link.push_back("q");
  network.connectNodes(link, -1000);

  link.clear();
  link.push_back("p");
  link.push_back("q");
  network.connectNodes(link, 1000);

  link.clear();
  link.push_back("r");
  link.push_back("q");
  network.connectNodes(link, 1000);

  link.clear();
  link.push_back("r");
  link.push_back("ap");
  link.push_back("q");
  network.connectNodes(link, -1000);

  link.clear();
  link.push_back("p");
  link.push_back("a");
  link.push_back("q");
  network.connectNodes(link, -1000);

  link.clear();
  link.push_back("r");
  link.push_back("p");
  link.push_back("q");
  network.connectNodes(link, -1000);

  link.clear();
  link.push_back("r");
  link.push_back("p");
  link.push_back("a");
  link.push_back("q");
  network.connectNodes(link, 1000);

  link.clear();
  link.push_back("r");
  link.push_back("p");
  link.push_back("ap");
  link.push_back("q");
  network.connectNodes(link, 1000);

  link.clear();
  link.push_back("p");
  link.push_back("a");
  link.push_back("r");
  link.push_back("ap");
  link.push_back("q");
  network.connectNodes(link, -1000);

  link.clear();
  link.push_back("p");
  link.push_back("ap");
  network.connectNodes(link, -1000);

  link.clear();
  link.push_back("r");
  link.push_back("a");
  network.connectNodes(link, -1000);

  link.clear();
  link.push_back("G");
  link.push_back("q");
  network.connectNodes(link, 1000);

  link.clear();
  link.push_back("G");
  network.connectNodes(link, 3000);

  link.clear();
  link.push_back("p");
  network.connectNodes(link, -10);

  link.clear();
  link.push_back("r");
  network.connectNodes(link, -100);

  ParamsMap question;
  question["p"] = 0;
  question["q"] = 0;
  question["r"] = 0;
  question["a"] = 0;
  question["ap"] = 0;
  question["G"] = 0;

  question = network.findFixPoint(question);

  for (ParamsMap::iterator it = question.begin(); it != question.end(); it++)
    cout << (*it).first << ": " << (*it).second << endl;

  ofstream out("hop.dot");
  out << network.debugString();
  out.flush();
  out.close();
}

void TestNeuralNetworks::testFixPointHopfield5() {
  HopfieldNeuralNetwork network;

  map<string, HopfieldNodePtr> nodeset;
  nodeset["p"].reset(new HopfieldNode(string("p")));
  nodeset["q"].reset(new HopfieldNode(string("q")));
  nodeset["a"].reset(new HopfieldNode(string("a")));
  nodeset["G"].reset(new HopfieldNode(string("G")));

  for (map<string, HopfieldNodePtr>::iterator it = nodeset.begin();
       it != nodeset.end(); it++)
    network.addNode((*it).second);

  vector<string> link;
  link.push_back("q");
  network.connectNodes(link, -1000);

  link.clear();
  link.push_back("p");
  link.push_back("q");
  network.connectNodes(link, 1000);

  link.clear();
  link.push_back("p");
  link.push_back("q");
  link.push_back("a");
  network.connectNodes(link, -1000);

  link.clear();
  link.push_back("G");
  link.push_back("q");
  network.connectNodes(link, 1000);

  link.clear();
  link.push_back("G");
  network.connectNodes(link, 3000);

  link.clear();
  link.push_back("q");
  network.connectNodes(link, -100);

  link.clear();
  link.push_back("p");
  network.connectNodes(link, -100);

  ParamsMap question;
  question["p"] = 0;
  question["q"] = 0;
  question["a"] = 0;
  question["G"] = 0;

  question = network.findFixPoint(question);

  for (ParamsMap::iterator it = question.begin(); it != question.end(); it++)
    cout << (*it).first << ": " << (*it).second << endl;
}

void TestNeuralNetworks::testFixPointHopfield6() {
  HopfieldNeuralNetwork network;

  map<string, HopfieldNodePtr> nodeset;
  nodeset["p"].reset(new HopfieldNode(string("p")));
  nodeset["q"].reset(new HopfieldNode(string("q")));
  nodeset["r"].reset(new HopfieldNode(string("r")));
  nodeset["s"].reset(new HopfieldNode(string("s")));
  nodeset["pp"].reset(new HopfieldNode(string("pp")));
  nodeset["G"].reset(new HopfieldNode(string("G")));

  for (map<string, HopfieldNodePtr>::iterator it = nodeset.begin();
       it != nodeset.end(); it++)
    network.addNode((*it).second);

  vector<string> link;
  link.push_back("r");
  network.connectNodes(link, -1000);

  link.clear();
  link.push_back("p");
  link.push_back("q");
  link.push_back("r");
  network.connectNodes(link, 1000);

  link.clear();
  link.push_back("r");
  link.push_back("p");
  link.push_back("s");
  network.connectNodes(link, 1000);

  link.clear();
  link.push_back("r");
  link.push_back("p");
  link.push_back("q");
  link.push_back("s");
  link.push_back("pp");
  network.connectNodes(link, -1000);

  link.clear();
  link.push_back("pp");
  network.connectNodes(link, -1000);

  link.clear();
  link.push_back("p");
  link.push_back("pp");
  network.connectNodes(link, 1000);

  link.clear();
  link.push_back("G");
  link.push_back("r");
  network.connectNodes(link, 1000);

  link.clear();
  link.push_back("G");
  network.connectNodes(link, 3000);

  link.clear();
  link.push_back("p");
  network.connectNodes(link, -100);

  ParamsMap question;
  question["p"] = 0;
  question["q"] = 0;
  question["r"] = 0;
  question["s"] = 0;
  question["pp"] = 0;
  question["G"] = 0;

  question = network.findFixPoint(question);

  for (ParamsMap::iterator it = question.begin(); it != question.end(); it++)
    cout << (*it).first << ": " << (*it).second << endl;
}

CppUnit::TestSuite* TestNeuralNetworks::suite() {
  CppUnit::TestSuite* suiteOfTests =
      new CppUnit::TestSuite("TestNeuralNetworks");
  suiteOfTests->addTest(new CppUnit::TestCaller<TestNeuralNetworks>(
      "testFeedForwardNeuralNetwork",
      &TestNeuralNetworks::testFeedForwardNeuralNetwork));
  suiteOfTests->addTest(new CppUnit::TestCaller<TestNeuralNetworks>(
      "testFixPointOperatorFeedForward",
      &TestNeuralNetworks::testFixPointOperatorFeedForward));
  // suiteOfTests->addTest(new
  // CppUnit::TestCaller<TestNeuralNetworks>("testHopfield",
  // &TestNeuralNetworks::testHopfield));
  suiteOfTests->addTest(new CppUnit::TestCaller<TestNeuralNetworks>(
      "testFixPointHopfield", &TestNeuralNetworks::testFixPointHopfield));
  suiteOfTests->addTest(new CppUnit::TestCaller<TestNeuralNetworks>(
      "testFixPointHopfield2", &TestNeuralNetworks::testFixPointHopfield2));
  // suiteOfTests->addTest(new
  // CppUnit::TestCaller<TestNeuralNetworks>("testFixPointHopfield3",
  // &TestNeuralNetworks::testFixPointHopfield3));
  suiteOfTests->addTest(new CppUnit::TestCaller<TestNeuralNetworks>(
      "testFixPointHopfield4", &TestNeuralNetworks::testFixPointHopfield4));
  suiteOfTests->addTest(new CppUnit::TestCaller<TestNeuralNetworks>(
      "testFixPointHopfield5", &TestNeuralNetworks::testFixPointHopfield5));
  suiteOfTests->addTest(new CppUnit::TestCaller<TestNeuralNetworks>(
      "testFixPointHopfield6", &TestNeuralNetworks::testFixPointHopfield6));
  return suiteOfTests;
}

}  // namespace test
