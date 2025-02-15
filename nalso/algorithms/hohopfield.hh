#pragma once
/*
 * @file hohopfield.h
 *
 * Contains the implementation of the abduction algorithm for cost propositional
 * logic using high order recurrent neural networks
 *
 * @date Oct 30, 2009
 * @author Alexander Rojas <alexander.rojas@gmail.com>
 */

#include <eclipsepl/eclipseclass.h>

#include <list>
#include <set>
#include <sstream>
#include <string>

#include "nalso/algorithms/networkbuilder.hh"
#include "nalso/neural/hopfield.hh"

namespace nalso {

namespace algorithms {

/** Provides a representation of a conjunction of literals */
typedef std::set<logic::LiteralPtr> Conjunction;
/** Representation of a disjunction of conjunction of literals */
typedef std::set<logic::Conjunction> DisjunciveNormalForm;

/** @brief Representation of a clause with ors involved
 *
 * Provides a representation of a disjunctive union of different clauses. i.e.
 * if one has the clauses:
 *
 * p_1 && ... && p_n -> q
 * r_1 && ... && r_m -> q
 *
 * this object will represent the clause
 *
 * (p_1 && ... && p_n) || (r_1 && ... && r_m) -> q
 *
 */
class DisjunctionOfConjunctionsClause {
  friend class HighOrderHopfieldNetwork;

 private:
  logic::DisjunciveNormalForm body;
  logic::BoolVarPtr head;
  std::map<std::string, logic::LiteralPtr> allInTheClause;

 protected:
  // a map containing all the auxiliar variables used, in the value, first is
  // the a pointer to the consequent literal
  static std::map<std::string, std::pair<logic::LiteralPtr, logic::LiteralPtr> > usedAuxLiterals;
  static unsigned int index;

 public:
  /**
   * Construct and or clause with the given head.
   *
   * @param _head The head of the clause.
   */
  DisjunctionOfConjunctionsClause(logic::BoolVarPtr _head) : head(_head) {}

