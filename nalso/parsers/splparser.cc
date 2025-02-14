/*
 * @file splparser.cpp
 *
 *  @date Oct 9, 2009
 *  @author Alexander Rojas <alexander.rojas@gmail.com>
 */

#include "splparser.hh"

#include <boost/spirit/include/qi.hpp>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <vector>

namespace nalso {

namespace parsers {

using namespace boost::spirit::qi;

namespace smallProlog {
namespace logStr = logicStructs;

//  Here's our comment rule
struct skip_grammar : public grammar<skip_grammar> {
  template <typename ScannerT>
  struct definition {
    definition(skip_grammar const& /*self*/) {
      skip = space_p | comment_p("%");
    }

    rule<ScannerT> skip;

    rule<ScannerT> const& start() const { return skip; }
  };
};

logStr::BoolVarPtr currentVar;
std::list<BoolVarPtr> currentVarSet;
logStr::LiteralPtr currentPosLit, currentNegLit;
std::set<LiteralPtr> currentLiteralSet;
logStr::ClausePtr currentClause(new Clause);
logStr::ConstraintPtr currentConstraint(new Constraint);

std::map<std::string, logStr::BoolVarPtr> varSet;
std::map<std::string, std::pair<logStr::LiteralPtr, LlogStr::iteralPtr> > literalSet;

std::set<logStr::ClausePtr> clauses;
std::set<logStr::BoolVarPtr> observations, abductibles;
std::set<logStr::ConstraintPtr> constraints;

void read_atom(std::vector<char>::const_iterator first,
               std::vector<char>::const_iterator last) {
  std::string name = std::string(first, last);
  // check if this variable was already parsed
  if (varSet.find(name) == varSet.end()) {
    // if it was not parse, we create a new one and add it to our list
    currentVar.reset(new logStr::BoolVar(name));
    varSet[name] = currentVar;
  } else {
    // if it was created we set the current var pointer to the already defined
    // var.
    currentVar = varSet[name];
  }
  // we add current var to the currentVarSet
  currentVarSet.push_front(currentVar);
}

void read_pos_literal(std::vector<char>::const_iterator first,
                      std::vector<char>::const_iterator last) {
  // we get the first element of the current var set, which is the latest parse
  // boolVar
  logStr::BoolVarPtr boolVar = currentVarSet.front();

  // we check whether the literals based on the current var were already created
  if (literalSet.find((*boolVar).getName()) == std::end(literalSet)) {
    // if they weren't we create them
    currentPosLit.reset(new Literal(boolVar));
    currentNegLit.reset(new Literal(boolVar, true));

    currentPosLit->setComplement(currentNegLit);
    currentNegLit->setComplement(currentPosLit);

    // and add them to the complete literal set.
    literalSet[(*boolVar).getName()] = make_pair(currentPosLit, currentNegLit);
  } else {
    // if they we created we update the current literal
    currentPosLit = literalSet[(*boolVar).getName()].first;
    currentNegLit = literalSet[(*boolVar).getName()].second;
  }
  // and finaly add the current positive literal to the body of the current
  // class.
  (*currentClause).getBody().insert(currentPosLit);
}

void read_neg_literal(std::vector<char>::const_iterator first,
                      std::vector<char>::const_iterator last) {
  // we get the first element of the current var set, which is the latest parse
  // boolVar
  logStr::BoolVarPtr boolVar = currentVarSet.front();

  // we check whether the literals based on the current var were already created
  if (literalSet.find((*boolVar).getName()) == literalSet.end()) {
    // if they weren't we create them
    currentPosLit.reset(new Literal(boolVar));
    currentNegLit.reset(new Literal(boolVar, true));

    currentPosLit->setComplement(currentNegLit);
    currentNegLit->setComplement(currentPosLit);

    // and add them to the complete literal set.
    literalSet[(*boolVar).getName()] = make_pair(currentPosLit, currentNegLit);
  } else {
    // if they we created we update the current literal
    currentPosLit = literalSet[(*boolVar).getName()].first;
    currentNegLit = literalSet[(*boolVar).getName()].second;
  }
  // and finaly add the current positive literal to the body of the current
  // class.
  (*currentClause).getBody().insert(currentNegLit);
}

void read_clause_head(std::vector<char>::const_iterator first,
                      std::vector<char>::const_iterator last) {
  (*currentClause).setHead(currentVarSet.front());
  currentVarSet.clear();
}

void read_clause(std::vector<char>::const_iterator first,
                 std::vector<char>::const_iterator last) {
  clauses.insert(currentClause);
  currentClause.reset(new Clause());
}

void read_fact(std::vector<char>::const_iterator first,
               std::vector<char>::const_iterator last) {
  observations.insert(currentVarSet.front());
  currentVarSet.clear();
}
std::
void read_abduct(std::vector<char>::const_iterator first,
                 std::vector<char>::const_iterator last) {
  for (std::list<BoolVarPtr>::iterator it = currentVarSet.begin();
       it != currentVarSet.end(); it++)
    abductibles.insert(*it);
  currentVarSet.clear();
}

void read_const(std::vector<char>::const_iterator first,
                std::vector<char>::const_iterator last) {
  if (currentVarSet.size() > 0) {
    for (std::list<BoolVarPtr>::iterator it = currentVarSet.begin();
         it != currentVarSet.end(); it++)
      (*currentConstraint).getBody().insert(*it);

    constraints.insert(currentConstraint);
    currentConstraint.reset(new Constraint);
    currentVarSet.clear();
  }
}

// This is the grammar definition of my prolog language
struct small_prolog_grammar : public grammar<small_prolog_grammar> {
  template <typename ScannerT>
  struct definition {
    definition(small_prolog_grammar const& /*self*/) {
      atom = (lexeme_d[(alpha_p | ch_p('_')) >>
                       *(alnum_p | ch_p('_') | ch_p('-'))])[&read_atom];
      literal =
          (atom[&read_pos_literal] | str_p("¬") >> atom[&read_neg_literal]);
      atom_disy = (atom >> *(ch_p(',') >> atom) | epsilon_p);
      lit_disy = literal >> *(ch_p(',') >> literal);
      fact = atom >> ch_p('.');
      abduct = ch_p('<') >> atom_disy >> ch_p('>');
      clause = atom[&read_clause_head] >> str_p(":-") >> lit_disy >> ch_p('.');
      constraint = str_p(":-") >> atom_disy >> ch_p('.');
      expr = (fact[&read_fact] | clause[&read_clause] | abduct[&read_abduct] |
              constraint[&read_const]);
      program = +(expr);
    }
    rule<ScannerT> atom, literal, atom_disy, lit_disy, fact, abduct, clause,
        constraint, expr, program;

