/*
 * alparser.cpp
 *
 *  Created on: Oct 9, 2009
 *      Author: Alexander Rojas <alexander.rojas@gmail.com>
 */

#include "alparser.hh"

namespace nalso {

namespace parsers {

bool AbductLogicProgramParser::check(Program& pr, string& message) {
  set<BoolVarPtr> obs = pr.getObsers();
  set<BoolVarPtr> abd = pr.getAbducts();
  set<BoolVarPtr> cls = pr.clausesPropositionalVariables();

  set<BoolVarPtr>::iterator it, aux;

  for (it = obs.begin(); it != obs.end(); it++)
    if (utils::isInSet(abd, **it, aux)) {
      message = "The abductibles and observation sets must be disjunct.";
      return false;
    }

  for (it = abd.begin(); it != abd.end(); it++)
    if (utils::isInSet(obs, **it, aux)) {
      message = "The abductibles and observation sets must be disjunct.";
      return false;
    }

  for (it = obs.begin(); it != obs.end(); it++)
    if (!utils::isInSet(cls, **it, aux)) {
      message = "All observations should appear in the clauses";
      return false;
    }

  return true;
}

}  // namespace parsers

}  // namespace nalso
