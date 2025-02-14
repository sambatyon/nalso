/**
 * @file program.h
 * @brief Contains the definition of all the needed structures to represent an
 * abductive logic programming program.
 *
 * Defines the five core classes needed to represent an abductive logic
 * programming program. As well as the typedefs to its pointers. Four of this
 * classes are used inside the fifth class (Program) which works as a full
 * container.
 *
 * For a next iteration, a Literal must be a subclass of BoolVar and contain a
 * BoolVar
 *
 *  @date Sep 30, 2009
 *  @author Alexander Rojas <alexander.rojas@gmail.com>
 */

#ifndef PROGRAM_H_
#define PROGRAM_H_

#include <boost/shared_ptr.hpp>
#include <list>
#include <set>
#include <string>
/**
 * @brief Main namespace for the project.
 *
 * This is the namespace that contains all the classes required by the naslo
 * project.
 */
namespace nalso {

/**
 * @brief container for the structures used by the representation of an
 * abductive logc programming program.
 *
 * It is a container for all the structures needed to represent an abductive
 * logic programming program.
 */
namespace logicStructs {
using namespace std;
using namespace boost;

/**
 * @brief Represents an variable in a logic program.
 *
 * Represents an variable in a logic program. Just Contains one attribute but it
 * is done in this way so it can be compatible with the literal class.
 *
 * @author Alexander Rojas
 *
 * @see nalso::logicStructs::Literal
 */
class BoolVar {
 private:
  string name; /**< Name of the propositional variable.*/
 public:
  /**
   * Clones the variable passed as a reference such that *this == at and this !=
   * &at
   *
   * @param at the boolean variable to be cloned.
   */
  BoolVar(BoolVar const& at) { name = at.name; }
  /**
   * Creates a boolean variable with the given name.
   *
   * @param _name The name of the newly create variable.
   */
  BoolVar(string _name) : name(_name) {}
  /**
   * Creates a boolean variable with the given name.
   *
   * @param _name The name of the newly create variable.
   */
  BoolVar(const char* _name) : name((char*)_name) {}
  /**
   * Creates a boolean variable with the given name.
   *
   * @param _name The name of the newly create variable.
   */
  BoolVar(char* _name) : name(_name) {}

  /**
   * Returns a string representation of the variable, useful as a casting
   * operator.
   *
   * @remark When casting an object of type BoolVar to a string an error occur.
   * However there's no problem using the object as a string without anz
   * casting.
   */
  operator std::string() { return name; }
  /*
   * Returns a c string representation of the variable.
   */
  // operator char*(){return (char*)name.c_str();}

  /**
   * Clones the variable such that
   * <code>*this == other</code> but <code>this != &other</code>
   *
   * @param other The variable to be cloned.
   *
   * @return A reference to this object.
   */
  BoolVar& operator=(const BoolVar& other);
  /**
   * Sets the attribute name of this variable to the one passed as a parameter.
   *
   * @param _name name that the returned variable will hold.
   *
   * @return a reference to this.
   */
  BoolVar& operator=(const string _name) {
    name = _name;
    return *this;
  }
  /**
   * Sets the attribute name of this variable to the one passed as a parameter.
   *
   * @param _name name that the returned variable will hold.
   *
   * @return a reference to this.
   */
  BoolVar& operator=(const char* _name) {
    name = _name;
    return *this;
  }

  /**
   * To objects are consider to be equal if their attribute values, in this case
   * the name, are equal.
   *
   * @remark if you want to see if the objects are indeed identical, i.e. They
   * refer to the same object, use <code>&boolVar1 == &boolVar2</code> which
   * compares the actual memory addresses of the two objects.
   *
   * @param other The object that will be compared agains this.
   *
   * @return true if the two variables have the same name, false otherwise.
   */
  bool operator==(const BoolVar& other) { return name == other.name; }
  /**
   * Calls the equality operator (==) and returns the opposite value.
   *
   * @param other The object that will be compared agains this.
   *
   * @return false if the two variables have the same name, true otherwise.
   *
   * @see operator==(const BoolVar&other)
   */
  bool operator!=(const BoolVar& other) { return !(*this == other); }

