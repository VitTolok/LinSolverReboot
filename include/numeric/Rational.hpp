#pragma once
#include "BigInt.hpp"
#include "UBigInt.hpp"
#include "definitions.hpp"
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <compare>
#include <type_traits>

class InvalidRational : public MachineError<InvalidRational> {
 public:
  InvalidRational() : MachineError("Denominator can't be 0") {
  }
};

class InvalidRationalString : public MachineError<InvalidRationalString> {
 public:
  InvalidRationalString() : MachineError("Invalid Rational String") {
  }
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
  constexpr inline Rational() : num_(), den_(1) {
  }
  constexpr inline Rational(const Rational &) = default;
  constexpr inline Rational(Rational &&) = default;
  template <SomeInt Integer>
  explicit constexpr inline Rational(Integer &&);
  template <SomeInt Numerator, SomeInt Denominator>
  constexpr inline Rational(Numerator &&, Denominator &&);

  constexpr inline Rational &operator=(const Rational &) = default;
  constexpr inline Rational &operator=(Rational &&) = default;
  template <SomeInt Integer>
  constexpr inline Rational &operator=(Integer &&);

  constexpr inline const BigInt &Num() const &;
  constexpr inline const UBigInt &Den() const &;
  constexpr inline BigInt &&Num() &&;
  constexpr inline UBigInt &&Den() &&;

  friend constexpr inline Rational &operator+=(Rational &, const Rational &);
  friend constexpr inline Rational &operator-=(Rational &, const Rational &);
  friend constexpr inline Rational &operator*=(Rational &, const Rational &);
  friend constexpr inline Rational &operator/=(Rational &, const Rational &);
  friend constexpr inline Rational &operator%=(Rational &, const Rational &);

  friend constexpr inline Rational &operator+=(Rational &, Rational &&);
  friend constexpr inline Rational &operator-=(Rational &, Rational &&);

  template <SomeInt Integer>
  friend constexpr inline Rational &operator+=(Rational &, Integer &&);
  template <SomeInt Integer>
  friend constexpr inline Rational &operator-=(Rational &, Integer &&);
  template <SomeInt Integer>
  friend constexpr inline Rational &operator*=(Rational &, Integer &&);
  template <SomeInt Integer>
  friend constexpr inline Rational &operator/=(Rational &, Integer &&);
  template <SomeInt Integer>
  friend constexpr inline Rational &operator%=(Rational &, Integer &&);

  friend constexpr inline Rational operator-(const Rational &);
  friend constexpr inline Rational operator-(Rational &&);

  template <OnlyRational ORationalA, OnlyRational ORationalB>
  friend constexpr inline Rational operator+(ORationalA &&, ORationalB &&);
  template <OnlyRational ORationalA, OnlyRational ORationalB>
  friend constexpr inline Rational operator-(ORationalA &&, ORationalB &&);
  template <OnlyRational ORationalA, OnlyRational ORationalB>
  friend constexpr inline Rational operator*(ORationalA &&, ORationalB &&);
  template <OnlyRational ORationalA, OnlyRational ORationalB>
  friend constexpr inline Rational operator/(ORationalA &&, ORationalB &&);
  template <OnlyRational ORationalA, OnlyRational ORationalB>
  friend constexpr inline Rational operator%(ORationalA &&, ORationalB &&);

  template <OnlyRational ORational, SomeInt Integer>
  friend constexpr inline Rational operator+(ORational &&, Integer &&);
  template <OnlyRational ORational, SomeInt Integer>
  friend constexpr inline Rational operator-(ORational &&, Integer &&);
  template <OnlyRational ORational, SomeInt Integer>
  friend constexpr inline Rational operator*(ORational &&, Integer &&);
  template <OnlyRational ORational, SomeInt Integer>
  friend constexpr inline Rational operator/(ORational &&, Integer &&);
  template <OnlyRational ORational, SomeInt Integer>
  friend constexpr inline Rational operator%(ORational &&, Integer &&);

