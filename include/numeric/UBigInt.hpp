#pragma once
#include "definitions.hpp"
#include <deque>
#include <iostream>

class UBigIntUnderflow : public MachineError {
public:
  [[nodiscard]] const char *what() const noexcept override {
    return message_.c_str();
  }

private:
  std::string message_ = "The unsigned big integer cannot go negative";
};

class BigIntInvalidString : public MachineError {
public:
  [[nodiscard]] const char *what() const noexcept override {
    return message_.c_str();
  }

private:
  std::string message_ = "Use either full integers or '1.01612e288' form";
};

class BigIntDivisionByZero : public MachineError {
public:
  [[nodiscard]] const char *what() const noexcept override {
    return message_.c_str();
  }

private:
  std::string message_ = "Can't divide by 0";
};

class UBigInt {
public:
  UBigInt();
  explicit UBigInt(u_int64_t);
  UBigInt(const UBigInt &) = default;
  UBigInt(UBigInt &&) = default;

  UBigInt &operator=(const UBigInt &) = default;
  UBigInt &operator=(UBigInt &&) = default;
  UBigInt &operator=(u_int64_t);

  friend UBigInt &operator+=(UBigInt &, const UBigInt &);
  friend UBigInt &operator-=(UBigInt &, const UBigInt &);
  friend UBigInt &operator*=(UBigInt &, const UBigInt &);
  friend UBigInt &operator/=(UBigInt &, const UBigInt &);
  friend UBigInt &operator%=(UBigInt &, const UBigInt &);
  friend UBigInt &operator+=(UBigInt &, u_int64_t);
  friend UBigInt &operator-=(UBigInt &, u_int64_t);
  friend UBigInt &operator*=(UBigInt &, u_int64_t);
  friend UBigInt &operator/=(UBigInt &, u_int64_t);
  friend UBigInt &operator%=(UBigInt &, u_int64_t);

  friend UBigInt operator+(const UBigInt &, const UBigInt &);
  friend UBigInt operator-(const UBigInt &, const UBigInt &);
  friend UBigInt operator*(const UBigInt &, const UBigInt &);
  friend UBigInt operator/(const UBigInt &, const UBigInt &);
  friend UBigInt operator%(const UBigInt &, const UBigInt &);
  friend UBigInt operator+(const UBigInt &, u_int64_t);
  friend UBigInt operator-(const UBigInt &, u_int64_t);
  friend UBigInt operator*(const UBigInt &, u_int64_t);
  friend UBigInt operator/(const UBigInt &, u_int64_t);
  friend UBigInt operator%(const UBigInt &, u_int64_t);

  friend bool operator<(const UBigInt &, const UBigInt &);
  friend bool operator>(const UBigInt &, const UBigInt &);
  friend bool operator<=(const UBigInt &, const UBigInt &);
  friend bool operator>=(const UBigInt &, const UBigInt &);
  friend bool operator==(const UBigInt &, const UBigInt &);
  friend bool operator!=(const UBigInt &, const UBigInt &);

  friend bool operator<(const UBigInt &, u_int64_t);
  friend bool operator>(const UBigInt &, u_int64_t);
  friend bool operator<=(const UBigInt &, u_int64_t);
  friend bool operator>=(const UBigInt &, u_int64_t);
  friend bool operator==(const UBigInt &, u_int64_t);
  friend bool operator!=(const UBigInt &, u_int64_t);

  friend bool operator<(u_int64_t, const UBigInt &);
  friend bool operator>(u_int64_t, const UBigInt &);
  friend bool operator<=(u_int64_t, const UBigInt &);
  friend bool operator>=(u_int64_t, const UBigInt &);
  friend bool operator==(u_int64_t, const UBigInt &);
  friend bool operator!=(u_int64_t, const UBigInt &);

  friend UBigInt &operator<<=(UBigInt &, size_t);
  friend UBigInt &operator>>=(UBigInt &, size_t);
  friend UBigInt operator<<(const UBigInt &, size_t);
  friend UBigInt operator>>(const UBigInt &, size_t);

  explicit operator u_int64_t() const;
  explicit operator u_int32_t() const;

  friend UBigInt Power(const UBigInt &, size_t);
  friend UBigInt GCD(const UBigInt &, const UBigInt &);

  friend std::ostream &operator<<(std::ostream &, const UBigInt &);
  friend std::istream &operator>>(std::istream &, UBigInt &);
  std::string Exact() const;

  ~UBigInt() = default;

private:
  std::deque<u_int32_t> storage_;
  void Shrink() noexcept;
};