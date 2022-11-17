#ifndef EnumToString_h
#define EnumToString_h
//
// Template used to instantiate enum-matched-to-string classes in which the enum
// is allowed required to have continuous values that start at 0.  For enums in
// which the enum values do not satisfy this requirement, one may use the
// template EnumToStringSparse.h, which allows enum values that do not start at
// zero and which may have gaps in the sequence.
//
// The user must supply a Detail class with the following requirements:
//
//   1) The detail class must contain an enum named enum_type.
//
//   2) The enum_type must contain two named identifiers:
//         unknown  - must have a value of zero.  See note 1, below.
//         lastEnum - must have a value one more than the last defined value.
//
//      For example:
//       enum enum_type { unknown, red, green, blue, lastEnum }
//
//   3) The class must contain two static functions:
//       static std::string const& typeName();
//       static std::vector<std::string> const& names();
//
//      The first function returns a string that holds the name of the type.
//      The template uses this function to decorate some printed output.
//      The suggested practice is as follows:  if the type ColorId is defined
//      as:
//        typedef EnumToStringSparse<ColorIdDetail> ColorId;
//      then the method ColorIdDetail::typename should return the string
//      "ColorId".
//
//      The second function returns a std::vector that implements the cross
//      reference between the enum_type and the string representations of the
//      enum value.
//
// Notes:
//
// 1) The ROOT IO system and several useful STL container types require that the
// class have a
//    default c'tor.  Therefore we require the enum_type to have a value named
//    Detail::unknown . Default constructed objects have this value.
//
// 2) By design there is no operator<( EnumToStringSparse const& ) since that
// would be ambiguous for comparison
//    by id or by name. Instead there are two free functions, lessById and
//    lessByName.
//
// 3) There are two notions of validity.
//      a) The value is defined by the enum type and the value Detail::unknown
//      is considered valid. b) The value is defined by the enum type but the
//      value Detail::unknown is NOT considered valid.
//    Several functions have a second argument that permits the user to choose
//    between these two notions. The default behaviour is the value
//    Detail::unknown is NOT considered valid.
//

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace emph {

  template <class Detail>
  class EnumToString : public Detail {
  public:
    typedef typename Detail::enum_type enum_type;
    typedef std::vector<std::string> names_type;

    explicit EnumToString() : _id(Detail::unknown) {}

    explicit EnumToString(enum_type id) : _id(id) {}

    explicit EnumToString(int id, bool unknownIsError = true)
      : _id(static_cast<enum_type>(id))
    {
      isValidOrThrow(id, unknownIsError);
    }

    explicit EnumToString(std::string const& name,
                          bool throwIfUnknown = true,
                          bool throwIfUndefined = true)
      : _id(findByName(name, throwIfUnknown, throwIfUndefined).id())
    {}

    // Accept compiler generated d'tor, copy c'tor and copy assignment.

    enum_type
    id() const
    {
      return _id;
    }

    std::string const&
    name() const
    {
      return Detail::names()[_id];
    }

    // This one apparently does not work.
    EnumToString&
    operator=(enum_type const& c)
    {
      _id = c;
      return *this;
    }

    // Do I really want these next two conversion operators?
    operator enum_type() const { return _id; }

    operator std::string const&() const { return name(); }

    // Comparisons.  See note 2.
    bool
    operator==(const EnumToString g) const
    {
      return (_id == g._id);
    }

    bool
    operator==(const enum_type g) const
    {
      return (_id == g);
    }

    static bool
    isValid(int id, bool unknownIsError = true)
    {
      if (id < Detail::unknown)
        return false;
      if (id >= Detail::lastEnum)
        return false;
      if (id == Detail::unknown && unknownIsError)
        return false;
      return true;
    }

    static bool
    isValidOrThrow(int id, bool unknownIsError = true)
    {
      if (!isValid(id, unknownIsError)) {
        std::ostringstream os;
        os << Detail::typeName() << " invalid enum value : " << id;
        throw std::out_of_range(os.str());
        return false;
      }
      return true;
    }

    static EnumToString
    findByName(std::string const& name,
               bool throwIfUnknown = true,
               bool throwIfUndefined = true)
    {

      // Find the string in the list of names.
      names_type const& n = Detail::names();
      names_type::const_iterator i = std::find(n.begin(), n.end(), name);
      if (i != n.end()) {
        int index = i - n.begin();
        if (index == Detail::unknown) {
          std::ostringstream os;
          os << Detail::typeName() << "::unknown is not allowed at this time";
          throw std::out_of_range(os.str());
        }
        return EnumToString(index);
      }

      // Did not find it;
      if (throwIfUndefined) {
        std::ostringstream os;
        os << Detail::typeName() << " invalid enum name : " << name;
        throw std::out_of_range(os.str());
      }

      return EnumToString();
    }

    // The number of names, including "unknown".
    static size_t
    size()
    {
      return Detail::names().size();
    }

    // Raw access to the names of the enums.
    //     "unknown" is present but "lastEnum" is not.
    static names_type const&
    names()
    {
      return Detail::names();
    }

    // Raw access to the names of the enums, with the "unknown" stripped out.
    static names_type const&
    knownNames()
    {
      static names_type known;
      if (known.empty()) {
        known.insert(++names().begin(), names().end());
      }
      return known;
    }

    static void
    printAll(std::ostream& ost)
    {
      ost << "List of defined enum values for " << Detail::typeName()
          << std::endl;
      for (int i = 0; i < Detail::lastEnum; ++i) {
        ost << std::setw(3) << i << " " << names().at(i) << std::endl;
      }
    }

  private:
    enum_type _id;
  };

  template <class Detail>
  std::ostream&
  operator<<(std::ostream& ost, const EnumToString<Detail>& id)
  {
    ost << "( " << id.id() << ": " << id.name() << " )";
    return ost;
  }

}

#endif /* DataProducts_EnumToString_h */