  template <OnlyRational ORational, SomeInt Integer>
  friend constexpr inline Rational operator+(Integer &&, ORational &&);
  template <OnlyRational ORational, SomeInt Integer>
  friend constexpr inline Rational operator-(Integer &&, ORational &&);
  template <OnlyRational ORational, SomeInt Integer>
  friend constexpr inline Rational operator*(Integer &&, ORational &&);
  template <OnlyRational ORational, SomeInt Integer>
  friend constexpr inline Rational operator/(Integer &&, ORational &&);
  template <OnlyRational ORational, SomeInt Integer>
  friend constexpr inline Rational operator%(Integer &&, ORational &&);

  template <OnlyRational ORationalA, OnlyRational ORationalB>
  friend constexpr inline std::strong_ordering operator<=>(ORationalA &&, ORationalB &&);
  template <OnlyRational ORationalA, OnlyRational ORationalB>
  friend constexpr inline bool operator==(ORationalA &&, ORationalB &&);

  template <OnlyRational ORational, SomeInt Integer>
  friend constexpr inline std::strong_ordering operator<=>(ORational &&, Integer &&);
  template <OnlyRational ORational, SomeInt Integer>
  friend constexpr inline bool operator==(ORational &&, Integer &&);

  template <OnlyRational ORational, SomeInt Integer>
  friend constexpr inline std::strong_ordering operator<=>(Integer &&, ORational &&);
  template <OnlyRational ORational, SomeInt Integer>
  friend constexpr inline bool operator==(Integer &&, ORational &&);

  constexpr inline char *ToString(char *, size_t) const;
  [[nodiscard]] constexpr inline std::string ToString() const;
  [[nodiscard]] static constexpr inline Rational FromString(std::string_view);
  static constexpr inline Rational &FromString(std::string_view, Rational &);

  friend constexpr inline std::ostream &operator<<(std::ostream &, const Rational &);
  friend constexpr inline std::istream &operator>>(std::istream &, Rational &);

  [[nodiscard]] explicit constexpr inline operator BigInt() const &;
  [[nodiscard]] explicit constexpr inline operator UBigInt() const &;
  [[nodiscard]] explicit constexpr inline operator BigInt() &&;
  [[nodiscard]] explicit constexpr inline operator UBigInt() &&;

  constexpr inline Rational &Floor();

  struct Approximation {
    long double multiplier;
    int64_t power;
  };
  [[nodiscard]] constexpr inline Approximation GetApproximation() const noexcept;

 private:
  BigInt num_;
  UBigInt den_;
  constexpr inline void Normalize();
};

template <SomeInt Integer>
constexpr inline Rational::Rational(Integer &&integer) : num_(std::forward<Integer>(integer)), den_(1) {
}
template <SomeInt Numerator, SomeInt Denominator>
constexpr inline Rational::Rational(Numerator &&num, Denominator &&den) : num_(), den_() {
  if constexpr (std::is_signed_v<Denominator> || std::is_same_v<std::decay_t<Denominator>, BigInt>) {
    if (den < 0) {
      (num_ = std::forward<Numerator>(num)) *= -1;
      den_ = (std::forward<Denominator>(den) * -1).Abs();
    }
  }
  num_ = std::forward<Numerator>(num);
  den_ = std::forward<Denominator>(den);
  Normalize();
}

template <SomeInt Integer>
constexpr inline Rational &Rational::operator=(Integer &&integer) {
  num_ = std::forward<Integer>(integer);
  den_ = 1;
  return *this;
}

constexpr inline const BigInt &Rational::Num() const & {
  return this->num_;
}
constexpr inline const UBigInt &Rational::Den() const & {
  return this->den_;
}
constexpr inline BigInt &&Rational::Num() && {
  return std::move(this->num_);
}
constexpr inline UBigInt &&Rational::Den() && {
  return std::move(this->den_);
}