  /**
   * Setter for the name property.
   *
   * @param _name a new name for the object.
   */
  void setName(const string _name) { name = _name; }

  /**
   * Setter for the name property.
   *
   * @param _name a new name for the object.
   *
   * @see setName(const string _name)
   */
  void setName(const char* _name) { name = _name; }
  /**
   * Getter for the name propertie.
   *
   * @return name property.
   */
  string getName() { return name; }
};
/// A shared pointer to an variable object
/// @see nalso::logicStructs::BoolVar
typedef shared_ptr<BoolVar> BoolVarPtr;

/**
 * @brief Abstract representation of a prepositional logic literal.
 *
 * Represents a literal in a logic program. i.e. a logical variable (for the
 * prepositional case they coincide) which can be negated.
 *
 * @author Alexander Rojas
 */
class Literal {
 private:
  shared_ptr<Literal> complement;
  BoolVarPtr var;
  bool negated;

 public:
  /**
   * Creates a literal object with a clone of the BoolVar _var and a truth value
   * _negated. The new literal, after constructed will have the following
   * property: <code>*(this->var) == _var</code> but <code>this->var !=
   * &_var</code>
   *
   * @param _var A reference to an BoolVar object which will be cloned.
   *
   * @param _negated indicates if this is a negated literal (true) or a positive
   * literal (false)
   */
  Literal(BoolVar& _var, bool _negated = false) : negated(_negated) {
    var.reset(new BoolVar(_var));
  }
  /**
   * Creates a new literal which contains _var as its BoolVar object. i.e. The
   * var pointer will point to the same object (and therefore memory address) as
   * _var
   *
   * @param _var Pointer that will be used as destination for the var object.
   * @param _negated indicates if this is a negated literal (true) or a positive
   * literal (false)
   */
  Literal(BoolVarPtr _var, bool _negated = false) : negated(_negated) {
    var = _var;
  }

  /**
   * Calling this constructor will created a cloned copy of the literal passed
   * as a parameter and which share the same var object (the two literals point
   * to the same BoolVar* object), but the literals itself are different and use
   * different memory addresses.
   *
   * @param other a reference to the literal to be cloned.
   */
  Literal(Literal& other) {
    var = other.var;
    negated = other.negated;
  }
  /**
   * Creates a new literal which will contain a freshly created BoolVar whose
   * name is _var.
   *
   * @param _var the name of the var attribute of the new object.
   * @param _negated true if this object is a negative literal, false otherwise.
   */
  Literal(string _var, bool _negated = false) : negated(_negated) {
    var.reset(new BoolVar(_var));
  }

  /**
   * Getter method for the var attribute.
   *
   * @return a pointer to the var attribute.
   */
  BoolVarPtr getVar() { return var; }
  /**
   * Sets the var object to the one passed as parameter.
   *
   * @param _var the new var object for the literal.
   */
  void setVar(BoolVarPtr _var) { var = _var; }

  /**
   * Getter for the negate attribute.
   *
   * @return the negated attribute.
   */
  bool isNegated() { return negated; }
  /**
   * Setter for the newgate attribute.
   *
   * @param _negated the new value for the negated attribute.
   */
  void setNegated(bool _negated) { negated = _negated; }

  /// returns the string representation of this literal
  operator string();
  /// Returns a copy of var object.
  operator BoolVar();
  /// Returns a pointer to the var object.
  operator shared_ptr<BoolVar>();

