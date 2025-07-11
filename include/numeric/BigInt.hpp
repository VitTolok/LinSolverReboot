#include "UBigInt.hpp"

class BigInt {
public:
  BigInt();
  explicit BigInt(int64_t);
  BigInt(const BigInt &) = default;
  BigInt(BigInt &&) = default;

  BigInt &operator=(const BigInt &) = default;
  BigInt &operator=(BigInt &&) = default;
  BigInt &operator=(int64_t);

  friend BigInt &operator+=(BigInt &, const BigInt &);
  friend BigInt &operator-=(BigInt &, const BigInt &);
  friend BigInt &operator*=(BigInt &, const BigInt &);
  friend BigInt &operator/=(BigInt &, const BigInt &);
  friend BigInt &operator%=(BigInt &, const BigInt &);
  friend BigInt &operator+=(BigInt &, const UBigInt &);
  friend BigInt &operator-=(BigInt &, const UBigInt &);
  friend BigInt &operator*=(BigInt &, const UBigInt &);
  friend BigInt &operator/=(BigInt &, const UBigInt &);
  friend BigInt &operator%=(BigInt &, const UBigInt &);
  friend BigInt &operator+=(BigInt &, int64_t);
  friend BigInt &operator-=(BigInt &, int64_t);
  friend BigInt &operator*=(BigInt &, int64_t);
  friend BigInt &operator/=(BigInt &, int64_t);
  friend BigInt &operator%=(BigInt &, int64_t);
  friend UBigInt &operator+=(UBigInt &, const BigInt &);
  friend UBigInt &operator-=(UBigInt &, const BigInt &);
  friend UBigInt &operator*=(UBigInt &, const BigInt &);
  friend UBigInt &operator/=(UBigInt &, const BigInt &);
  friend UBigInt &operator%=(UBigInt &, const BigInt &);
  friend BigInt operator+(const BigInt &, const BigInt &);
  friend BigInt operator-(const BigInt &, const BigInt &);
  friend BigInt operator*(const BigInt &, const BigInt &);
  friend BigInt operator/(const BigInt &, const BigInt &);
  friend BigInt operator%(const BigInt &, const BigInt &);
  friend BigInt operator+(const BigInt &, const UBigInt &);
  friend BigInt operator-(const BigInt &, const UBigInt &);
  friend BigInt operator*(const BigInt &, const UBigInt &);
  friend BigInt operator/(const BigInt &, const UBigInt &);
  friend BigInt operator%(const BigInt &, const UBigInt &);
  friend BigInt operator+(const BigInt &, int64_t);
  friend BigInt operator-(const BigInt &, int64_t);
  friend BigInt operator*(const BigInt &, int64_t);
  friend BigInt operator/(const BigInt &, int64_t);
  friend BigInt operator%(const BigInt &, int64_t);
  friend UBigInt operator+(const UBigInt &, const BigInt &);
  friend UBigInt operator-(const UBigInt &, const BigInt &);
  friend UBigInt operator*(const UBigInt &, const BigInt &);
  friend UBigInt operator/(const UBigInt &, const BigInt &);
  friend UBigInt operator%(const UBigInt &, const BigInt &);

  friend bool operator<(const BigInt &, const BigInt &);
  friend bool operator>(const BigInt &, const BigInt &);
  friend bool operator<=(const BigInt &, const BigInt &);
  friend bool operator>=(const BigInt &, const BigInt &);
  friend bool operator==(const BigInt &, const BigInt &);
  friend bool operator!=(const BigInt &, const BigInt &);
  friend bool operator<(const BigInt &, const UBigInt &);
  friend bool operator>(const BigInt &, const UBigInt &);
  friend bool operator<=(const BigInt &, const UBigInt &);
  friend bool operator>=(const BigInt &, const UBigInt &);
  friend bool operator==(const BigInt &, const UBigInt &);
  friend bool operator!=(const BigInt &, const UBigInt &);
  friend bool operator<(const BigInt &, int64_t);
  friend bool operator>(const BigInt &, int64_t);
  friend bool operator<=(const BigInt &, int64_t);
  friend bool operator>=(const BigInt &, int64_t);
  friend bool operator==(const BigInt &, int64_t);
  friend bool operator!=(const BigInt &, int64_t);
  friend bool operator<(const UBigInt &, const BigInt &);
  friend bool operator>(const UBigInt &, const BigInt &);
  friend bool operator<=(const UBigInt &, const BigInt &);
  friend bool operator>=(const UBigInt &, const BigInt &);
  friend bool operator==(const UBigInt &, const BigInt &);
  friend bool operator!=(const UBigInt &, const BigInt &);
  friend bool operator<(int64_t, const BigInt &);
  friend bool operator>(int64_t, const BigInt &);
  friend bool operator<=(int64_t, const BigInt &);
  friend bool operator>=(int64_t, const BigInt &);
  friend bool operator==(int64_t, const BigInt &);
  friend bool operator!=(int64_t, const BigInt &);

  friend BigInt &operator<<=(BigInt &, size_t);
  friend BigInt &operator>>=(BigInt &, size_t);
  friend BigInt operator<<(const BigInt &, size_t);
  friend BigInt operator>>(const BigInt &, size_t);

  explicit operator int64_t() const;
  const UBigInt &Abs() const noexcept;

  friend std::ostream &operator<<(std::ostream &, const BigInt &);
  friend std::istream &operator>>(std::istream &, BigInt &);
  std::string Exact() const;

  ~BigInt() = default;

private:
  UBigInt abs_;
  bool pos_;
};