template <SomeInt Integer>
constexpr inline Rational &operator+=(Rational &rational, Integer &&integer) {
  rational.num_ += (std::forward<Integer>(integer) * rational.den_);
  return rational;
}
template <SomeInt Integer>
constexpr inline Rational &operator-=(Rational &rational, Integer &&integer) {
  rational.num_ -= (std::forward<Integer>(integer) * rational.den_);
  return rational;
}
template <SomeInt Integer>
constexpr inline Rational &operator*=(Rational &rational, Integer &&integer) {
  rational.num_ *= std::forward<Integer>(integer);
  rational.Normalize();
  return rational;
}
template <SomeInt Integer>
constexpr inline Rational &operator/=(Rational &rational, Integer &&integer) {
  rational.den_ *= std::forward<Integer>(integer);
  rational.Normalize();
  return rational;
}
template <SomeInt Integer>
constexpr inline Rational &operator%=(Rational &rational, Integer &&integer) {
  rational.num_ -= (static_cast<BigInt>(rational) / integer) * std::forward<Integer>(integer);
  rational.Normalize();
  return rational;
}

[[nodiscard]] constexpr inline Rational operator-(const Rational &rational) {
  Rational copy(rational);
  return -std::move(copy);
}
[[nodiscard]] constexpr inline Rational operator-(Rational &&rational) {
  rational.num_ = -std::move(rational.num_);
  return std::move(rational);
}

template <OnlyRational ORationalA, OnlyRational ORationalB>
[[nodiscard]] constexpr inline Rational operator+(ORationalA &&first, ORationalB &&second) {
  if constexpr (std::is_rvalue_reference_v<decltype(first)>) {
    return std::move(first += second);
  } else if constexpr (std::is_rvalue_reference_v<decltype(second)>) {
    return std::move(second += first);
  } else {
    Rational copy(first);
    copy += second;
    return copy;
  }
}
template <OnlyRational ORationalA, OnlyRational ORationalB>
[[nodiscard]] constexpr inline Rational operator-(ORationalA &&first, ORationalB &&second) {
  if constexpr (std::is_rvalue_reference_v<decltype(first)>) {
    return std::move(first -= second);
  } else if constexpr (std::is_rvalue_reference_v<decltype(second)>) {
    return std::move((second -= first) *= -1);
  } else {
    Rational copy(first);
    copy -= second;
    return copy;
  }
}
template <OnlyRational ORationalA, OnlyRational ORationalB>
[[nodiscard]] constexpr inline Rational operator*(ORationalA &&first, ORationalB &&second) {
  if constexpr (std::is_rvalue_reference_v<decltype(first)>) {
    return std::move(first *= second);
  } else if constexpr (std::is_rvalue_reference_v<decltype(second)>) {
    return std::move(second *= first);
  } else {
    Rational copy(first);
    copy *= second;
    return copy;
  }
}
template <OnlyRational ORationalA, OnlyRational ORationalB>
[[nodiscard]] constexpr inline Rational operator/(ORationalA &&first, ORationalB &&second) {
  if constexpr (std::is_rvalue_reference_v<decltype(first)>) {
    return std::move(first /= second);
  } else if constexpr (std::is_rvalue_reference_v<decltype(second)>) {
    return 1 / std::move(second /= first);
  } else {
    Rational copy(first);
    copy /= second;
    return copy;
  }
}
template <OnlyRational ORationalA, OnlyRational ORationalB>
[[nodiscard]] constexpr inline Rational operator%(ORationalA &&first, ORationalB &&second) {
  if constexpr (std::is_rvalue_reference_v<decltype(first)>) {
    return std::move(first %= second);
  } else {
    Rational copy(first);
    copy %= second;
    return copy;
  }
}