  /**
   * Copies the values of other into this, such that *this == other but this !=
   * &other.
   *
   * @param other the object to be copied.
   *
   * @return a reference to this object.
   */
  Literal& operator=(const Literal& other);
  /**
   * Copies other and sets the copy as the val attribute and sets the negated
   * attribute to false.
   *
   * @param other The BoolVar to be copied as the new var attribute of this.
   *
   * @return a reference to this object.
   */
  Literal& operator=(const BoolVar& other) {
    *var = other;
    negated = false;
    return *this;
  }
  /**
   * Sets the value passed as a parameter as the new var value and sets the
   * negated attribute to false.
   *
   * @param other The BoolVar used as the new var attribute of this.
   *
   * @return a reference to this object.
   */
  Literal& operator=(const BoolVarPtr& other) {
    var = other;
    negated = false;
    return *this;
  }
  /**
   * Sets the var attribute to a new BoolVar with the name passed as the
   * parameter, also sets the negated value to false.
   *
   * @param other The name of the new BoolVar object to be used as the var
   * attribute.
   *
   * @return a reference to this object.
   */
  Literal& operator=(const string& other) {
    var.reset(new BoolVar(other));
    negated = false;
    return *this;
  }
  /**
   * Sets the var attribute to a new BoolVar with the name passed as the
   * parameter, also sets the negated value to false.
   *
   * @param other The name of the new BoolVar object to be used as the var
   * attribute.
   *
   * @return a reference to this object.
   *
   * @see operator=(const string&other)
   */
  Literal& operator=(const char* other) {
    var.reset(new BoolVar(other));
    negated = false;
    return *this;
  }

  /**
   * Compares two literals and returns true if their attributes have the same
   * values. The var attributes are not compare by the addresses but by their
   * contents.
   *
   * @param other The literal to be compare against
   *
   * @return true if both literals are negated and the two vars have the same
   * name.
   */
  bool operator==(const Literal& other) {
    return (negated == other.negated) && (*var == *other.var);
  }
  /**
   * Compares this literal with a BoolVar, if this literal is positive and the
   * var has the same name as other then they're consider to be equal.
   *
   * @param other The BoolVar to be compared against this literal
   *
   * @return true if this literal is positive and other == *(this->var) false
   * otherwise.
   */
  bool operator==(const BoolVar& other) { return !negated && (*var == other); }
  /**
   * Returns the negation of the equal operator.
   *
   * @param other the other literal to be compared.
   *
   * @return false if *this == other true otherwise
   *
   * @see operator==(const Literal&other)
   */
  bool operator!=(const Literal& other) { return !(*this == other); }
  /**
   * Returns the negation of the equal operator.
   *
   * @param other the other literal to be compared.
   *
   * @return false if *this == other true otherwise
   *
   * @see operator==(const Literal&other)
   */
  bool operator!=(const BoolVar& other) { return !(*this == other); }

  shared_ptr<Literal> getComplement() { return complement; };
  void setComplement(shared_ptr<Literal> com) { complement = com; };
};
/// A pointer to a literal
/// @see Literal
typedef shared_ptr<Literal> LiteralPtr;

/**
 * @brief Representation of a clause.
 *
 * This class represents a logical clause of the form <code>p_1 && .. && p_n ->
 * q</code>. The head is only allowed to be a positive literal while the
 * elements of the body can be positive or negative literals.
 *
 * @author Alexander Rojas
 */
class Clause {
 private:
  BoolVarPtr head; /*< The atom in the head of the clause, only positive atoms
                      are allowed */
  set<LiteralPtr> body;

 public:
  /**
   * Setter of the head attribute.
   *
   * @param _head A pointer to the object that will be used as the head.
   */
  void setHead(BoolVarPtr _head) { head = _head; }
  /**
   * Getter of the head property.
   *
   * @return A pointer to the object in the head.
   */
  BoolVarPtr getHead() { return head; }

  /**
   * Getter of the body attribute.
   *
   * @return A reference to the set containing the literals in the body.
   */
  set<LiteralPtr>& getBody() { return body; }

  /**
   * Cleans the attribute body and puts all the elements of _body into body
   * (they point to the same place in the memory).
   *
   * @param _body A set of pointers to literals which will be used as the
   * elements of the body in this clause.
   */
  void setBody(set<LiteralPtr>& _body);

  /**
   * Checks whether the given atom appears in the body of the clause without
   * taking into account if it is a negated or a positive literal.
   *
   * @param var the atom to be tested for existence in the body of the clause
   */
  bool containsInBody(BoolVar& var) const;

  /**
   * Checks whether the given literal appears in the body of the clause taking
   * into account if it is a negated or a positive literal.
   *
   * @param lit the literal to be checked for existence.
   */
  bool containsInBody(Literal& lit) const;

