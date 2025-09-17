#pragma once
#include "BigInt.hpp"
#include "definitions.hpp"
#include "BigInt.hpp"
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

class Rational;

template <class Integer>
concept SomeInt = std::integral<std::decay_t<Integer>> || std::same_as<UBigInt, std::decay_t<Integer>> ||
                  std::same_as<BigInt, std::decay_t<Integer>>;
template <class Integer>
concept SomeBigInt = std::same_as<UBigInt, std::decay_t<Integer>> || std::same_as<BigInt, std::decay_t<Integer>>;
template <class ORational>
concept OnlyRational = std::same_as<Rational, std::decay_t<ORational>>;

class Rational {
 public:
  Rational() : num_(), den_(1) {
  }
  Rational(const Rational &) = default;
  Rational(Rational &&) = default;
  template <SomeInt Integer>
  explicit inline Rational(Integer &&);
  template <SomeInt Numerator, SomeInt Denominator>
  inline Rational(Numerator &&, Denominator &&);

  Rational &operator=(const Rational &) = default;
  Rational &operator=(Rational &&) = default;
  template <SomeInt Integer>
  Rational &operator=(Integer &&);

  const BigInt &Num() const;
  const UBigInt &Den() const;

  friend Rational &operator+=(Rational &, const Rational &);
  friend Rational &operator-=(Rational &, const Rational &);
  friend Rational &operator*=(Rational &, const Rational &);
  friend Rational &operator/=(Rational &, const Rational &);
  friend Rational &operator%=(Rational &, const Rational &);

  friend Rational &operator+=(Rational &, Rational &&);
  friend Rational &operator-=(Rational &, Rational &&);

  template <SomeInt Integer>
  friend inline Rational &operator+=(Rational &, Integer &&);
  template <SomeInt Integer>
  friend inline Rational &operator-=(Rational &, Integer &&);
  template <SomeInt Integer>
  friend inline Rational &operator*=(Rational &, Integer &&);
  template <SomeInt Integer>
  friend inline Rational &operator/=(Rational &, Integer &&);
  template <SomeInt Integer>
  friend inline Rational &operator%=(Rational &, Integer &&);

  friend inline Rational operator-(const Rational &);
  friend inline Rational operator-(Rational &&);

  template <OnlyRational ORationalA, OnlyRational ORationalB>
  friend inline Rational operator+(ORationalA &&, ORationalB &&);
  template <OnlyRational ORationalA, OnlyRational ORationalB>
  friend inline Rational operator-(ORationalA &&, ORationalB &&);
  template <OnlyRational ORationalA, OnlyRational ORationalB>
  friend inline Rational operator*(ORationalA &&, ORationalB &&);
  template <OnlyRational ORationalA, OnlyRational ORationalB>
  friend inline Rational operator/(ORationalA &&, ORationalB &&);
  template <OnlyRational ORationalA, OnlyRational ORationalB>
  friend inline Rational operator%(ORationalA &&, ORationalB &&);

  template <OnlyRational ORational, SomeInt Integer>
  friend inline Rational operator+(ORational &&, Integer &&);
  template <OnlyRational ORational, SomeInt Integer>
  friend inline Rational operator-(ORational &&, Integer &&);
  template <OnlyRational ORational, SomeInt Integer>
  friend inline Rational operator*(ORational &&, Integer &&);
  template <OnlyRational ORational, SomeInt Integer>
  friend inline Rational operator/(ORational &&, Integer &&);
  template <OnlyRational ORational, SomeInt Integer>
  friend inline Rational operator%(ORational &&, Integer &&);

  template <OnlyRational ORational, SomeInt Integer>
  friend inline Rational operator+(Integer &&, ORational &&);
  template <OnlyRational ORational, SomeInt Integer>
  friend inline Rational operator-(Integer &&, ORational &&);
  template <OnlyRational ORational, SomeInt Integer>
  friend inline Rational operator*(Integer &&, ORational &&);
  template <OnlyRational ORational, SomeInt Integer>
  friend inline Rational operator/(Integer &&, ORational &&);
  template <OnlyRational ORational, SomeInt Integer>
  friend inline Rational operator%(Integer &&, ORational &&);

  template <OnlyRational ORationalA, OnlyRational ORationalB>
  friend inline bool operator<(ORationalA &&, ORationalB &&);
  template <OnlyRational ORationalA, OnlyRational ORationalB>
  friend inline bool operator>(ORationalA &&, ORationalB &&);
  template <OnlyRational ORationalA, OnlyRational ORationalB>
  friend inline bool operator<=(ORationalA &&, ORationalB &&);
  template <OnlyRational ORationalA, OnlyRational ORationalB>
  friend inline bool operator>=(ORationalA &&, ORationalB &&);
  template <OnlyRational ORationalA, OnlyRational ORationalB>
  friend inline bool operator==(ORationalA &&, ORationalB &&);
  template <OnlyRational ORationalA, OnlyRational ORationalB>
  friend inline bool operator!=(ORationalA &&, ORationalB &&);