template <OnlyRational ORational, SomeInt Integer>
[[nodiscard]] constexpr inline Rational operator+(ORational &&rational, Integer &&integer) {
  if constexpr (std::is_rvalue_reference_v<decltype(rational)>) {
    return std::move(rational += integer);
  } else if constexpr (std::is_rvalue_reference_v<decltype(integer)>) {
    return rational + Rational(std::move(integer));
  } else {
    Rational copy(rational);
    copy += integer;
    return copy;
  }
}
template <OnlyRational ORational, SomeInt Integer>
[[nodiscard]] constexpr inline Rational operator-(ORational &&rational, Integer &&integer) {
  if constexpr (std::is_rvalue_reference_v<decltype(rational)>) {
    return std::move(rational -= integer);
  } else if constexpr (std::is_rvalue_reference_v<decltype(integer)>) {
    return rational - Rational(std::move(integer));
  } else {
    Rational copy(rational);
    copy -= integer;
    return copy;
  }
}
template <OnlyRational ORational, SomeInt Integer>
[[nodiscard]] constexpr inline Rational operator*(ORational &&rational, Integer &&integer) {
  if constexpr (std::is_rvalue_reference_v<decltype(rational)>) {
    return std::move(rational *= integer);
  } else if constexpr (std::is_rvalue_reference_v<decltype(integer)>) {
    return rational * Rational(std::move(integer));
  } else {
    Rational copy(rational);
    copy *= integer;
    return copy;
  }
}
template <OnlyRational ORational, SomeInt Integer>
[[nodiscard]] constexpr inline Rational operator/(ORational &&rational, Integer &&integer) {
  if constexpr (std::is_rvalue_reference_v<decltype(rational)>) {
    return std::move(rational /= integer);
  } else if constexpr (std::is_rvalue_reference_v<decltype(integer)>) {
    return rational / Rational(std::move(integer));
  } else {
    Rational copy(rational);
    copy /= integer;
    return copy;
  }
}
template <OnlyRational ORational, SomeInt Integer>
[[nodiscard]] constexpr inline Rational operator%(ORational &&rational, Integer &&integer) {
  if constexpr (std::is_rvalue_reference_v<decltype(rational)>) {
    return std::move(rational %= integer);
  } else if constexpr (std::is_rvalue_reference_v<decltype(integer)>) {
    return rational % Rational(std::move(integer));
  } else {
    Rational copy(rational);
    copy %= integer;
    return copy;
  }
}

template <OnlyRational ORational, SomeInt Integer>
[[nodiscard]] constexpr inline Rational operator+(Integer &&integer, ORational &&rational) {
  return std::forward<ORational>(rational) + std::forward<Integer>(integer);
}
template <OnlyRational ORational, SomeInt Integer>
[[nodiscard]] constexpr inline Rational operator-(Integer &&integer, ORational &&rational) {
  return (std::forward<ORational>(rational) - std::forward<Integer>(integer)) * -1;
}
template <OnlyRational ORational, SomeInt Integer>
[[nodiscard]] constexpr inline Rational operator*(Integer &&integer, ORational &&rational) {
  return std::forward<ORational>(rational) * std::forward<Integer>(integer);
}
template <OnlyRational ORational, SomeInt Integer>
[[nodiscard]] constexpr inline Rational operator/(Integer &&integer, ORational &&rational) {
  if (rational.num_ == 0) {
    throw InvalidRational{};
  }
  Rational inverse = std::forward<ORational>(rational) / std::forward<Integer>(integer);
  std::swap(inverse.num_.Abs(), inverse.den_);
  return inverse;
}
template <OnlyRational ORational, SomeInt Integer>
[[nodiscard]] constexpr inline Rational operator%(Integer &&integer, ORational &&rational) {
  return std::forward<Integer>(integer) - (integer / rational).Floor() * std::forward<ORational>(rational);
}

