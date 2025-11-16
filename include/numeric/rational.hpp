#pragma once
#include "BigInt.hpp"
#include "definitions.hpp"
#include "BigInt.hpp"
#include <cstdint>
#include <iostream>
#include <compare>

class InvalidRational : public MachineError {
 public:
  InvalidRational() {
    message_ptr = message_;
  }

 private:
  inline constexpr static const char message_[] = "Denominator can't be 0";
};

class InvalidRationalString : public MachineError {
 public:
  InvalidRationalString() {
    message_ptr = message_;
  }

 private:
  inline constexpr static const char message_[] = "Invalid Rational String";
};

class Rational;

template <class Integer>
concept SomeBigInt = std::same_as<UBigInt, std::decay_t<Integer>> || std::same_as<BigInt, std::decay_t<Integer>>;
template <class Integer>
concept SomeInt = std::integral<std::decay_t<Integer>> || SomeBigInt<Integer>;
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
  inline Rational &operator=(Integer &&);

  inline const BigInt &Num() const &;
  inline const UBigInt &Den() const &;
  inline BigInt &&Num() &&;
  inline UBigInt &&Den() &&;

  friend inline Rational &operator+=(Rational &, const Rational &);
  friend inline Rational &operator-=(Rational &, const Rational &);
  friend inline Rational &operator*=(Rational &, const Rational &);
  friend inline Rational &operator/=(Rational &, const Rational &);
  friend inline Rational &operator%=(Rational &, const Rational &);

  friend inline Rational &operator+=(Rational &, Rational &&);
  friend inline Rational &operator-=(Rational &, Rational &&);

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
  friend inline std::strong_ordering operator<=>(ORationalA &&, ORationalB &&);
  template <OnlyRational ORationalA, OnlyRational ORationalB>
  friend inline bool operator==(ORationalA &&, ORationalB &&);

  template <OnlyRational ORational, SomeInt Integer>
  friend inline std::strong_ordering operator<=>(ORational &&, Integer &&);
  template <OnlyRational ORational, SomeInt Integer>
  friend inline bool operator==(ORational &&, Integer &&);

  template <OnlyRational ORational, SomeInt Integer>
  friend inline std::strong_ordering operator<=>(Integer &&, ORational &&);
  template <OnlyRational ORational, SomeInt Integer>
  friend inline bool operator==(Integer &&, ORational &&);

  friend std::ostream &operator<<(std::ostream &, const Rational &);
  friend std::istream &operator>>(std::istream &, Rational &);

  explicit inline operator BigInt() const &;
  explicit inline operator UBigInt() const &;
  explicit inline operator BigInt() &&;
  explicit inline operator UBigInt() &&;

  inline Rational &Floor();

  struct Approximation {
    long double multiplier;
    int64_t power;
  };
  inline Approximation GetApproximation() const noexcept;

 private:
  BigInt num_;
  UBigInt den_;
  inline void Normalize();
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

inline const BigInt &Rational::Num() const & {
  return this->num_;
}
inline const UBigInt &Rational::Den() const & {
  return this->den_;
}
inline BigInt &&Rational::Num() && {
  return std::move(this->num_);
}
inline UBigInt &&Rational::Den() && {
  return std::move(this->den_);
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
  if constexpr (std::is_rvalue_reference_v<decltype(first)>) {
    return std::move(first += second);
  } else if constexpr (std::is_rvalue_reference_v<decltype(second)>) {
    return std::move(second += first);
  } else {
    Rational copy(first);
    return std::move(copy += second);
  }
}
template <OnlyRational ORationalA, OnlyRational ORationalB>
inline Rational operator-(ORationalA &&first, ORationalB &&second) {
  if constexpr (std::is_rvalue_reference_v<decltype(first)>) {
    return std::move(first -= second);
  } else if constexpr (std::is_rvalue_reference_v<decltype(second)>) {
    return std::move((second -= first) *= -1);
  } else {
    Rational copy(first);
    return std::move(copy -= second);
  }
}
template <OnlyRational ORationalA, OnlyRational ORationalB>
inline Rational operator*(ORationalA &&first, ORationalB &&second) {
  if constexpr (std::is_rvalue_reference_v<decltype(first)>) {
    return std::move(first *= second);
  } else if constexpr (std::is_rvalue_reference_v<decltype(second)>) {
    return std::move(second *= first);
  } else {
    Rational copy(first);
    return std::move(copy *= second);
  }
}
template <OnlyRational ORationalA, OnlyRational ORationalB>
inline Rational operator/(ORationalA &&first, ORationalB &&second) {
  if constexpr (std::is_rvalue_reference_v<decltype(first)>) {
    return std::move(first /= second);
  } else if constexpr (std::is_rvalue_reference_v<decltype(second)>) {
    return 1 / std::move(second /= first);
  } else {
    Rational copy(first);
    return std::move(copy /= second);
  }
}
template <OnlyRational ORationalA, OnlyRational ORationalB>
inline Rational operator%(ORationalA &&first, ORationalB &&second) {
  if constexpr (std::is_rvalue_reference_v<decltype(first)>) {
    return std::move(first %= second);
  } else {
    Rational copy(first);
    return std::move(copy %= second);
  }
}