  template <OnlyRational ORational, SomeInt Integer>
  friend inline bool operator<(ORational &&, Integer &&);
  template <OnlyRational ORational, SomeInt Integer>
  friend inline bool operator>(ORational &&, Integer &&);
  template <OnlyRational ORational, SomeInt Integer>
  friend inline bool operator<=(ORational &&, Integer &&);
  template <OnlyRational ORational, SomeInt Integer>
  friend inline bool operator>=(ORational &&, Integer &&);
  template <OnlyRational ORational, SomeInt Integer>
  friend inline bool operator==(ORational &&, Integer &&);
  template <OnlyRational ORational, SomeInt Integer>
  friend inline bool operator!=(ORational &&, Integer &&);

  template <OnlyRational ORational, SomeInt Integer>
  friend inline bool operator<(Integer &&, ORational &&);
  template <OnlyRational ORational, SomeInt Integer>
  friend inline bool operator>(Integer &&, ORational &&);
  template <OnlyRational ORational, SomeInt Integer>
  friend inline bool operator<=(Integer &&, ORational &&);
  template <OnlyRational ORational, SomeInt Integer>
  friend inline bool operator>=(Integer &&, ORational &&);
  template <OnlyRational ORational, SomeInt Integer>
  friend inline bool operator==(Integer &&, ORational &&);
  template <OnlyRational ORational, SomeInt Integer>
  friend inline bool operator!=(Integer &&, ORational &&);

  friend std::ostream &operator<<(std::ostream &, const Rational &);
  friend std::istream &operator>>(std::istream &, Rational &);

  explicit operator BigInt() const;
  explicit operator UBigInt() const;

  Rational &Floor();

  using Approximation = BigInt::Approximation;
  Approximation GetApproximation() const;

 private:
  BigInt num_;
  UBigInt den_;
  void Normalize();
};

template <SomeInt Integer>
inline Rational::Rational(Integer &&integer) : num_(std::forward<Integer>(integer)), den_(1) {
}
template <SomeInt Numerator, SomeInt Denominator>
inline Rational::Rational(Numerator &&num, Denominator &&den) : num_(), den_() {
  if (den < 0) {
    (num_ = std::forward<Numerator>(num)) *= -1;
    den_ = (std::forward<Denominator>(den) * -1);
  }
  num_ = std::forward<Numerator>(num);
  den_ = std::forward<Denominator>(den);
  Normalize();
}

template <SomeInt Integer>
inline Rational &Rational::operator=(Integer &&integer) {
  num_ = std::forward<Integer>(integer);
  den_ = 1;
  return *this;
}

template <SomeInt Integer>
inline Rational &operator+=(Rational &rational, Integer &&integer) {
  rational.num_ += (std::forward<Integer>(integer) * rational.den_);
  return rational;
}
template <SomeInt Integer>
inline Rational &operator-=(Rational &rational, Integer &&integer) {
  rational.num_ -= (std::forward<Integer>(integer) * rational.den_);
  return rational;
}
template <SomeInt Integer>
inline Rational &operator*=(Rational &rational, Integer &&integer) {
  rational.num_ *= std::forward<Integer>(integer);
  rational.Normalize();
  return rational;
}
template <SomeInt Integer>
inline Rational &operator/=(Rational &rational, Integer &&integer) {
  rational.den_ *= std::forward<Integer>(integer);
  rational.Normalize();
  return rational;
}
template <SomeInt Integer>
inline Rational &operator%=(Rational &rational, Integer &&integer) {
  rational.num_ -= (static_cast<BigInt>(rational) / integer) * std::forward<Integer>(integer);
  rational.Normalize();
  return rational;
}

inline Rational operator-(const Rational &rational) {
  Rational copy(rational);
  return -std::move(copy);
}
inline Rational operator-(Rational &&rational) {
  rational.num_ = -std::move(rational.num_);
  return std::move(rational);
}

