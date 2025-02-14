/*
 * @file utils.cpp
 *
 * @date Nov 5, 2009
 * @author arojasp
 */
#include "utils.h"

namespace nalso {

namespace utils {
string removeMinus(string str) {
  using namespace boost;
  const regex s("(.*)-(.*)");
  while (regex_match(str, s))
    str = regex_replace(str, s, "\\1_\\2",
                        boost::match_default | boost::format_sed);
  return str;
}
}  // namespace utils
}  // namespace nalso