template <OnlyRational ORational, SomeInt Integer>
inline Rational operator+(ORational &&rational, Integer &&integer) {
  if constexpr (std::is_rvalue_reference_v<decltype(rational)>) {
    return std::move(rational += integer);
  } else if constexpr (std::is_rvalue_reference_v<decltype(integer)>) {
    return std::move(rational + Rational(std::move(integer)));
  } else {
    Rational copy(rational);
    return std::move(copy += integer);
  }
}
template <OnlyRational ORational, SomeInt Integer>
inline Rational operator-(ORational &&rational, Integer &&integer) {
  if constexpr (std::is_rvalue_reference_v<decltype(rational)>) {
    return std::move(rational -= integer);
  } else if constexpr (std::is_rvalue_reference_v<decltype(integer)>) {
    return std::move(rational - Rational(std::move(integer)));
  } else {
    Rational copy(rational);
    return std::move(copy -= integer);
  }
}
template <OnlyRational ORational, SomeInt Integer>
inline Rational operator*(ORational &&rational, Integer &&integer) {
  if constexpr (std::is_rvalue_reference_v<decltype(rational)>) {
    return std::move(rational *= integer);
  } else if constexpr (std::is_rvalue_reference_v<decltype(integer)>) {
    return std::move(rational * Rational(std::move(integer)));
  } else {
    Rational copy(rational);
    return std::move(copy *= integer);
  }
}
template <OnlyRational ORational, SomeInt Integer>
inline Rational operator/(ORational &&rational, Integer &&integer) {
  if constexpr (std::is_rvalue_reference_v<decltype(rational)>) {
    return std::move(rational /= integer);
  } else if constexpr (std::is_rvalue_reference_v<decltype(integer)>) {
    return rational / Rational(std::move(integer));
  } else {
    Rational copy(rational);
    return std::move(copy /= integer);
  }
}
template <OnlyRational ORational, SomeInt Integer>
inline Rational operator%(ORational &&rational, Integer &&integer) {
  if constexpr (std::is_rvalue_reference_v<decltype(rational)>) {
    return std::move(rational %= integer);
  } else if constexpr (std::is_rvalue_reference_v<decltype(integer)>) {
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
  if (rational.num_ == 0) {
    throw InvalidRational{};
  }
  Rational inverse = std::forward<ORational>(rational) / std::forward<Integer>(integer);
  std::swap(inverse.num_.Abs(), inverse.den_);
  return inverse;
}
template <OnlyRational ORational, SomeInt Integer>
inline Rational operator%(Integer &&integer, ORational &&rational) {
  return std::forward<Integer>(integer) - (integer / rational).Round() * std::forward<ORational>(rational);
}

template <OnlyRational ORationalA, OnlyRational ORationalB>
inline std::strong_ordering operator<=>(ORationalA &&first, ORationalB &&second) {
  if (first.num_.Abs() == 0) {
    return 0 <=> second.num_;
  }
  if ((first.num_ <=> 0) != (second.num_ <=> 0)) {
    return first.num_ <=> 0;
  }
  return std::forward<ORationalA>(first).num_ * std::forward<ORationalB>(second).den_ <=>
         std::forward<ORationalB>(second).num_ * std::forward<ORationalA>(first).den_;
}
template <OnlyRational ORationalA, OnlyRational ORationalB>
inline bool operator==(ORationalA &&first, ORationalB &&second) {
  return (std::forward<ORationalA>(first) <=> std::forward<ORationalB>(second)) == std::strong_ordering::equal;
}

template <OnlyRational ORational, SomeInt Integer>
inline std::strong_ordering operator<=>(ORational &&rational, Integer &&integer) {
  if (integer == 0) {
    return rational.num_ <=> 0;
  }
  if ((integer <=> 0) != (rational.num_ <=> 0)) {
    return 0 <=> integer;
  }
  return std::forward<ORational>(rational).num_ <=>
         (std::forward<Integer>(integer) * std::forward<ORational>(rational).den_);
}
template <OnlyRational ORational, SomeInt Integer>
inline bool operator==(ORational &&rational, Integer &&integer) {
  return (std::forward<ORational>(rational) <=> std::forward<Integer>(integer)) == std::strong_ordering::equal;
}

template <OnlyRational ORational, SomeInt Integer>
inline std::strong_ordering operator<=>(Integer &&integer, ORational &&rational) {
  return 0 <=> (std::forward<ORational>(rational) <=> std::forward<Integer>(integer));
}
template <OnlyRational ORational, SomeInt Integer>
inline bool operator==(Integer &&integer, ORational &&rational) {
  return std::forward<ORational>(rational) == std::forward<Integer>(integer);
}

inline Rational &operator+=(Rational &base, const Rational &mod) {
  base.num_ = (std::move(base.num_) * mod.den_) + (mod.num_ * base.den_);
  base.den_ *= mod.den_;
  base.Normalize();
  return base;
}
inline Rational &operator-=(Rational &base, const Rational &mod) {
  base.num_ = (std::move(base.num_) * mod.den_) - (mod.num_ * base.den_);
  base.den_ *= mod.den_;
  base.Normalize();
  return base;
}
inline Rational &operator*=(Rational &base, const Rational &mod) {
  base.num_ *= mod.num_;
  base.den_ *= mod.den_;
  base.Normalize();
  return base;
}
inline Rational &operator/=(Rational &base, const Rational &mod) {
  base.num_ *= mod.den_;
  base.den_ *= mod.num_.Abs();
  if (mod.num_ < 0) {
    base.num_ = -std::move(base.num_);
  }
  base.Normalize();
  return base;
}
inline Rational &operator%=(Rational &base, const Rational &mod) {
  return base -= std::move((base / mod).Floor()) * mod;
}

inline Rational &operator+=(Rational &first, Rational &&second) {
  first.num_ = (std::move(first.num_) * second.den_) + (first.den_ * std::move(second.num_));
  first.den_ *= second.den_;
  first.Normalize();
  return first;
}
inline Rational &operator-=(Rational &first, Rational &&second) {
  first.num_ = (std::move(first.num_) * second.den_) - (first.den_ * std::move(second.num_));
  first.den_ *= second.den_;
  first.Normalize();
  return first;
}

inline Rational::operator BigInt() const & {
  return num_ / den_ * den_;
}
inline Rational::operator UBigInt() const & {
  if (num_ < 0) {
    throw UBigIntUnderflow{};
  }
  return num_.Abs() / den_ * den_;
}
inline Rational::operator BigInt() && {
  return std::move((num_ /= den_) *= den_);
}
inline Rational::operator UBigInt() && {
  if (num_ < 0) {
    throw UBigIntUnderflow{};
  }
  return std::move((num_.Abs() /= den_) *= den_);
}

inline Rational &Rational::Floor() {
  num_ /= den_;
  den_ = 1;
  return *this;
}

inline Rational::Approximation Rational::GetApproximation() const noexcept {
  auto num_approx = num_.GetApproximation();
  auto den_approx = den_.GetApproximation();
  Approximation approx{num_approx.multiplier / den_approx.multiplier,
                       static_cast<int64_t>(num_approx.power) - static_cast<int64_t>(den_approx.power)};
  if (approx.multiplier < 1 - 5e-12) {
    approx.multiplier *= 10;
    --approx.power;
  }
  return approx;
}

inline void Rational::Normalize() {
  if (this->den_ == 1_uint64) {
    return;
  }
  if (this->den_ == 0_uint64) {
    throw InvalidRational();
  }
  UBigInt gcd = GCD(this->num_.Abs(), this->den_);
  this->num_ /= gcd;
  this->den_ /= gcd;
  return;
}