  /**
   * Add the given atom as a positive literal to the body of the clause. If it
   * does not exists.
   *
   * @param var A pointer to the BoolVar that will be added to the body of the
   * clause.
   *
   * @see addToBody(LiteralPtr lit)
   */
  void addToBody(BoolVarPtr var);
  /**
   * Add the given literal to the body of the clause, if it is not already.
   *
   * @param lit A pointer to a Literal object that will be added to the body of
   * the clause.
   */
  void addToBody(LiteralPtr lit);

  /**
   * Generates a string representation of the Clause. This representation has
   * the form: q:-p_1,...,p_k,¬p_k+1,...,¬p_n. where q is the head of the
   * clause, p_1 to p_k are the positive literals in the clause and p_k+1 to p_n
   * the negative literals.
   */
  operator string();
  /**
   * Checks for the equality of two clauses. Two clauses are said to be equal if
   * the two heads are valued equal and all the literals in the bodies are
   * equal.
   *
   * @warning I don't think this compares onl the values. Now that I think it
   * shouldn't use the pointers at all.
   *
   * @todo This shouldn't use pointer comparison which is indirectly used in the
   * literal comparison. or I have to decide to use pointer comparison in all
   * the elements.
   *
   * @param other The other clause to be compared to.
   *
   * @return true if the clauses are equal.
   */
  bool operator==(const Clause& other) const;
  /**
   * Returns the negation of the equality operator.
   *
   * @param other The other clause to be compared to.
   *
   * @return the negarion of the equality operator.
   *
   * @see operator==(const Clause&other)
   */
  bool operator!=(const Clause& other) const { return !(*this == other); }

  /**
   * Get the pointers to the BoolVar's in the literals of the body of the clause
   * and returns a set with all these elements.
   *
   * @return a set containing pointers to the boolean variables in the literals
   * of the body.
   */
  set<BoolVarPtr> getAllBoolVars();
};
/// A pointer to a clause
/// @see Clause
typedef shared_ptr<Clause> ClausePtr;

/**
 * @brief represents a set of values that cannot be true at the same time. i.e.
 * A constraint
 *
 * An set of variables that cannot be interpreted to true at the same time. In
 * other words, it is constraint over the set of variables.
 */
class Constraint {
 private:
  set<BoolVarPtr> body;

 public:
  /**
   * Getter of the body property.
   *
   * @return A reference to the body of the constraint.
   */
  set<BoolVarPtr>& getBody() { return body; }
  /**
   * The setter of the body attribute. It initializes the body with a copy of
   * the _body parameter, set will contain pointers to the same elements as
   * _body does, but the two objects are different.
   *
   * @param _body a set of boolvar pointers that will be cloned.
   */
  void setBody(set<BoolVarPtr>& _body);

  /**
   * Generates a string representation of a constraint to be used as an
   * automatic string castin. The string representation is very similar to that
   * of the clause class but this has no negative literals nor head.
   *
   * @see Clause#operator string()
   */
  operator string();
  /**
   * Two constraints are considered equal if their bodies contain the same
   * propositional variables ie. each propositional varaible in this an other
   * have an element in the body of the other object which match to equal.
   *
   * @param other The object that will be compared with this.
   *
   * @return true if the bodies of the two objects contains the same elements.
   * false otherwise
   */
  bool operator==(const Constraint& other) const;
  /**
   * Returns the negation of the equality operator.
   *
   * @return the negation of the equality operator
   *
   * @see operator==(const Constraint&other)
   */
  bool operator!=(const Constraint& other) const { return !(*this == other); }

  /**
   * Checks whether the given atom appears in the body of the clause without
   * taking into account if it is a negated or a positive literal.
   *
   * @param var the atom to be tested for existence in the body of the clause
   */
  bool containsInBody(BoolVar& var) const;

