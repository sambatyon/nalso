/**
 * @file splparser.h
 *
 * @brief declares the parser for the small prolog.
 *
 * Contains the definition for the Small Prolog Parser class.
 *
 *  @date Oct 9, 2009
 *  @author Alexander Rojas <alexander.rojas@gmail.com>
 */

#pragma once

#include <iostream>

#include "nalso/parsers/alparser.hh"

namespace nalso {

namespace parsers {
/**
 * @brief small prolog parser class.
 *
 * This class provides a way to parse abductive logic programs which are written
 * in the small prolog language. This language is a specific purpose language
 * that derives many of its characteristics from prolog but setting aside the
 * first order nature of prolog. It also provides an special syntax to represent
 * abductibles sets.
 *
 * The grammar of the language is the following:
 *
 * program := expr+
 *
 * expr := fact | clause | abduct | constraint
 *
 * @author Alexander Rojas
 *
 * @todo Finish this documentation.
 */
class SmallPrologParser : AbductLogicProgramParser {
 public:
  SmallPrologParser(std::istream& _source = std::cin)
      : AbductLogicProgramParser(_source) {};
  virtual ~SmallPrologParser() {};

  /**
   * Performs the actual parsing of an small prolog program. It uses
   * boost::spirit to generate the representation.
   *
   * @return A pointer to a Program object which contains the representation of
   * the data stored in source.
   *
   * @see AbductLogicProgramParser#parse()
   */
  logic::ProgramPtr parseProgram();
};

}  // namespace parsers
}  // namespace nalso