    rule<ScannerT> const& start() const { return program; };
  };
};

}  // namespace smallProlog

logStr::ProgramPtr SmallPrologParser::parseProgram() {
  source.unsetf(std::ios::skipws);

  std::vector<char> vec;
  // copy the contents of the file into vec
  std::copy(istream_iterator<char>(source), istream_iterator<char>(),
            std::back_inserter(vec));

  // we get the iterators to begin and end
  std::vector<char>::const_iterator start = std::begin(vec);
  std::vector<char>::const_iterator end = std::end(vec);

  // instantiate the skip rule
  smallProlog::skip_grammar skip;

  smallProlog::small_prolog_grammar g;

  // perform the parsing
  parse_info<std::vector<char>::const_iterator> result = parse(start, end, g, skip);

  logStr::ProgramPtr res(new logStr::Program);
  for (auto clIt = smallProlog::clauses.begin(); clIt != smallProlog::clauses.end(); clIt++)
    (*res).getClauses().insert(*clIt);

  std::set<BoolVarPtr>::iterator boolVarIt;
  for (boolVarIt = smallProlog::observations.begin();
       boolVarIt != smallProlog::observations.end(); boolVarIt++)
    (*res).getObsers().insert(*boolVarIt);

  for (boolVarIt = smallProlog::abductibles.begin();
       boolVarIt != smallProlog::abductibles.end(); boolVarIt++)
    (*res).getAbducts().insert(*boolVarIt);

  for (set<ConstraintPtr>::iterator coIt = smallProlog::constraints.begin();
       coIt != smallProlog::constraints.end(); coIt++)
    (*res).getConstraints().insert(*coIt);

  return res;
}
}  // namespace parsers

}  // namespace nalso