template <OnlyRational ORationalA, OnlyRational ORationalB>
inline Rational operator+(ORationalA &&first, ORationalB &&second) {
  if constexpr (std::is_rvalue_reference_v<ORationalA &&>) {
    return std::move(first += second);
  } else if constexpr (std::is_rvalue_reference_v<ORationalB &&>) {
    return std::move(second += first);
  } else {
    Rational copy(first);
    return std::move(copy += second);
  }
}
template <OnlyRational ORationalA, OnlyRational ORationalB>
inline Rational operator-(ORationalA &&first, ORationalB &&second) {
  if constexpr (std::is_rvalue_reference_v<ORationalA &&>) {
    return std::move(first -= second);
  } else if constexpr (std::is_rvalue_reference_v<ORationalB &&>) {
    return std::move((second -= first) *= -1);
  } else {
    Rational copy(first);
    return std::move(copy -= second);
  }
}
template <OnlyRational ORationalA, OnlyRational ORationalB>
inline Rational operator*(ORationalA &&first, ORationalB &&second) {
  if constexpr (std::is_rvalue_reference_v<ORationalA &&>) {
    return std::move(first *= second);
  } else if constexpr (std::is_rvalue_reference_v<ORationalB &&>) {
    return std::move(second *= first);
  } else {
    Rational copy(first);
    return std::move(copy *= second);
  }
}
template <OnlyRational ORationalA, OnlyRational ORationalB>
inline Rational operator/(ORationalA &&first, ORationalB &&second) {
  if constexpr (std::is_rvalue_reference_v<ORationalA &&>) {
    return std::move(first /= second);
  } else if constexpr (std::is_rvalue_reference_v<ORationalB &&>) {
    return std::move(1 / std::move(second /= first));
  } else {
    Rational copy(first);
    return std::move(copy /= second);
  }
}
template <OnlyRational ORationalA, OnlyRational ORationalB>
inline Rational operator%(ORationalA &&first, ORationalB &&second) {
  if constexpr (std::is_rvalue_reference_v<ORationalA &&>) {
    return std::move(first %= second);
  } else {
    Rational copy(first);
    return std::move(copy %= second);
  }
}

template <OnlyRational ORational, SomeInt Integer>
inline Rational operator+(ORational &&rational, Integer &&integer) {
  if constexpr (std::is_rvalue_reference<ORational>(rational)) {
    return std::move(rational += integer);
  } else if constexpr (std::is_rvalue_reference<Integer>(integer)) {
    return std::move(rational + Rational(std::move(integer)));
  } else {
    Rational copy(rational);
    return std::move(copy += integer);
  }
}
template <OnlyRational ORational, SomeInt Integer>
inline Rational operator-(ORational &&rational, Integer &&integer) {
  if constexpr (std::is_rvalue_reference<ORational>(rational)) {
    return std::move(rational -= integer);
  } else if constexpr (std::is_rvalue_reference<Integer>(integer)) {
    return std::move(rational - Rational(std::move(integer)));
  } else {
    Rational copy(rational);
    return std::move(copy -= integer);
  }
}
template <OnlyRational ORational, SomeInt Integer>
inline Rational operator*(ORational &&rational, Integer &&integer) {
  if constexpr (std::is_rvalue_reference<ORational>(rational)) {
    return std::move(rational *= integer);
  } else if constexpr (std::is_rvalue_reference<Integer>(integer)) {
    return std::move(rational * Rational(std::move(integer)));
  } else {
    Rational copy(rational);
    return std::move(copy *= integer);
  }
}
template <OnlyRational ORational, SomeInt Integer>
inline Rational operator/(ORational &&rational, Integer &&integer) {
  if constexpr (std::is_rvalue_reference<ORational>(rational)) {
    return std::move(rational /= integer);
  } else if constexpr (std::is_rvalue_reference<Integer>(integer)) {
    return std::move(rational / Rational(std::move(integer)));
  } else {
    Rational copy(rational);
    return std::move(copy /= integer);
  }
}
template <OnlyRational ORational, SomeInt Integer>
inline Rational operator%(ORational &&rational, Integer &&integer) {
  if constexpr (std::is_rvalue_reference<ORational>(rational)) {
    return std::move(rational %= integer);
  } else if constexpr (std::is_rvalue_reference<Integer>(integer)) {
    return std::move(rational % Rational(std::move(integer)));
  } else {
    Rational copy(rational);
    return std::move(copy %= integer);
  }
}

template <OnlyRational ORational, SomeInt Integer>
inline Rational operator+(Integer &&integer, ORational &&rational) {
  return std::forward<ORational>(rational) + std::forward<Integer>(integer);
}
template <OnlyRational ORational, SomeInt Integer>
inline Rational operator-(Integer &&integer, ORational &&rational) {
  return (std::forward<ORational>(rational) - std::forward<Integer>(integer)) * -1;
}
template <OnlyRational ORational, SomeInt Integer>
inline Rational operator*(Integer &&integer, ORational &&rational) {
  return std::forward<ORational>(rational) * std::forward<Integer>(integer);
}
template <OnlyRational ORational, SomeInt Integer>
inline Rational operator/(Integer &&integer, ORational &&rational) {
  Rational inverse = std::forward<ORational>(rational) / std::forward<Integer>(integer);
  std::swap(const_cast<UBigInt &>(rational.num_.Abs()), inverse.den_);
  return inverse;
}
template <OnlyRational ORational, SomeInt Integer>
inline Rational operator%(Integer &&integer, ORational &&rational) {
  return std::forward<Integer>(integer) - (integer / rational).Round() * std::forward<ORational>(rational);
}

