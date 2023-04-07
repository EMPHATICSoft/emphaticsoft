#include "OnlineMonitoring/util/RegexMatch.h"
#include <cstdlib>
//extern "C" {
#include <regex.h>
//}

int emph::onmon::regex_match(const std::string& s, const std::string& w) 
{
  // Change the wild card pattern into a regex
  std::string p = "^";
  for (unsigned int i=0; i<w.size(); ++i) {
    if      (w[i]=='*') p += ".*";
    else if (w[i]=='.') p += "\\.";
    else                p += w[i];
  }
  p += '$';
  
  //
  // Compose the regular expression structure from the pattern string
  //
  regex_t re;
  int ierr = regcomp(&re, p.c_str(), REG_EXTENDED|REG_NOSUB);
  if (ierr!=0) abort();
  
  //
  // Test s against the pattern. 0 = match, 1 = no match
  //
  int stat = regexec(&re, s.c_str(), 0, 0, 0);
  
  regfree(&re);
  
  return stat;
}
////////////////////////////////////////////////////////////////////////

