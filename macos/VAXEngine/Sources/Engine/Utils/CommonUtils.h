//
// Created by Alexander Lakhonin on 21.03.2025.
//

#ifndef CommonUtils_h
#define CommonUtils_h

template <typename Enumeration>
auto as_integer(Enumeration const value) -> typename std::underlying_type<Enumeration>::type
{
  return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}

#endif /* CommonUtils_h */