template <OnlyRational ORationalA, OnlyRational ORationalB>
inline bool operator<(ORationalA &&first, ORationalB &&second) {
  return std::forward<ORationalA>(first).num_ * std::forward<ORationalB>(second).den_ <
         std::forward<ORationalB>(second).num_ * std::forward<ORationalA>(first).den_;
}
template <OnlyRational ORationalA, OnlyRational ORationalB>
inline bool operator>(ORationalA &&first, ORationalB &&second) {
  return std::forward<ORationalB>(second) < std::forward<ORationalA>(first);
}
template <OnlyRational ORationalA, OnlyRational ORationalB>
inline bool operator<=(ORationalA &&first, ORationalB &&second) {
  return !(std::forward<ORationalB>(second) < std::forward<ORationalA>(first));
}
template <OnlyRational ORationalA, OnlyRational ORationalB>
inline bool operator>=(ORationalA &&first, ORationalB &&second) {
  return !(std::forward<ORationalA>(first) < std::forward<ORationalB>(second));
}
template <OnlyRational ORationalA, OnlyRational ORationalB>
inline bool operator==(ORationalA &&first, ORationalB &&second) {
  return std::forward<ORationalA>(first).num_ * std::forward<ORationalB>(second).den_ ==
         std::forward<ORationalB>(second).num_ * std::forward<ORationalA>(first).den_;
}
template <OnlyRational ORationalA, OnlyRational ORationalB>
inline bool operator!=(ORationalA &&first, ORationalB &&second) {
  return !(std::forward<ORationalA>(first) == std::forward<ORationalB>(second));
}

template <OnlyRational ORational, SomeInt Integer>
inline bool operator<(ORational &&rational, Integer &&integer) {
  return std::forward<ORational>(rational).num_ <
         (std::forward<Integer>(integer) * std::forward<ORational>(rational).den_);
}
template <OnlyRational ORational, SomeInt Integer>
inline bool operator>(ORational &&rational, Integer &&integer) {
  return std::forward<ORational>(rational).num_ >
         (std::forward<Integer>(integer) * std::forward<ORational>(rational).den_);
}
template <OnlyRational ORational, SomeInt Integer>
inline bool operator>=(ORational &&rational, Integer &&integer) {
  return !(std::forward<ORational>(rational) < std::forward<Integer>(integer));
}
template <OnlyRational ORational, SomeInt Integer>
inline bool operator<=(ORational &&rational, Integer &&integer) {
  return !(std::forward<ORational>(rational) > std::forward<Integer>(integer));
}
template <OnlyRational ORational, SomeInt Integer>
inline bool operator==(ORational &&rational, Integer &&integer) {
  return std::forward<ORational>(rational).num_ ==
         (std::forward<Integer>(integer) * std::forward<ORational>(rational).den_);
}
template <OnlyRational ORational, SomeInt Integer>
inline bool operator!=(ORational &&rational, Integer &&integer) {
  return !(std::forward<ORational>(rational) == std::forward<Integer>(integer));
}

template <OnlyRational ORational, SomeInt Integer>
inline bool operator<(Integer &&integer, ORational &&rational) {
  return std::forward<ORational>(rational) > std::forward<Integer>(integer);
}
template <OnlyRational ORational, SomeInt Integer>
inline bool operator>(Integer &&integer, ORational &&rational) {
  return std::forward<ORational>(rational) < std::forward<Integer>(integer);
}
template <OnlyRational ORational, SomeInt Integer>
inline bool operator<=(Integer &&integer, ORational &&rational) {
  return std::forward<ORational>(rational) >= std::forward<Integer>(integer);
}
template <OnlyRational ORational, SomeInt Integer>
inline bool operator>=(Integer &&integer, ORational &&rational) {
  return std::forward<ORational>(rational) <= std::forward<Integer>(integer);
}
template <OnlyRational ORational, SomeInt Integer>
inline bool operator==(Integer &&integer, ORational &&rational) {
  return std::forward<ORational>(rational) == std::forward<Integer>(integer);
}
template <OnlyRational ORational, SomeInt Integer>
inline bool operator!=(Integer &&integer, ORational &&rational) {
  return std::forward<ORational>(rational) != std::forward<Integer>(integer);
}