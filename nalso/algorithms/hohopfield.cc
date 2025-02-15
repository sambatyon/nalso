/*
 * hohopfield.cpp
 *
 *  Created on: Oct 30, 2009
 *      Author: diego
 */

#include "hohopfield.hh"

#include <ginac/ginac.h>

#include <algorithm>
#include <vector>

namespace nalso {

namespace algorithms {

unsigned int DisjunctionOfConjunctionsClause::index = 0;
map<string, pair<LiteralPtr, LiteralPtr> >
    DisjunctionOfConjunctionsClause::usedAuxLiterals;

void DisjunctionOfConjunctionsClause::addConsequent(LiteralPtr l) {
  Conjunction ncon;
  ncon.insert(l->isNegated() ? l->getComplement() : l);
  body.insert(ncon);
}

bool DisjunctionOfConjunctionsClause::addConjunction(ClausePtr cl) {
  // if the new clause has another head, we don't add it
  if (*(cl->getHead()) == *head) return false;

  addConjunctionOfLiterals(cl->getBody());
  return true;
}

void DisjunctionOfConjunctionsClause::addConjunctionOfLiterals(
    Conjunction con) {
  Conjunction ncon;
  for (Conjunction::iterator it = con.begin(); it != con.end(); it++) {
    // we check that the literal is not used yet in this clause
    if (allInTheClause.find(*(*it)->getVar()) == allInTheClause.end())
      ncon.insert(*it);
    else {  // else, we replace it for a fresh aux variable
      stringstream ss;
      ss << index;
      index++;
      string aux = "aux_";

      // create a new literal for the list
      BoolVarPtr tmp(new BoolVar(aux + ss.str()));
      LiteralPtr posLit(new Literal(tmp));
      LiteralPtr negLit(new Literal(tmp, true));
      posLit->setComplement(negLit);
      negLit->setComplement(posLit);
      usedAuxLiterals[aux + ss.str()].first =
          *it;  // it is the consequent which has the new literal as consequence
      usedAuxLiterals[aux + ss.str()].second = posLit;

      // add the new literal instead of the original one
      ncon.insert((*it)->isNegated() ? negLit : posLit);
    }
  }

  body.insert(ncon);
}

HighOrderHopfieldNetwork::HighOrderHopfieldNetwork() {
  // TODO Auto-generated constructor stub
}

eclipse_pl::EC_word HighOrderHopfieldNetwork::createNegatedClause(
    DisjunctionOfConjunctionsClausePtr cl) {
  eclipse_pl::EC_functor _neg((char*)"neg", 1);
  return term(_neg, createClause(cl));
}

eclipse_pl::EC_word HighOrderHopfieldNetwork::createClause(
    DisjunctionOfConjunctionsClausePtr cl) {
  using namespace eclipse_pl;
  EC_functor _or((char*)"or", 2), _neg((char*)"neg", 1);

  // get the head name
  string head = *(*cl).head;
  // the name has to be made lowercase or it will be treated as a variable in
  // prolog
  std::transform(head.begin(), head.end(), head.begin(), ::tolower);
  // create a list with the conjunctions of the body
  std::list<Conjunction> body;
  for (DisjunciveNormalForm::iterator dit = cl->body.begin();
       dit != cl->body.end(); dit++)
    body.push_back(*dit);

  return term(_or, term(_neg, EC_atom((char*)head.c_str())),
              createDisjunctionOfConjunctions(body));
}

eclipse_pl::EC_word HighOrderHopfieldNetwork::createDisjunctionOfConjunctions(
    list<Conjunction>& dis) {
  eclipse_pl::EC_functor _or((char*)"or", 2);

  // we take the first element on the set of the list and transform it on a list
  Conjunction front = dis.front();
  list<LiteralPtr> frontList;
  for (Conjunction::iterator cit = front.begin(); cit != front.end(); cit++)
    frontList.push_back(*cit);
  // now we get the formula representing the conjunction of conjunction
  eclipse_pl::EC_word con = createConjunction(frontList);
  // delete the first element from the list
  dis.pop_front();
  // if dis is empty we return con, otherwise we create a disjunction between
  // con and the rest of the list
  return dis.empty() ? con
                     : term(_or, con, createDisjunctionOfConjunctions(dis));
}

eclipse_pl::EC_word HighOrderHopfieldNetwork::createConjunction(
    list<LiteralPtr>& con) {
  eclipse_pl::EC_functor _and((char*)"and", 2), _neg((char*)"neg", 1);
  LiteralPtr lit = con.front();
  string litnam = *lit->getVar();
  // the name has to be made lowercase or it will be treated as a variable in
  // prolog
  std::transform(litnam.begin(), litnam.end(), litnam.begin(), ::tolower);
  eclipse_pl::EC_word litw = eclipse_pl::EC_atom((char*)litnam.c_str());
  if (lit->isNegated()) litw = term(_neg, litw);

  con.pop_front();
  return con.empty() ? litw : term(_and, litw, createConjunction(con));
}

string HighOrderHopfieldNetwork::createPolynomial(
    DisjunctionOfConjunctionsClausePtr cl) {
  using namespace eclipse_pl;
  eclipse_pl::EC_word plClause = createNegatedClause(cl);

  EC_ref c_res;
  post_goal(term(EC_functor((char*)"compile", 1), (char*)"program.pl"));
  EC_resume();

  post_goal(term(EC_functor((char*)"eva", 2), plClause, c_res));
  EC_resume();

  char* s;
  ((EC_word)c_res).is_string(&s);

  string res = s;
  return res;
}

list<string> HighOrderHopfieldNetwork::createTableRow(string monom) {
  regex re("\\*");
  sregex_token_iterator riti(monom.begin(), monom.end(), re, -1);
  sregex_token_iterator rempty;
  list<string> res;
  while (riti != rempty) res.push_back(*riti++);
  return res;
}

list<pair<string, list<string> > > HighOrderHopfieldNetwork::createTable(
    string poly) {
  list<pair<string, list<string> > > res;

  regex re("\\+");
  sregex_token_iterator riti(poly.begin(), poly.end(), re, -1);
  sregex_token_iterator rempty;
  while (riti != rempty) {
    list<string> tmp = createTableRow(*riti++);
    string key = tmp.front();
    // check if the first element is a number, if it is not, the coefficient of
    // the polynomial is 1
    stringstream ss(key);
    int itmp;
    if (ss >> itmp)
      tmp.pop_front();
    else
      key = "1";
    res.push_back(pair<string, list<string> >(key, tmp));
  }

  return res;
}

NeuralNetworkPtr HighOrderHopfieldNetwork::buildNetwork(ProgramPtr pr) {
  eclipse_pl::ec_init();

  map<string, DisjunctionOfConjunctionsClausePtr> clauses;

  // here the clauses are added to a disjunction according to the head
  for (set<ClausePtr>::iterator it = pr->getClauses().begin();
       it != pr->getClauses().end(); it++) {
    if (clauses.find(*(*it)->getHead()) == clauses.end())
      clauses[*(*it)->getHead()].reset(
          new DisjunctionOfConjunctionsClause((*it)->getHead()));

    clauses[*(*it)->getHead()]->addConjunction(*it);
  }

  // create the goal clause
  clauses["goal__"].reset(
      new DisjunctionOfConjunctionsClause(BoolVarPtr(new BoolVar("goal__"))));
  Conjunction goalElem;
  for (set<BoolVarPtr>::iterator it = pr->getObsers().begin();
       it != pr->getObsers().end(); it++)
    goalElem.insert(LiteralPtr(new Literal(*it)));
  clauses["goal__"]->addConjunctionOfLiterals(goalElem);

  // add the clauses for the aux variables
  for (map<string, pair<LiteralPtr, LiteralPtr> >::iterator it =
           DisjunctionOfConjunctionsClause::usedAuxLiterals.begin();
       it != DisjunctionOfConjunctionsClause::usedAuxLiterals.end(); it++) {
    clauses[(*it).first].reset(
        new DisjunctionOfConjunctionsClause((*it).second.second->getVar()));
    clauses[(*it).first]->addConsequent((*it).second.first);
  }

  // create the string representation of the polynomials
  list<string> polynomials;
  for (map<string, DisjunctionOfConjunctionsClausePtr>::iterator it =
           clauses.begin();
       it != clauses.end(); it++)
    polynomials.push_back("1000*(" + createPolynomial((*it).second) + ")");

  // add the goal value
  stringstream ss;
  ss << 1000 * polynomials.size();
  polynomials.push_back("-" + ss.str() + "goal__");
  // add the abductibles values
  for (set<BoolVarPtr>::iterator it = pr->getAbducts().begin();
       it != pr->getAbducts().end(); it++) {
    string varname = **it;
    // the name has to be made lowercase or it will be treated as a variable in
    // prolog
    std::transform(varname.begin(), varname.end(), varname.begin(), ::tolower);
    polynomials.push_back("100*" + varname);
  }

  // we close the conetion to eclipse prolog
  eclipse_pl::ec_cleanup();

  // now it is time to create the big polynomial which will be reduced
  bool first = true;
  string fullPoly = "-(";  // done because in the paper it requieres a sign
                           // change (don't know why)
  for (list<string>::iterator it = polynomials.begin(); it != polynomials.end();
       it++) {
    if (!first) fullPoly += "+";
    fullPoly += *it;
    first = false;
  }
  fullPoly += ")";

  // now we use ginac to get the monomials which will contain the information
  // for the weights of the net
  string expandedPoly;
  // we also create a copy to initialize the network nodes
  list<string> fullnodelist;
  {
    using namespace GiNaC;
    lst smb;
    // we add the goal to the list of symbols
    smb.append(symbol("goal__"));
    fullnodelist.push_back("goal__");
    // we add the aux variables to the list of symbosl
    for (map<string, pair<LiteralPtr, LiteralPtr> >::iterator it =
             DisjunctionOfConjunctionsClause::usedAuxLiterals.begin();
         it != DisjunctionOfConjunctionsClause::usedAuxLiterals.end(); it++) {
      string varname = (*it).first;
      // the name has to be made lowercase or it will be treated as a variable
      // in prolog
      std::transform(varname.begin(), varname.end(), varname.begin(),
                     ::tolower);
      smb.append(symbol(varname));
      fullnodelist.push_back(varname);
    }
    set<BoolVarPtr> prop = pr->allPropositionalVariables();
    for (set<BoolVarPtr>::iterator it = prop.begin(); it != prop.end(); it++) {
      string varname = **it;
      // the name has to be made lowercase or it will be treated as a variable
      // in prolog
      std::transform(varname.begin(), varname.end(), varname.begin(),
                     ::tolower);
      smb.append(symbol(varname));
      fullnodelist.push_back(varname);
    }
    // now we run ginac
    ex poly(fullPoly, smb);
    poly = poly.expand();
    ss.str("");
    ss << poly;
    expandedPoly = ss.str();
  }
  // now we modified the string so we can split it properly
  const regex e("-");
  expandedPoly =
      regex_replace(expandedPoly, e, string("+-"), match_default | format_sed);
  if (expandedPoly[0] == '+') expandedPoly.erase(0, 1);

  // now we split the polynomial so we have a nice table
  list<pair<string, list<string> > > hyperWeights = createTable(expandedPoly);

  shared_ptr<HopfieldNeuralNetwork> res(new HopfieldNeuralNetwork);
  for (list<string>::iterator nodeName = fullnodelist.begin();
       nodeName != fullnodelist.end(); nodeName++)
    res->addNode(HopfieldNodePtr(new HopfieldNode(*nodeName)));

  // and construct the network
  for (list<pair<string, list<string> > >::iterator it = hyperWeights.begin();
       it != hyperWeights.end(); it++) {
    double weight;
    ss.str((*it).first);
    ss >> weight;

    vector<string> nodes;
    for (list<string>::iterator node = (*it).second.begin();
         node != (*it).second.end(); node++)
      nodes.push_back(*node);

    // we add the hyper connection to the network
    res->connectNodes(nodes, weight);
  }

  return res;
}

NeuralNetworkPtr HighOrderHopfieldNetwork::buildNetwork(set<ClausePtr> pr) {
  ProgramPtr prog(new Program);

  prog->setClauses(pr);
  prog->fillAbducts();
  prog->fillObsers();

  return buildNetwork(prog);
}
}  // namespace algorithms

}  // namespace nalso
