/**
 * @file utils.h
 *
 * @brief Utilities proxy header.
 *
 * works as a proxy header for utilities classes as well as define short useful
 * functions which are no project specific and can be used by other programs.
 *
 * @date Sep 30, 2009
 * @author Alexander Rojas <alexander.rojas@gmail.com>
 */

#pragma once

#include <memory>
#include <set>

namespace nalso {

/**
 * @brief utilities container namespace.
 *
 * Contains all the functions and classes that are not specific to the project.
 */
namespace utils {

/**
 * Checks whether an object is contained in a set comparing it by value.
 *
 * @param list_ The set to be checked.
 *
 * @param elem The object to be checked.
 *
 * @param it An iterator of type set<shared_ptr<T> >::iterator used to go
 * through the set.
 *
 * @return true if the set list_ contains an element that computes to equal (by
 * value) the the one passed as a reference, false otherwise.
 */
template <typename T, class InputIterator>
bool isInSet(std::set<std::shared_ptr<T> > &list_, T &elem, InputIterator it) {
  it = list_.begin();
  while (it != list_.end() && elem != **it) it++;
  return it == list_.end() ? false : true;
}

std::string removeMinus(std::string str);

}  // namespace utils
}  // namespace nalso
