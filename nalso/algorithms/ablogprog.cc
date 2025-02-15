/*
 * ablogprog.cpp
 *
 *  Created on: Oct 27, 2009
 *      Author: diego
 */

#include "ablogprog.hh"

#include <sstream>

namespace nalso {
namespace algorithms {

std::map<std::string, std::pair<logic::LiteralPtr, logic::LiteralPtr> > AbLogProg::generateAuxVars(
    logic::ProgramPtr pr) {
  std::map<std::string, std::pair<logic::LiteralPtr, logic::LiteralPtr> > res;

  std::string name = "goal";
  logic::BoolVarPtr tmp(new logic::BoolVar(name));
  res[name].first.reset(new logic::Literal(tmp));
  res[name].second.reset(new logic::Literal(tmp, true));

  name = "ic";
  tmp.reset(new logic::BoolVar(name));
  res[name].first.reset(new logic::Literal(tmp));
  res[name].second.reset(new logic::Literal(tmp, true));

  name = "nogood";
  tmp.reset(new logic::BoolVar(name));
  res[name].first.reset(new logic::Literal(tmp));
  res[name].second.reset(new logic::Literal(tmp, true));

  name = "sync";
  tmp.reset(new logic::BoolVar(name));
  res[name].first.reset(new logic::Literal(tmp));
  res[name].second.reset(new logic::Literal(tmp, true));

  name = "soln";
  tmp.reset(new logic::BoolVar(name));
  res[name].first.reset(new logic::Literal(tmp));
  res[name].second.reset(new logic::Literal(tmp, true));

  name = "next";
  tmp.reset(new logic::BoolVar(name));
  res[name].first.reset(new logic::Literal(tmp));
  res[name].second.reset(new logic::Literal(tmp, true));

  name = "hold";
  tmp.reset(new logic::BoolVar(name));
  res[name].first.reset(new logic::Literal(tmp));
  res[name].second.reset(new logic::Literal(tmp, true));

  name = "done";
  tmp.reset(new logic::BoolVar(name));
  res[name].first.reset(new logic::Literal(tmp));
  res[name].second.reset(new logic::Literal(tmp, true));

  for (unsigned int abdcont = 1; abdcont <= pr->getAbducts().size(); abdcont++) {
    std::stringstream ap, am1p, bp, bm1p, cp, dp;
    ap << "a_" << abdcont;
    bp << "b_" << abdcont;
    cp << "c_" << abdcont;
    dp << "d_" << abdcont;

    name = ap.str();
    tmp.reset(new logic::BoolVar(name));
    res[name].first.reset(new logic::Literal(tmp));
    res[name].second.reset(new logic::Literal(tmp, true));

    name = bp.str();
    tmp.reset(new logic::BoolVar(name));
    res[name].first.reset(new logic::Literal(tmp));
    res[name].second.reset(new logic::Literal(tmp, true));

    name = cp.str();
    tmp.reset(new logic::BoolVar(name));
    res[name].first.reset(new logic::Literal(tmp));
    res[name].second.reset(new logic::Literal(tmp, true));

    name = dp.str();
    tmp.reset(new logic::BoolVar(name));
    res[name].first.reset(new logic::Literal(tmp));
    res[name].second.reset(new logic::Literal(tmp, true));
  }

  name = "a_0";
  tmp.reset(new logic::BoolVar(name));
  res[name].first.reset(new logic::Literal(tmp));
  res[name].second.reset(new logic::Literal(tmp, true));

  name = "b_0";
  tmp.reset(new logic::BoolVar(name));
  res[name].first.reset(new logic::Literal(tmp));
  res[name].second.reset(new logic::Literal(tmp, true));

  clockSize = clockSize == NAN ? (3 * pr->getAbducts().size() + 1) : clockSize;

  for (unsigned int i = 0; i <= clockSize; i++) {
    std::stringstream ss;

    ss << "k_" << i;

    name = ss.str();
    tmp.reset(new logic::BoolVar(name));
    res[name].first.reset(new logic::Literal(tmp));
    res[name].second.reset(new logic::Literal(tmp, true));
  }

  return res;
}

neural::NeuralNetworkPtr AbLogProg::buildNetwork(logic::ProgramPtr pr) {
  // Create a copy of the program since this algorithm will modify the program
  // adding clauses
  logic::ProgramPtr copy(new logic::Program);
  std::set<logic::ConstraintPtr> cpConst = pr->getConstraints();
  copy->setConstraints(cpConst);
  std::set<logic::ClausePtr> cpCls = pr->getClauses();
  copy->setClauses(cpCls);
  std::set<logic::BoolVarPtr> cpAbds = pr->getAbducts();
  copy->setAbducst(cpAbds);
  std::set<logic::BoolVarPtr> cpObs = pr->getObsers();
  copy->setObsers(cpObs);

  // generate variables in order to save memory
  std::map<std::string, std::pair<logic::LiteralPtr, logic::LiteralPtr> > auxVars = generateAuxVars(copy);

  // create the goal clause
  logic::ClausePtr tmp(new logic::Clause);
  tmp->setHead(auxVars["goal"].first->getVar());
  for (auto it = copy->getObsers().begin(); it != copy->getObsers().end(); it++) {
    tmp->getBody().insert(logic::LiteralPtr(new logic::Literal(*it)));
  }
  copy->getClauses().insert(tmp);

  // create the integrity constraints clauses
  for (auto it = copy->getConstraints().begin(); it != copy->getConstraints().end(); it++) {
    tmp.reset(new logic::Clause);
    tmp->setHead(auxVars["ic"].first->getVar());

    for (auto cit = (**it).getBody().begin(); cit != (**it).getBody().end(); cit++)
      tmp->getBody().insert(logic::LiteralPtr(new logic::Literal(*cit)));

    copy->getClauses().insert(tmp);
  }

  // creates the "logic engine"
  // ic -> nogood
  tmp.reset(new logic::Clause);
  tmp->setHead(auxVars["nogood"].first->getVar());
  tmp->getBody().insert(auxVars["ic"].first);
  copy->getClauses().insert(tmp);

  // ¬goal -> nogood
  tmp.reset(new logic::Clause);
  tmp->setHead(auxVars["nogood"].first->getVar());
  tmp->getBody().insert(auxVars["goal"].second);
  copy->getClauses().insert(tmp);

  // sync,¬nogood -> soln
  tmp.reset(new logic::Clause);
  tmp->setHead(auxVars["soln"].first->getVar());
  tmp->getBody().insert(auxVars["nogood"].second);
  tmp->getBody().insert(auxVars["sync"].first);
  copy->getClauses().insert(tmp);

  // soln,¬nogood -> soln
  tmp.reset(new logic::Clause);
  tmp->setHead(auxVars["soln"].first->getVar());
  tmp->getBody().insert(auxVars["nogood"].second);
  tmp->getBody().insert(auxVars["soln"].first);
  copy->getClauses().insert(tmp);

  // soln -> hold
  tmp.reset(new logic::Clause);
  tmp->setHead(auxVars["hold"].first->getVar());
  tmp->getBody().insert(auxVars["soln"].first);
  copy->getClauses().insert(tmp);

  // done -> hold
  tmp.reset(new logic::Clause);
  tmp->setHead(auxVars["hold"].first->getVar());
  tmp->getBody().insert(auxVars["done"].first);
  copy->getClauses().insert(tmp);

  // sync,nogood -> next
  tmp.reset(new logic::Clause);
  tmp->setHead(auxVars["next"].first->getVar());
  tmp->getBody().insert(auxVars["nogood"].first);
  tmp->getBody().insert(auxVars["sync"].first);
  copy->getClauses().insert(tmp);

  // now we add the counter
  std::string a = "a_";
  std::string b = "b_";
  std::string c = "c_";
  std::string d = "d_";
  std::stringstream si, si_1;
  for (unsigned int i = 1; i <= copy->getAbducts().size(); i++) {
    si << i;
    si_1 << (i - 1);

    // a_i, ¬c_i -> a_i
    tmp.reset(new logic::Clause);
    tmp->setHead(auxVars[a + si.str()].first->getVar());
    tmp->getBody().insert(auxVars[a + si.str()].first);
    tmp->getBody().insert(auxVars[c + si.str()].second);
    copy->getClauses().insert(tmp);

    // d_i -> a_i
    tmp.reset(new logic::Clause);
    tmp->setHead(auxVars[a + si.str()].first->getVar());
    tmp->getBody().insert(auxVars[d + si.str()].first);
    copy->getClauses().insert(tmp);

    // a_i -> b_i
    tmp.reset(new logic::Clause);
    tmp->setHead(auxVars[b + si.str()].first->getVar());
    tmp->getBody().insert(auxVars[a + si.str()].first);
    copy->getClauses().insert(tmp);

    // b_{i-1}, ¬a_{i-1}, a_i -> c_i
    tmp.reset(new logic::Clause);
    tmp->setHead(auxVars[c + si.str()].first->getVar());
    tmp->getBody().insert(auxVars[b + si_1.str()].first);
    tmp->getBody().insert(auxVars[a + si_1.str()].second);
    tmp->getBody().insert(auxVars[a + si.str()].first);
    copy->getClauses().insert(tmp);

    // b_{i-1}, ¬a_{i-1}, ¬a_i -> d_i
    tmp.reset(new logic::Clause);
    tmp->setHead(auxVars[d + si.str()].first->getVar());
    tmp->getBody().insert(auxVars[b + si_1.str()].first);
    tmp->getBody().insert(auxVars[a + si_1.str()].second);
    tmp->getBody().insert(auxVars[a + si.str()].second);
    copy->getClauses().insert(tmp);
  }

  // next -> b_0
  tmp.reset(new logic::Clause);
  tmp->setHead(auxVars["b_0"].first->getVar());
  tmp->getBody().insert(auxVars["next"].first);
  copy->getClauses().insert(tmp);

  // b_N, ¬a_N -> done
  tmp.reset(new logic::Clause);
  tmp->setHead(auxVars["done"].first->getVar());
  tmp->getBody().insert(auxVars[b + si.str()].first);
  tmp->getBody().insert(auxVars[a + si.str()].second);
  copy->getClauses().insert(tmp);

  // done -> done
  tmp.reset(new logic::Clause);
  tmp->setHead(auxVars["done"].first->getVar());
  tmp->getBody().insert(auxVars["done"].first);
  copy->getClauses().insert(tmp);

  // now the clock is added
  std::string k = "k_";
  for (unsigned int i = 1; i <= clockSize; i++) {
    si << i;
    si_1 << i - 1;

    // k_{i-1} -> k_i
    tmp.reset(new logic::Clause);
    tmp->setHead(auxVars[k + si.str()].first->getVar());
    tmp->getBody().insert(auxVars[k + si_1.str()].first);
    copy->getClauses().insert(tmp);
  }

  // ¬hold,¬k_M -> k_0
  tmp.reset(new logic::Clause);
  tmp->setHead(auxVars["k_0"].first->getVar());
  tmp->getBody().insert(auxVars["hold"].second);
  tmp->getBody().insert(auxVars[k + si.str()].second);
  copy->getClauses().insert(tmp);

  // k_0,¬k_1 -> sync
  tmp.reset(new logic::Clause);
  tmp->setHead(auxVars["sync"].first->getVar());
  tmp->getBody().insert(auxVars["k_0"].first);
  tmp->getBody().insert(auxVars["k_1"].second);
  copy->getClauses().insert(tmp);

  // now we create a_i -> abduct_i
  int counter = 1;
  for (auto it = copy->getAbducts().begin(); it != copy->getAbducts().end(); it++) {
    si << counter;

    tmp.reset(new logic::Clause);
    tmp->setHead(*it);
    tmp->getBody().insert(auxVars[a + si.str()].first);
    copy->getClauses().insert(tmp);

    counter++;
  }

  return Cilp::buildNetwork(copy);
}

neural::NeuralNetworkPtr AbLogProg::buildNetwork(std::set<logic::ClausePtr> pr) {
  logic::ProgramPtr prg(new logic::Program);

  (*prg).setClauses(pr);
  prg->fillAbducts();
  prg->fillObsers();

  return buildNetwork(prg);
}

}  // namespace algorithms
}  // namespace nalso