template <OnlyRational ORationalA, OnlyRational ORationalB>
[[nodiscard]] constexpr inline std::strong_ordering operator<=>(ORationalA &&first, ORationalB &&second) {
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
[[nodiscard]] constexpr inline bool operator==(ORationalA &&first, ORationalB &&second) {
  return (std::forward<ORationalA>(first) <=> std::forward<ORationalB>(second)) == std::strong_ordering::equal;
}

template <OnlyRational ORational, SomeInt Integer>
[[nodiscard]] constexpr inline std::strong_ordering operator<=>(ORational &&rational, Integer &&integer) {
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
[[nodiscard]] constexpr inline bool operator==(ORational &&rational, Integer &&integer) {
  return (std::forward<ORational>(rational) <=> std::forward<Integer>(integer)) == std::strong_ordering::equal;
}

template <OnlyRational ORational, SomeInt Integer>
[[nodiscard]] constexpr inline std::strong_ordering operator<=>(Integer &&integer, ORational &&rational) {
  return 0 <=> (std::forward<ORational>(rational) <=> std::forward<Integer>(integer));
}
template <OnlyRational ORational, SomeInt Integer>
[[nodiscard]] constexpr inline bool operator==(Integer &&integer, ORational &&rational) {
  return std::forward<ORational>(rational) == std::forward<Integer>(integer);
}
constexpr inline char *Rational::ToString(char *const start, const size_t size) const {
  if (num_.Abs() < 1'000'000 && den_ < 1'000'000) {
    {
      char result[10]{};
      auto num_ptr = num_.ToString(result, 7);
      if (den_ == 1) {
        for (size_t i = 0; result + i < num_ptr; i++) {
          start[i] = result[i];
        }
        return start + (num_ptr - result);
      }
      *num_ptr = '/';
      num_ptr++;
      auto den_ptr = den_.ToString(num_ptr, static_cast<size_t>((result + 10) - num_ptr));
      if (den_ptr != nullptr) {
        if (size >= static_cast<size_t>(den_ptr - result)) {
          for (size_t i = 0; result + i < den_ptr; i++) {
            start[i] = result[i];
          }
        }
        return nullptr;
      }
    }
  }
  auto approx = GetApproximation();
  char *ptr = nullptr;
  if (num_ < 0) {
    if (size < 1) {
      return nullptr;
    }
    start[0] = '-';
    ptr = DoublePrinter(start + 1, size - 1, Abs(approx.multiplier));
  } else {
    ptr = DoublePrinter(start, size, Abs(approx.multiplier));
  }
  if (ptr == nullptr) {
    return nullptr;
  }
  auto [eptr, errc] = std::to_chars(ptr, start + size, approx.power);
  if (errc != std::errc{}) {
    return nullptr;
  }
  return eptr;
}
[[nodiscard]] constexpr inline std::string Rational::ToString() const {
  char result[32]{};
  ToString(result, 31);
  return result;
}

[[nodiscard]] constexpr inline Rational Rational::FromString(std::string_view str) {
  Rational result;
  FromString(str, result);
  return result;
}
constexpr inline Rational &Rational::FromString(std::string_view str, Rational &result) {
  result = 0;
  if (str.empty()) {
    return result;
  }
  // division parsing
  {
    const auto div_pos = str.find('/');
    if (div_pos != std::string_view::npos) {
      BigInt::FromString(str.substr(0, div_pos), result.num_);
      UBigInt::FromString(str.substr(div_pos + 1), result.den_);
      result.Normalize();
      return result;
    }
  }
  // exponent parsing
  int64_t exp = 0;
  {
    const auto exp_pos = str.find('e');
    if (exp_pos != std::string_view::npos) {
      auto [ptr, errc] = std::from_chars(str.begin() + exp_pos + 1, str.end(), exp);
      if (ptr != str.end() || errc != std::errc{}) [[unlikely]] {
        throw InvalidRationalString{};
      }
      str = str.substr(0, exp_pos);
    }
  }
  // decimal digits parsing
  UBigInt frac;
  size_t frac_size = 0;
  {
    const auto dot_pos = str.find('.');
    if (dot_pos != std::string_view::npos) {
      frac_size = str.size() + 1 - dot_pos;
      try {
        UBigInt::FromString(str.substr(dot_pos + 1), frac);
      } catch (const BigIntInvalidString &e) {
        throw InvalidRationalString{};
      }
      str = str.substr(0, dot_pos);
    }
  }
  // rational number construction
  BigInt::FromString(str, result.num_);
  if (frac_size <= 19) {
    result.den_ = TenToIntPower(frac_size);
  } else {
    result.den_ = Power(UBigInt(10), frac_size);
  }
  result.num_ *= result.den_;
  if (!str.empty() && str[0] == '-') {
    result.num_ -= frac;
  } else {
    result.num_ += frac;
  }
  result.Normalize();
  if (exp > 0) {
    if (exp <= 19) {
      return result *= TenToIntPower(static_cast<uint64_t>(exp));
    }
    return result *= Power(UBigInt(10), static_cast<size_t>(exp));
  }
  if (exp < 0) {
    if ((-exp) <= 19) {
      return result /= TenToIntPower(static_cast<uint64_t>(-exp));
    }
    return result /= Power(UBigInt(10), static_cast<size_t>(-exp));
  }
  return result;
}

constexpr inline Rational &operator+=(Rational &base, const Rational &mod) {
  base.num_ = (std::move(base.num_) * mod.den_) + (mod.num_ * base.den_);
  base.den_ *= mod.den_;
  base.Normalize();
  return base;
}
constexpr inline Rational &operator-=(Rational &base, const Rational &mod) {
  base.num_ = (std::move(base.num_) * mod.den_) - (mod.num_ * base.den_);
  base.den_ *= mod.den_;
  base.Normalize();
  return base;
}
constexpr inline Rational &operator*=(Rational &base, const Rational &mod) {
  base.num_ *= mod.num_;
  base.den_ *= mod.den_;
  base.Normalize();
  return base;
}
constexpr inline Rational &operator/=(Rational &base, const Rational &mod) {
  base.num_ *= mod.den_;
  base.den_ *= mod.num_.Abs();
  if (mod.num_ < 0) {
    base.num_ = -std::move(base.num_);
  }
  base.Normalize();
  return base;
}
constexpr inline Rational &operator%=(Rational &base, const Rational &mod) {
  return base -= std::move((base / mod).Floor()) * mod;
}

constexpr inline Rational &operator+=(Rational &first, Rational &&second) {
  first.num_ = (std::move(first.num_) * second.den_) + (first.den_ * std::move(second.num_));
  first.den_ *= second.den_;
  first.Normalize();
  return first;
}
constexpr inline Rational &operator-=(Rational &first, Rational &&second) {
  first.num_ = (std::move(first.num_) * second.den_) - (first.den_ * std::move(second.num_));
  first.den_ *= second.den_;
  first.Normalize();
  return first;
}

[[nodiscard]] constexpr inline Rational::operator BigInt() const & {
  return num_ / den_;
}
[[nodiscard]] constexpr inline Rational::operator UBigInt() const & {
  if (num_ < 0) [[unlikely]] {
    throw UBigIntUnderflow{};
  }
  return num_.Abs() / den_;
}
[[nodiscard]] constexpr inline Rational::operator BigInt() && {
  return std::move(num_ /= den_);
}
[[nodiscard]] constexpr inline Rational::operator UBigInt() && {
  if (num_ < 0) [[unlikely]] {
    throw UBigIntUnderflow{};
  }
  return std::move(num_.Abs() /= den_);
}

constexpr inline Rational &Rational::Floor() {
  num_ /= den_;
  den_ = 1;
  return *this;
}

[[nodiscard]] constexpr inline Rational::Approximation Rational::GetApproximation() const noexcept {
  auto num_approx = num_.GetApproximation();
  auto den_approx = den_.GetApproximation();
  Approximation approx{num_approx.multiplier / den_approx.multiplier,
                       static_cast<int64_t>(num_approx.power) - static_cast<int64_t>(den_approx.power)};
  return approx;
}

constexpr inline void Rational::Normalize() {
  if (this->den_ == 1_uint64) {
    return;
  }
  if (this->den_ == 0_uint64) [[unlikely]] {
    throw InvalidRational{};
  }
  UBigInt gcd = GCD(this->num_.Abs(), this->den_);
  this->num_ /= gcd;
  this->den_ /= gcd;
}

constexpr inline std::ostream &operator<<(std::ostream &out, const Rational &rational) {
  return out << rational.ToString();
}
constexpr inline std::istream &operator>>(std::istream &in, Rational &rational) {
  std::string input;
  in >> input;
  rational = Rational::FromString(input);
  return in;
}