/*
 * @file utils.cpp
 *
 * @date Nov 5, 2009
 * @author arojasp
 */
#include "utils.hh"

#include <regex>

namespace nalso {
namespace utils {
std::string removeMinus(std::string str) {
  const std::regex s("(.*)-(.*)");
  while (std::regex_match(str, s)) {
    str = std::regex_replace(
      str, s, "\\1_\\2",
      std::regex_constants::match_default | std::regex_constants::format_sed
    );
  }
  return str;
}
}  // namespace utils
}  // namespace nalso
