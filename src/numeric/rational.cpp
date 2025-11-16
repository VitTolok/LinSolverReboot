#include "rational.hpp"
#include <cstring>
#include <numeric>
#include <sstream>
#include <cmath>

std::ostream &operator<<(std::ostream &out, const Rational &number) {
  if (number == 0) {
    return out << 0;
  }
  if (number.num_.Abs() <= 1'000'000 && number.den_ <= 1'000'000) {
    std::string numer = (std::ostringstream() << number.num_).str();
    if (number.den_ == 1) {
      return out << numer;
    }
    std::string denom = (std::ostringstream() << number.den_).str();
    if (numer.size() + denom.size() <= 6) {
      return out << numer << '/' << denom;
    }
  }
  auto approx = number.GetApproximation();
  if (std::abs(approx.power) <= 2) {
    return out << (approx.multiplier * powl(10, approx.power));
  }
  return out << approx.multiplier << 'e' << approx.power;
}

std::istream &operator>>(std::istream &in, Rational &number) try {
  std::string input;
  in >> input;
  size_t sep = input.find('/');
  if (sep < input.size()) {
    std::istringstream(input.substr(0, sep)) >> number.num_;
    std::istringstream(input.substr(sep + 1, input.size())) >> number.den_;
    number.Normalize();
    return in;
  }
  size_t dot = input.find('.');
  size_t exp = input.find('e');
  int64_t mult = 0;
  if (exp < input.size()) {
    if (exp < input.size() - 1) {
      std::istringstream(input.c_str() + exp + 1) >> mult;
    }
    input.resize(exp);
  }
  if (dot < input.size()) {
    mult -= input.size() - dot - 1;
    input.erase(dot, 1);
  }
  std::istringstream(input) >> number.num_;
  if (mult > 0) {
    number.num_ *= Power(UBigInt(10), mult);
    number.den_ = 1;
  } else if (mult == 0) {
    number.den_ = 1;
  } else {
    number.den_ = Power(UBigInt(10), -mult);
    number.Normalize();
  }
  return in;
} catch (...) {
  throw InvalidRationalString();
}
