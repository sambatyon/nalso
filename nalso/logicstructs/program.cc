/*
 * program.cpp
 *
 *  Created on: Sep 30, 2009
 *      Author: Alexander Rojas <alexander.rojas@gmail.com>
 */

#include "program.hh"

#include "nalso/utils/utils.hh"

namespace nalso {

namespace logicStructs {

BoolVar& BoolVar::operator=(const BoolVar& other) {
  if (this != &other) {
    name = other.name;
  }
  return *this;
}

Literal& Literal::operator=(const Literal& other) {
  if (this != &other) {
    var = other.var;
    negated = other.negated;
  }
  return *this;
}

/// returns the string representation of this literal
Literal::operator std::string() {
  std::string res;
  if (negated) {
    res += "¬";
  }
  res += (*var).getName();
  return res;
}
/// Returns a copy of the atom in the head
Literal::operator BoolVar() { return BoolVar(*var); }

/// Returns a shared pointer to the property atom
Literal::operator std::shared_ptr<BoolVar>() {
  return std::shared_ptr<BoolVar>(var);
}

void Clause::setBody(std::set<LiteralPtr>& _body) {
  body.clear();
  for (auto it = _body.begin(); it != _body.end(); it++) {
    body.insert(LiteralPtr(*it));
  }
}

bool Clause::containsInBody(BoolVar& atom) const {
  for (auto it = body.begin(); it != body.end(); it++) {
    if (**it == atom) {
      return true;
    }
  }
  return false;
}

bool Clause::containsInBody(Literal& lit) const {
  for (auto it = body.begin(); it != body.end(); it++) {
    if (**it == lit) {
      return true;
    }
  }
  return false;
}

void Clause::addToBody(BoolVarPtr var) {
  if (!containsInBody(*var)) {
    body.insert(LiteralPtr(new Literal(var)));
  }
}

void Clause::addToBody(LiteralPtr lit) {
  if (!containsInBody(*lit)) {
    body.insert(lit);
  }
}

Clause::operator std::string() {
  std::string res = *head;
  res += ":-";
  bool first = true;
  for (auto it = body.begin(); it != body.end(); it++) {
    if (!first) {
      res += ",";
    }
    first = false;
    res += **it;
  }
  res += ".";

  return res;
}

bool Clause::operator==(const Clause& other) const {
  if (this == &other) {
    return true;
  }
  if (*head != *other.head) {
    return false;
  }

  for (auto it = body.begin(); it != body.end(); it++) {
    if (!other.containsInBody(**it)) {
      return false;
    }
  }

  for (auto it = other.body.begin(); it != other.body.end(); it++) {
    if (!containsInBody(**it)) {
      return false;
    }
  }

  return true;
}

std::set<BoolVarPtr> Clause::getAllBoolVars() {
  std::set<BoolVarPtr> res;
  for (auto bodyIt = body.begin(); bodyIt != body.end(); bodyIt++) {
    if (!utils::isInSet(res, *(**bodyIt).getVar()))
      res.insert((**bodyIt).getVar());
  }

  return res;
}

void Constraint::setBody(std::set<BoolVarPtr>& _body) {
  body.clear();
  for (auto it = _body.begin(); it != body.end(); it++)
    body.insert(*it);
}

Constraint::operator std::string() {
  std::string res = ":-";
  bool first = true;
  for (auto it = body.begin(); it != body.end(); it++) {
    if (!first) res += ",";
    first = false;
    res += **it;
  }
  res += ".";

  return res;
}

bool Constraint::operator==(const Constraint& other) const {
  if (this == &other) {
    return true;
  }

  std::set<BoolVarPtr>::iterator it;
  for (it = body.begin(); it != body.end(); it++) {
    if (!other.containsInBody(**it)) {
      return false;
    }
  }

  for (it = other.body.begin(); it != other.body.end(); it++) {
    if (!containsInBody(**it)) {
      return false;
    }
  }

  return true;
}

bool Constraint::containsInBody(BoolVar& atom) const {
  for (auto it = body.begin(); it != body.end(); it++) {
    if (**it == atom) {
      return true;
    }
  }
  return false;
}

bool Constraint::containsInBody(Literal& lit) const {
  for (auto it = body.begin(); it != body.end(); it++) {
    if (**it == lit) {
      return true;
    }
  }
  return false;
}

Program::Program() {
  // TODO Auto-generated constructor stub
}

Program::~Program() {
  // TODO Auto-generated destructor stub
}

void Program::setClauses(std::set<ClausePtr>& _clauses) {
  clauses.clear();
  for (auto it = _clauses.begin(); it != _clauses.end(); it++) {
    clauses.insert(*it);
  }
}

void Program::setObsers(std::set<BoolVarPtr>& _obsers) {
  obsers.clear();
  for (auto it = _obsers.begin(); it != _obsers.end(); it++) {
    obsers.insert(*it);
  }
}

void Program::setAbducst(std::set<BoolVarPtr>& _abducts) {
  abducts.clear();
  for (auto it = _abducts.begin(); it != _abducts.end(); it++) {
    abducts.insert(*it);
  }
}

void Program::setConstraints(std::set<ConstraintPtr>& _consts) {
  consts.clear();
  for (auto it = _consts.begin(); it != _consts.end(); it++) {
    consts.insert(*it);
  }
}

Program::operator std::string() {
  std::string str("%Automatically generated as small prolog\n");

  str += "%program clauses\n";
  for (auto iter = clauses.begin(); iter != clauses.end(); iter++) {
    str += **iter;
    str += "\n";
  }

  str += "\n%observations\n";
  for (auto iter = obsers.begin(); iter != obsers.end(); iter++) {
    str += (std::string) * *iter;
    str += ".\n";
  }

  str += "\n%abductibles\n";
  bool first = true;
  str += "<";
  for (auto iter = abducts.begin(); iter != abducts.end(); iter++) {
    if (!first) str += ",";
    first = false;
    str += **iter;
  }
  str += ">\n";

  str += "\n%constraints\n";
  for (auto iter = consts.begin(); iter != consts.end(); iter++) {
    str += **iter;
    str += "\n";
  }

  return str;
}

std::set<BoolVarPtr> Program::allPropositionalVariables() {
  std::set<BoolVarPtr> res;
  // we add all the observations
  for (auto it = obsers.begin(); it != obsers.end(); it++) {
    res.insert(*it);
  }

  // we add all the abductibles if they're not in the list
  for (auto it = abducts.begin(); it != abducts.end(); it++) {
    if (!utils::isInSet(res, **it)) {
      res.insert(*it);
    }
  }

  std::set<BoolVarPtr> aux = clausesPropositionalVariables();
  for (auto it = aux.begin(); it != aux.end(); it++) {
    if (!utils::isInSet(res, **it)) {
      res.insert(*it);
    }
  }

  aux = constrainsPropositionalVariables();
  for (auto it = aux.begin(); it != aux.end(); it++) {
    if (!utils::isInSet(res, **it)) {
      res.insert(*it);
    }
  }

  return res;
}

std::set<BoolVarPtr> Program::constrainsPropositionalVariables() {
  std::set<BoolVarPtr> res;
  // now we add all the clauses atoms if they're not already in the list
  for (auto conIt = consts.begin(); conIt != consts.end(); conIt++) {
    // now we add the variables in the body
    for (auto bodyIt = (**conIt).getBody().begin();
         bodyIt != (**conIt).getBody().end(); bodyIt++) {
      if (!utils::isInSet(res, **bodyIt)) {
        res.insert(*bodyIt);
      }
    }
  }

  return res;
}

std::set<BoolVarPtr> Program::clausesPropositionalVariables() {
  std::set<BoolVarPtr> res;
  // now we add all the clauses atoms if they're not already in the list
  for (auto clIt = clauses.begin(); clIt != clauses.end(); clIt++) {
    // first we check the head.
    if (!utils::isInSet(res, *(**clIt).getHead())) {
      res.insert((**clIt).getHead());
    }

    // now we add the variables in the body
    for (auto bodyIt = (**clIt).getBody().begin(); bodyIt != (**clIt).getBody().end(); bodyIt++) {
      if (!utils::isInSet(res, *(**bodyIt).getVar())) {
        res.insert((**bodyIt).getVar());
      }
    }
  }

  return res;
}

void Program::fillAbducts() {
  if (abducts.size() < 1) {
    std::set<BoolVarPtr> allAtomsSet = allPropositionalVariables();
    for (auto it = allAtomsSet.begin(); it != allAtomsSet.end(); it++) {
      bool isHead = false;
      for (auto itCls = clauses.begin(); itCls != clauses.end(); itCls++)
        if ((std::string)(*(*itCls)->getHead()) == (std::string)(**it)) {
          isHead = true;
          break;
        }
      if (!isHead) {
        abducts.insert(*it);
      }
    }
  }
}

void Program::fillObsers() {
  if (obsers.size() < 1) {
    // first we go through all the clauses, since we will check all the clause
    // heads
    for (auto itHeads = clauses.begin(); itHeads != clauses.end(); itHeads++) {
      bool appearsInBody = false;  // true if the head of the current clause
                                   // appears in the body of a clause
      // now we iterate over all clauses
      for (auto itBodies = clauses.begin(); itBodies != clauses.end(); itBodies++) {
        // we check whether the current head appears in the body of a clause, if
        // so, we stop searching
        if ((**itBodies).containsInBody(*(**itHeads).getHead())) {
          appearsInBody = true;
          break;
        }
      }
      // if not appears in the body of any clause, nor is already in the list of
      // observations, we add it
      if (!appearsInBody && !utils::isInSet(obsers, *(**itHeads).getHead()))
        obsers.insert((**itHeads).getHead());
    }
  }
}

}  // namespace logicStructs
}  // namespace nalso