  /**
   * Checks whether the given literal appears in the body of the clause taking
   * into account if it is a negated or a positive literal.
   *
   * @param lit the literal to be checked for existence.
   */
  bool containsInBody(Literal& lit) const;
};

/// A pointer to a constraint
typedef shared_ptr<Constraint> ConstraintPtr;

/**
 * @brief Abstract representation of a full propositional abductive logic
 * program.
 *
 * A program is just a meta structure that contains all the elements of a
 * propositional abductive logic program. As such, it contains a set of
 * abductibles, observations, clauses, and constraints. It contains all the
 * necessary information to recreate any algorithm that works over abductible
 * logic programs.
 *
 * @author Alexander Rojas
 */
class Program {
 private:
  set<ClausePtr> clauses;
  set<BoolVarPtr> obsers;
  set<BoolVarPtr> abducts;
  set<ConstraintPtr> consts;

 public:
  Program();
  virtual ~Program();

  /**
   * Getter method of the clauses attribute.
   *
   * @return A reference to the clauses attribute.
   */
  set<ClausePtr>& getClauses() { return clauses; }
  /**
   * Setter method for the clauses attribute.
   *
   * @param _clauses A set of clauses to be cloned.
   */
  void setClauses(set<ClausePtr>& _clauses);

  /**
   * Getter method of the obsers attribute.
   *
   * @return A reference to the clauses obsers.
   */
  set<BoolVarPtr>& getObsers() { return obsers; }
  /**
   * Setter method for the obsers attribute.
   *
   * @param _obsers A set of obsers to be cloned.
   */
  void setObsers(set<BoolVarPtr>& _obsers);

  /**
   * Getter method of the abducts attribute.
   *
   * @return A reference to the clauses abducts.
   */
  set<BoolVarPtr>& getAbducts() { return abducts; }
  /**
   * Setter method for the abducts attribute.
   *
   * @param _abducts A set of abducts to be cloned.
   */
  void setAbducst(set<BoolVarPtr>& _abducts);

  /**
   * Getter method of the consts attribute.
   *
   * @return A reference to the clauses consts.
   */
  set<ConstraintPtr>& getConstraints() { return consts; }
  /**
   * Setter method for the consts attribute.
   *
   * @param _consts A set of consts to be cloned.
   */
  void setConstraints(set<ConstraintPtr>& _consts);

  /**
   * Generates the string representation of the program which can be parsed by
   * the default parser.
   */
  operator string();

  /**
   * Return all the different atoms declared in a program. i.e. All the
   * propositional boolean variables used in the bodies and heads of the
   * clauses, the abductibles and observations sets and constraint set.
   *
   * @return a set with pointers to all the atoms used by a program.
   */
  set<BoolVarPtr> allPropositionalVariables();
  /**
   * Return a set containing all the atoms used in the clauses of a program.
   * Useful to check the well definition of a program, since the set of
   * observations need to be a subset of the returned set or the problem won't
   * have solution.
   *
   * @return a set with pointers to all the atoms declared in the clauses set of
   * the program.
   */
  set<BoolVarPtr> clausesPropositionalVariables();
  /**
   * Return a set containing all the atoms used in the constrains of a program.
   * Useful to check the well definition of a program, since the set of
   * observations need to be a subset of the returned set or the problem won't
   * have solution.
   *
   * @return a set with pointers to all the atoms declared in the clauses set of
   * the program.
   */
  set<BoolVarPtr> constrainsPropositionalVariables();

  /**
   * When no abductibles are passed in the program file, they can still be
   * inferred. To do so, the program uses as abductibles all the propositional
   * variables that appear in the body of clauses as positive or negative
   * literals, but that do not appear as heads of any clause.
   *
   * @see fillObsers()
   */
  void fillAbducts();
  /**
   * When no observations are passed in the program file, they can still be
   * infered. To do so, the program uses as observations (or goals) all the
   * propositional variables that appear as heads of clauses but that do not
   * appear in the body of any clause, such that these are a consequence of
   * other propositional variables.
   *
   * @see fillAbducts()
   */
  void fillObsers();
};

/// A pointer to a Program
/// @see Program
typedef shared_ptr<Program> ProgramPtr;

}  // namespace logicStructs

}  // namespace nalso

#endif /* PROGRAM_H_ */