  /**
   * Adds the elements of the body of the given clause in a disjunctive way to
   * the disjunction of conjunctions that are already in the body if and only if
   * the head of the clause passed as a parameter is the same of the disjunctive
   * clause. If there's a variable that appears in another conjunction of the
   * body, automatically replaces it with a newly create auxiliary variable.
   *
   * @param cl The clause whose body will be added to this disjunctive clause.
   *
   * @return true if the body of cl was successfully added to this disjunctive
   * clause. false otherwise
   */
  bool addConjunction(logic::ClausePtr cl);
  /**
   * Add the given literal as a new disjunctive element.
   *
   * @param l the literal to be added to the body of the clause
   */
  void addConsequent(logic::LiteralPtr l);
  /**
   * Adds the conjunction of literals as a new disjunction element to the body
   * of this clause. If there's a variable that appears in another conjunction
   * of the body, automatically replaces it with a newly create auxiliary
   * variable.
   *
   * @param con the conjunction to be added to the body of the clause.
   */
  void addConjunctionOfLiterals(logic::Conjunction con);
  /**
   * Removes all the elements from the body of the clause.
   */
  void resetBody() { body.clear(); }
};
/// Defines a pointer to a DisjunctionOfConjunctionsClause
typedef std::shared_ptr<DisjunctionOfConjunctionsClause> DisjunctionOfConjunctionsClausePtr;

/** @brief Implements the Abduction algorithm for penalty logic using high order
 * recurrent neural networks
 *
 * This class builds a High order Hopfield neural network based on the algorithm
 * proposed by Aldebar in his 2003 paper, which describes how to use these
 * networks to perform abduction on penalty logic programs. Since we are not
 * dealing with penalty logic it sets the cost of each clause to 1000 and of
 * each abductive variable to 100. The cost of the goal is 1000*number of
 * clauses.
 *
 * @author Alexander Rojas <alexander.rojas@gmail.com>
 *
 */
class HighOrderHopfieldNetwork : public NNBuilderAlgo {
 protected:
  /**
   * Creates an eclipse prolog word of a list of LiteralPtr such that the
   * elements of the list are in a big conjunction in the returned word.
   *
   * @remark The list is used and purge recursively, so when the function
   * terminates the list is empty. If you need the list later one, create a copy
   * of it.
   *
   * @param con The list to be converted to a conjunction of its elements. This
   * list will be empty after calling the method
   *
   * @return An eclipse prolog word that contains a conjunction of the elements
   * passed in the list.
   */
  static eclipse_pl::EC_word createConjunction(std::list<logic::LiteralPtr>& con);
  /**
   * Creates an eclipse prolog representing a disjunction of the conjunction
   * elements in the list.
   *
   * @remark The list is used and purge recursively, so when the function
   * terminates the list is empty. If you need the list later one, create a copy
   * of it.
   *
   * @param dis The list to be converted to a disjunction of its elements. This
   * list will be empty after calling the method
   *
   * @return An eclipse prolog word that contains a disjunction of the elements
   * passed in the list.
   */
  static eclipse_pl::EC_word createDisjunctionOfConjunctions(
      list<Conjunction>& dis);
  /**
   * Creates an inverted clause of the elements passed in cl. i.e. if the clause
   * has the form p -> q The returned value will be q -> p.
   *
   * @param cl The clause to be transformed into an eclipse prolog object.
   *
   * @return an eclipse prolog object representing the clause passed as a
   * parameter.
   */
  static eclipse_pl::EC_word createClause(
      DisjunctionOfConjunctionsClausePtr cl);
  /**
   * Creates an inverted negated clause of the elements passed in cl. i.e. if
   * the clause has the form p -> q The returned value will be ¬(q -> p).
   *
   * @param cl The clause to be transformed into an eclipse prolog object.
   *
   * @return an eclipse prolog object representing the clause passed as a
   * parameter.
   */
  static eclipse_pl::EC_word createNegatedClause(
      DisjunctionOfConjunctionsClausePtr cl);
  /**
   * Creates the a polynomial of the negated inverted clause of cl using the
   * polynomial transformation algorithm described by aldeabar and works like
   * this:
   *
   * H(x) = {
   * 			x					if x = p
   * 			(1-H(y))			if x = ¬y
   * 			H(y)*H(z)			if x = y && z
   * 			H(y)+H(z)-H(y)*H(z)	if x = y || y
   * 		  }
   *
   * @remark The negated inverted clause works like this, if cl = p -> q then it
   * is ¬(q->p)
   *
   * @param cl The clause from which the polynomial is to be computed.
   *
   * @return A string representation of the polynomial that represents this
   * clause.
   */
  static std::string createPolynomial(DisjunctionOfConjunctionsClausePtr cl);
  static std::list<std::string> createTableRow(std::string monom);
  /**
   * Transforms a string that contains a polynomial (with not exponentials) and
   * creates a table which can be used to build the hyperlinks of the High order
   * Hopfield neural network.
   *
   * @param poly A string that contains a polynomial in multiple variables.
   *
   * @return a list of pairs. Each pair represents a monomial where the first
   * component represents the coefficient and the second is a list with the id's
   * of the variables of the monomial
   */
  static std::list<std::pair<sstd::tring, std::list<std::string> > > createTable(std::string poly);

 public:
  HighOrderHopfieldNetwork();
  virtual ~HighOrderHopfieldNetwork() {};

  virtual neural::NeuralNetworkPtr buildNetwork(ProgramPtr pr);
  /**
   * Creates a temporal program with the given clauses and uses the methods of a
   * program object to compute automatically the abductibles and observations.
   * Then it call buildNetwork method.
   *
   * @see buildNetwork(ProgramPtr pr)
   */
  virtual neural::NeuralNetworkPtr buildNetwork(std::set<ClausePtr> pr);
};

}  // namespace algorithms
}  // namespace nalso
