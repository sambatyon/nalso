/*
 * alparser.cpp
 *
 *  Created on: Oct 9, 2009
 *      Author: Alexander Rojas <alexander.rojas@gmail.com>
 */

#include "alparser.hh"

#include <set>

#include "nalso/logic/logic.hh"

namespace nalso {

namespace parsers {

bool AbductLogicProgramParser::check(logic::Program& pr, std::string& message) {
  std::set<logic::BoolVarPtr> obs = pr.getObsers();
  std::set<logic::BoolVarPtr> abd = pr.getAbducts();
  std::set<logic::BoolVarPtr> cls = pr.clausesPropositionalVariables();

  for (auto it = obs.begin(); it != obs.end(); it++)
    if (utils::contains(abd, **it)) {
      message = "The abductibles and observation sets must be disjunct.";
      return false;
    }

  for (auto it = abd.begin(); it != abd.end(); it++)
    if (utils::contains(obs, **it)) {
      message = "The abductibles and observation sets must be disjunct.";
      return false;
    }

  for (auto it = obs.begin(); it != obs.end(); it++)
    if (!utils::contains(cls, **it)) {
      message = "All observations should appear in the clauses";
      return false;
    }

  return true;
}

}  // namespace parsers

}  // namespace nalso
