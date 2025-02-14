/**
 * @file alparser.h
 *
 * @brief Abstract Abductive Logic programming parser
 *
 * Provides a unified interface for all the objects that want to parce abductive
 * logic programs from some kind of file.
 *
 * @date Oct 9, 2009
 * @author Alexander Rojas <alexander.rojas@gmail.com>
 */

#pragma once

#include <iostream>
#include <string>

#include "nalso/logicstructs/program.hh"
#include "nalso/utils/utils.hh"

namespace nalso {
/**
 * @brief container of the parses that generate Program objects from istreams
 *
 * All parsers that attempt to generate a Program object from an input stream
 * should be placed in this namespace and inherit from the pure virtual class
 * AbductLogicProgramParser
 */
namespace parsers {
namespace logStr = logicStructs;
/**
 * @brief Abstract class which works as parent of all classes that intend to
 * parse A.L. programs.
 *
 * This class is a meta class for all programs that intend to generate a Program
 * object from a file or any input stream. Examples are small prolog, prolog or
 * xml.
 *
 * @author Alexander Rojas
 */
class AbductLogicProgramParser {
 protected:
  std::istream &source;
  /**
   * Checks whether the program is consistent. For a program to be consistent
   * the observations and abdcutibles sets need to be disjunct and the
   * observations set to be a subset of the clauses variables set.
   *
   * @param[in] pr The program to be checked for consistency.
   *
   * @param[out] message A string that includes additional information about the
   * cause of the inconsistency if the program happens to be inconsistent.
   *
   * @return true if the program is inconsitent, false otherwise.
   */
  static bool check(logStr::Program &pr, std::string &message);

 public:
  /**
   * Sets the input stream that contains the data to be parsed.
   *
   * @param _source The input stream containing the data to be parsed.
   */
  AbductLogicProgramParser(std::istream &_source = std::cin) : source(_source) {}
  virtual ~AbductLogicProgramParser() {}

  /**
   * Virtual function that performs the actual parsing. It should be override by
   * all subclasses of AbductLogicProgramParser.
   *
   * @return A pointer to a Program object that represents the parsed stream.
   */
  virtual logStr::ProgramPtr parseProgram() = 0;
};

}  // namespace parsers
}  // namespace nalso
