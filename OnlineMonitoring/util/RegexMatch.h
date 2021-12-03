#ifndef OM_REGEXMATCH_H
#define OM_REGEXMATCH_H
#include <string>

namespace om {
  /// Does the string s match the pattern p?
  /// \returns 0 = match, 1 = no match
  int regex_match(const std::string& s, const std::string& p);
}

#endif
