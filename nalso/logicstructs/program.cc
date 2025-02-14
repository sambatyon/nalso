/*
 * program.cpp
 *
 *  Created on: Sep 30, 2009
 *      Author: Alexander Rojas <alexander.rojas@gmail.com>
 */

#include "program.hh"

#include <utils/utils.hh>

namespace nalso {

namespace logicStructs {

BoolVar& BoolVar::operator=(const BoolVar& other) {
  if (this != &other) name = other.name;
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
Literal::operator string() {
  string res;
  if (negated) res += "¬";
  res += (*var).getName();
  return res;
}
/// Returns a copy of the atom in the head
Literal::operator BoolVar() { return BoolVar(*var); }
/// Returns a shared pointer to the property atom
Literal::operator shared_ptr<BoolVar>() { return shared_ptr<BoolVar>(var); }

void Clause::setBody(set<LiteralPtr>& _body) {
  body.clear();
  for (set<LiteralPtr>::iterator it = _body.begin(); it != _body.end(); it++)
    body.insert(LiteralPtr(*it));
}

bool Clause::containsInBody(BoolVar& atom) const {
  set<LiteralPtr>::iterator it;
  for (it = body.begin(); it != body.end(); it++) {
    if (**it == atom) return true;
  }
  return false;
}

bool Clause::containsInBody(Literal& lit) const {
  set<LiteralPtr>::iterator it;
  for (it = body.begin(); it != body.end(); it++) {
    if (**it == lit) return true;
  }
  return false;
}

void Clause::addToBody(BoolVarPtr var) {
  if (!containsInBody(*var)) body.insert(LiteralPtr(new Literal(var)));
}
void Clause::addToBody(LiteralPtr lit) {
  if (!containsInBody(*lit)) body.insert(lit);
}

Clause::operator string() {
  string res = *head;
  res += ":-";
  bool first = true;
  for (set<LiteralPtr>::iterator it = body.begin(); it != body.end(); it++) {
    if (!first) res += ",";
    first = false;
    res += **it;
  }
  res += ".";

  return res;
}

bool Clause::operator==(const Clause& other) const {
  if (this == &other) return true;
  if (*head != *other.head) return false;

  set<LiteralPtr>::iterator it;
  for (it = body.begin(); it != body.end(); it++) {
    if (!other.containsInBody(**it)) return false;
  }

  for (it = other.body.begin(); it != other.body.end(); it++) {
    if (!containsInBody(**it)) return false;
  }

  return true;
}

set<BoolVarPtr> Clause::getAllBoolVars() {
  set<BoolVarPtr> res;
  set<BoolVarPtr>::iterator setIt;
  for (set<LiteralPtr>::iterator bodyIt = body.begin(); bodyIt != body.end();
       bodyIt++) {
    if (!utils::isInSet(res, *(**bodyIt).getVar(), setIt))
      res.insert((**bodyIt).getVar());
  }

  return res;
}

void Constraint::setBody(set<BoolVarPtr>& _body) {
  body.clear();
  for (set<BoolVarPtr>::iterator it = _body.begin(); it != body.end(); it++)
    body.insert(*it);
}

Constraint::operator string() {
  string res = ":-";
  bool first = true;
  for (set<BoolVarPtr>::iterator it = body.begin(); it != body.end(); it++) {
    if (!first) res += ",";
    first = false;
    res += **it;
  }
  res += ".";

  return res;
}

bool Constraint::operator==(const Constraint& other) const {
  if (this == &other) return true;

  set<BoolVarPtr>::iterator it;
  for (it = body.begin(); it != body.end(); it++) {
    if (!other.containsInBody(**it)) return false;
  }

  for (it = other.body.begin(); it != other.body.end(); it++) {
    if (!containsInBody(**it)) return false;
  }

  return true;
}

bool Constraint::containsInBody(BoolVar& atom) const {
  for (set<BoolVarPtr>::iterator it = body.begin(); it != body.end(); it++)
    if (**it == atom) return true;
  return false;
}

bool Constraint::containsInBody(Literal& lit) const {
  for (set<BoolVarPtr>::iterator it = body.begin(); it != body.end(); it++)
    if (**it == lit) return true;
  return false;
}

Program::Program() {
  // TODO Auto-generated constructor stub
}

Program::~Program() {
  // TODO Auto-generated destructor stub
}

void Program::setClauses(set<ClausePtr>& _clauses) {
  clauses.clear();
  for (set<ClausePtr>::iterator it = _clauses.begin(); it != _clauses.end();
       it++)
    clauses.insert(*it);
}

void Program::setObsers(set<BoolVarPtr>& _obsers) {
  obsers.clear();
  for (set<BoolVarPtr>::iterator it = _obsers.begin(); it != _obsers.end();
       it++)
    obsers.insert(*it);
}

void Program::setAbducst(set<BoolVarPtr>& _abducts) {
  abducts.clear();
  for (set<BoolVarPtr>::iterator it = _abducts.begin(); it != _abducts.end();
       it++)
    abducts.insert(*it);
}

void Program::setConstraints(set<ConstraintPtr>& _consts) {
  consts.clear();
  for (set<ConstraintPtr>::iterator it = _consts.begin(); it != _consts.end();
       it++)
    consts.insert(*it);
}

Program::operator string() {
  string str("%Automatically generated as small prolog\n");

  str += "%program clauses\n";
  for (set<ClausePtr>::iterator clauseIter = clauses.begin();
       clauseIter != clauses.end(); clauseIter++) {
    str += **clauseIter;
    str += "\n";
  }

  str += "\n%observations\n";
  for (set<BoolVarPtr>::iterator obsIter = obsers.begin();
       obsIter != obsers.end(); obsIter++) {
    str += (string) * *obsIter;
    str += ".\n";
  }

  str += "\n%abductibles\n";
  bool first = true;
  str += "<";
  for (set<BoolVarPtr>::iterator abdIter = abducts.begin();
       abdIter != abducts.end(); abdIter++) {
    if (!first) str += ",";
    first = false;
    str += **abdIter;
  }
  str += ">\n";

  str += "\n%constraints\n";
  for (set<ConstraintPtr>::iterator consIter = consts.begin();
       consIter != consts.end(); consIter++) {
    str += **consIter;
    str += "\n";
  }

  return str;
}

set<BoolVarPtr> Program::allPropositionalVariables() {
  set<BoolVarPtr> res;
  set<BoolVarPtr>::iterator it, inputIt;
  // we add all the observations
  for (it = obsers.begin(); it != obsers.end(); it++) res.insert(*it);

  // we add all the abductibles if they're not in the list
  for (it = abducts.begin(); it != abducts.end(); it++)
    if (!utils::isInSet(res, **it, inputIt)) res.insert(*it);

  set<BoolVarPtr> aux = clausesPropositionalVariables();
  for (it = aux.begin(); it != aux.end(); it++)
    if (!utils::isInSet(res, **it, inputIt)) res.insert(*it);

  aux = constrainsPropositionalVariables();
  for (it = aux.begin(); it != aux.end(); it++)
    if (!utils::isInSet(res, **it, inputIt)) res.insert(*it);

  return res;
}

set<BoolVarPtr> Program::constrainsPropositionalVariables() {
  set<BoolVarPtr> res;
  set<BoolVarPtr>::iterator it;
  // now we add all the clauses atoms if they're not already in the list
  for (set<ConstraintPtr>::iterator conIt = consts.begin();
       conIt != consts.end(); conIt++) {
    // now we add the variables in the body
    for (set<BoolVarPtr>::iterator bodyIt = (**conIt).getBody().begin();
         bodyIt != (**conIt).getBody().end(); bodyIt++)
      if (!utils::isInSet(res, **bodyIt, it)) res.insert(*bodyIt);
  }

  return res;
}

set<BoolVarPtr> Program::clausesPropositionalVariables() {
  set<BoolVarPtr> res;
  set<BoolVarPtr>::iterator it, inputIt;
  // now we add all the clauses atoms if they're not already in the list
  for (set<ClausePtr>::iterator clIt = clauses.begin(); clIt != clauses.end();
       clIt++) {
    // first we check the head.
    if (!utils::isInSet(res, *(**clIt).getHead(), inputIt))
      res.insert((**clIt).getHead());

    // now we add the variables in the body
    for (set<LiteralPtr>::iterator bodyIt = (**clIt).getBody().begin();
         bodyIt != (**clIt).getBody().end(); bodyIt++)
      if (!utils::isInSet(res, *(**bodyIt).getVar(), it))
        res.insert((**bodyIt).getVar());
  }

  return res;
}

void Program::fillAbducts() {
  if (abducts.size() < 1) {
    set<BoolVarPtr> allAtomsSet = allPropositionalVariables();
    for (set<BoolVarPtr>::iterator it = allAtomsSet.begin();
         it != allAtomsSet.end(); it++) {
      bool isHead = false;
      for (set<ClausePtr>::iterator itCls = clauses.begin();
           itCls != clauses.end(); itCls++)
        if ((string)(*(*itCls)->getHead()) == (string)(**it)) {
          isHead = true;
          break;
        }
      if (!isHead) abducts.insert(*it);
    }
  }
}

void Program::fillObsers() {
  if (obsers.size() < 1) {
    set<ClausePtr>::iterator itHeads, itBodies;
    set<BoolVarPtr>::iterator it;
    // first we go through all the clauses, since we will check all the clause
    // heads
    for (itHeads = clauses.begin(); itHeads != clauses.end(); itHeads++) {
      bool appearsInBody = false;  // true if the head of the current clause
                                   // appears in the body of a clause
      // now we iterate over all clauses
      for (itBodies = clauses.begin(); itBodies != clauses.end(); itBodies++) {
        // we check whether the current head appears in the body of a clause, if
        // so, we stop searching
        if ((**itBodies).containsInBody(*(**itHeads).getHead())) {
          appearsInBody = true;
          break;
        }
      }
      // if not appears in the body of any clause, nor is already in the list of
      // observations, we add it
      if (!appearsInBody &&
          !nalso::utils::isInSet(obsers, *(**itHeads).getHead(), it))
        obsers.insert((**itHeads).getHead());
    }
  }
}

}  // namespace logicStructs

}  // namespace nalso
