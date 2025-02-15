/*
 * @file splparser.cpp
 *
 *  @date Oct 9, 2009
 *  @author Alexander Rojas <alexander.rojas@gmail.com>
 */

#include "splparser.hh"

#include <boost/spirit/include/classic.hpp>
#include <list>
#include <map>
#include <memory>
#include <set>
#include <vector>

namespace nalso {

namespace parsers {

namespace spirit = boost::spirit::classic;

namespace smallProlog {

//  Here's our comment rule
struct skip_grammar : public spirit::grammar<skip_grammar> {
  template <typename ScannerT>
  struct definition {
    definition(skip_grammar const& /*self*/) {
      skip = spirit::space_p | spirit::comment_p("%");
    }

    spirit::rule<ScannerT> skip;

    spirit::rule<ScannerT> const& start() const { return skip; }
  };
};

logic::BoolVarPtr currentVar;
std::list<logic::BoolVarPtr> currentVarSet;
logic::LiteralPtr currentPosLit, currentNegLit;
std::set<logic::LiteralPtr> currentLiteralSet;
logic::ClausePtr currentClause(new logic::Clause());
logic::ConstraintPtr currentConstraint(new logic::Constraint());

std::map<std::string, logic::BoolVarPtr> varSet;
std::map<std::string, std::pair<logic::LiteralPtr, logic::LiteralPtr> > literalSet;

std::set<logic::ClausePtr> clauses;
std::set<logic::BoolVarPtr> observations, abductibles;
std::set<logic::ConstraintPtr> constraints;

void read_atom(std::vector<char>::const_iterator first,
               std::vector<char>::const_iterator last) {
  std::string name = std::string(first, last);
  // check if this variable was already parsed
  if (varSet.find(name) == std::end(varSet)) {
    // if it was not parse, we create a new one and add it to our list
    currentVar.reset(new logic::BoolVar(name));
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
  logic::BoolVarPtr boolVar = currentVarSet.front();

  // we check whether the literals based on the current var were already created
  if (literalSet.find((*boolVar).getName()) == std::end(literalSet)) {
    // if they weren't we create them
    currentPosLit.reset(new logic::Literal(boolVar));
    currentNegLit.reset(new logic::Literal(boolVar, true));

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
  logic::BoolVarPtr boolVar = currentVarSet.front();

  // we check whether the literals based on the current var were already created
  if (literalSet.find((*boolVar).getName()) == literalSet.end()) {
    // if they weren't we create them
    currentPosLit.reset(new logic::Literal(boolVar));
    currentNegLit.reset(new logic::Literal(boolVar, true));

    currentPosLit->setComplement(currentNegLit);
    currentNegLit->setComplement(currentPosLit);

    // and add them to the complete literal set.
    literalSet[(*boolVar).getName()] = make_pair(currentPosLit, currentNegLit);
  } else {
    // if they we created we update the current literal
    auto [pos, neg] = literalSet[(*boolVar).getName()];
    currentPosLit = pos;
    currentNegLit = neg;
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
  currentClause.reset(new logic::Clause());
}

void read_fact(std::vector<char>::const_iterator first,
               std::vector<char>::const_iterator last) {
  observations.insert(currentVarSet.front());
  currentVarSet.clear();
}

void read_abduct(std::vector<char>::const_iterator first,
                 std::vector<char>::const_iterator last) {
  for (auto it = currentVarSet.begin(); it != currentVarSet.end(); it++) {
    abductibles.insert(*it);
  }
  currentVarSet.clear();
}

void read_const(std::vector<char>::const_iterator first,
                std::vector<char>::const_iterator last) {
  if (currentVarSet.size() > 0) {
    for (auto it = currentVarSet.begin(); it != currentVarSet.end(); it++) {
      (*currentConstraint).getBody().insert(*it);
    }

    constraints.insert(currentConstraint);
    currentConstraint.reset(new logic::Constraint);
    currentVarSet.clear();
  }
}

// This is the grammar definition of my prolog language
struct small_prolog_grammar : public spirit::grammar<small_prolog_grammar> {
  template <typename ScannerT>
  struct definition {
    definition(small_prolog_grammar const& /*self*/) {
      using spirit::alnum_p;
      using spirit::alpha_p;
      using spirit::ch_p;
      using spirit::epsilon_p;
      using spirit::str_p;
      atom = (
        spirit::lexeme_d[(alpha_p | ch_p('_')) >> *(alnum_p | ch_p('_') | ch_p('-'))]
      )[&read_atom];
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

    spirit::rule<ScannerT> atom;
    spirit::rule<ScannerT> literal;
    spirit::rule<ScannerT> atom_disy;
    spirit::rule<ScannerT> lit_disy;
    spirit::rule<ScannerT> fact;
    spirit::rule<ScannerT> abduct;
    spirit::rule<ScannerT> clause;
    spirit::rule<ScannerT> constraint;
    spirit::rule<ScannerT> expr;
    spirit::rule<ScannerT> program;

    spirit::rule<ScannerT> const& start() const { return program; };
  };
};

}  // namespace smallProlog

logic::ProgramPtr SmallPrologParser::parseProgram() {
  source.unsetf(std::ios::skipws);

  std::vector<char> vec;
  // copy the contents of the file into vec
  std::copy(
    std::istream_iterator<char>(source),
    std::istream_iterator<char>(),
    std::back_inserter(vec)
  );

  // we get the iterators to begin and end
  std::vector<char>::const_iterator start = std::begin(vec);
  std::vector<char>::const_iterator end = std::end(vec);

  // instantiate the skip rule
  smallProlog::skip_grammar skip;
  smallProlog::small_prolog_grammar g;

  // perform the parsing
  parse(start, end, g, skip);

  namespace sprlg = smallProlog;

  logic::ProgramPtr res(new logic::Program);
  for (auto it = sprlg::clauses.begin(); it != sprlg::clauses.end(); it++) {
    (*res).getClauses().insert(*it);
  }

  for (auto it = sprlg::observations.begin(); it != sprlg::observations.end(); it++) {
    (*res).getObsers().insert(*it);
  }

  for (auto it = sprlg::abductibles.begin();
       it != sprlg::abductibles.end(); it++) {
    (*res).getAbducts().insert(*it);
  }

  for (auto it = sprlg::constraints.begin();it != sprlg::constraints.end(); it++) {
    (*res).getConstraints().insert(*it);
  }

  return res;
}
}  // namespace parsers
}  // namespace nalso
