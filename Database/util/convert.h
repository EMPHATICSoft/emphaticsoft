#ifndef cetlib_sqlite_detail_convert_h
#define cetlib_sqlite_detail_convert_h

// =================================================================
//
// For extraction operations, the following conversions are supported:
//
// .. std::string ==> std::string
//
//   (as specializations)
//
// .. std::string ==> int
// .. std::string ==> long
// .. std::string ==> long long
// .. std::string ==> unsigned
// .. std::string ==> unsigned long
// .. std::string ==> unsigned long long
// .. std::string ==> float
// .. std::string ==> double
// .. std::string ==> long double
//
// =================================================================

#include <string>

namespace convert {

  template <typename T>
  inline auto
  convertTo(std::string const& arg)
  {
    return arg;
  }

  template <>
  inline auto
  convertTo<int>(std::string const& arg)
  {
    return std::stoi(arg);
  }
  template <>
  inline auto
  convertTo<long>(std::string const& arg)
  {
    return std::stol(arg);
  }
  template <>
  inline auto
  convertTo<long long>(std::string const& arg)
  {
    return std::stoll(arg);
  }
  template <>
  inline auto
  convertTo<unsigned>(std::string const& arg)
  {
    return std::stoul(arg);
  }
  template <>
  inline auto
  convertTo<unsigned long>(std::string const& arg)
  {
    return std::stoul(arg);
  }
  template <>
  inline auto
  convertTo<unsigned long long>(std::string const& arg)
  {
    return std::stoull(arg);
  }
  template <>
  inline auto
  convertTo<float>(std::string const& arg)
  {
    return std::stof(arg);
  }
  template <>
  inline auto
  convertTo<double>(std::string const& arg)
  {
    return std::stod(arg);
  }
  template <>
  inline auto
  convertTo<long double>(std::string const& arg)
  {
    return std::stold(arg);
  }
} //convert

#endif /* cetlib_sqlite_detail_convert_h */

// Local Variables:
// mode: c++
// End:
