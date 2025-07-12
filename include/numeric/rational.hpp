#pragma once
#include "BigInt.hpp"
#include "definitions.hpp"
#include <cstdint>
#include <iostream>

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
  const BigInt &Num() const;
  const UBigInt &Den() const;

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
  explicit Rational(const BigInt &);
  explicit Rational(BigInt &&);
  Rational(const BigInt &, const BigInt &);
  Rational(const BigInt &, const UBigInt &);

  friend bool operator<(const Rational &, const Rational &);
  friend bool operator>(const Rational &, const Rational &);
  friend bool operator<=(const Rational &, const Rational &);
  friend bool operator>=(const Rational &, const Rational &);
  friend bool operator==(const Rational &, const Rational &);
  friend bool operator!=(const Rational &, const Rational &);

  friend std::ostream &operator<<(std::ostream &, const Rational &);
  friend std::istream &operator>>(std::istream &, Rational &);

private:
  BigInt num_;
  UBigInt den_;
  void Normalize();
};
