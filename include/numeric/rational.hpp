#pragma once
#include "definitions.hpp"
#include <cstdint>
#include <iostream>
#include <compare>

class InvalidRational : public MachineError {
 private:
  std::string message_ = "Denominator can't be 0";
};

class InvalidRationalString : public MachineError {
 private:
  std::string message_ = "Invalid Rational String";
};

class Rational {
 public:
  const int64_t &Num() const;
  const uint64_t &Den() const;

  friend Rational &operator+=(Rational &, const Rational &);
  friend Rational &operator-=(Rational &, const Rational &);
  friend Rational &operator*=(Rational &, const Rational &);
  friend Rational &operator/=(Rational &, const Rational &);

  friend Rational operator+(const Rational &, const Rational &);
  friend Rational operator-(const Rational &, const Rational &);
  friend Rational operator-(const Rational &);
  friend Rational operator*(const Rational &, const Rational &);
  friend Rational operator/(const Rational &, const Rational &);

  Rational();
  explicit Rational(int64_t);
  Rational(int64_t, int64_t);
  Rational(int64_t, uint64_t);

  friend bool operator<(const Rational &, const Rational &);
  friend bool operator>(const Rational &, const Rational &);
  friend bool operator<=(const Rational &, const Rational &);
  friend bool operator>=(const Rational &, const Rational &);
  friend bool operator==(const Rational &, const Rational &);
  friend bool operator!=(const Rational &, const Rational &);

  friend std::ostream &operator<<(std::ostream &, const Rational &);
  friend std::istream &operator>>(std::istream &, Rational &);

 private:
  int64_t num_;
  uint64_t den_;
  void Normalize();
};
