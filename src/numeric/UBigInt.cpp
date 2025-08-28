#include "UBigInt.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <map>
#include <sstream>
#include <vector>
#include <numeric>

UBigInt::UBigInt() : storage_() {
  Shrink();
}
UBigInt::UBigInt(u_int64_t number) : storage_(2, 0u) {
  storage_[0] = static_cast<u_int32_t>(number % (1ull << 32));
  storage_[1] = static_cast<u_int32_t>(number >> 32);
  Shrink();
}

UBigInt &UBigInt::operator=(u_int64_t number) {
  storage_.resize(2, 0);
  storage_[0] = static_cast<u_int32_t>(number % (1ull << 32));
  storage_[1] = static_cast<u_int32_t>(number >> 32);
  Shrink();
  return *this;
}

UBigInt &operator+=(UBigInt &first, const UBigInt &second) {
  if (first == 0) {
    return first = second;
  }
  first.storage_.resize(std::max(first.storage_.size(), second.storage_.size()), 0u);
  u_int32_t carryover = 0u;
  for (size_t i = 0; i < second.storage_.size(); ++i) {
    u_int64_t sum = static_cast<u_int64_t>(first.storage_[i]) + static_cast<u_int64_t>(second.storage_[i]) +
                    static_cast<u_int64_t>(carryover);
    first.storage_[i] = static_cast<u_int32_t>(sum % (1ull << 32));
    carryover = static_cast<u_int32_t>(sum >> 32);
  }
  if (carryover > 0u) {
    for (size_t i = second.storage_.size(); carryover > 0u; ++i) {
      while (i >= first.storage_.size()) {
        first.storage_.push_back(0u);
      }
      u_int64_t sum = static_cast<u_int64_t>(first.storage_[i]) + static_cast<u_int64_t>(carryover);
      first.storage_[i] = static_cast<u_int32_t>(sum % (1ull << 32));
      carryover = static_cast<u_int32_t>(sum >> 32);
    }
  }
  first.Shrink();
  return first;
}

UBigInt &operator-=(UBigInt &first, const UBigInt &second) {
  if (&first == &second || first == second) {
    return first = 0;
  }
  if (first < second) {
    throw UBigIntUnderflow();
  }
  if (first <= std::numeric_limits<u_int64_t>::max() && second <= std::numeric_limits<u_int64_t>::max()) {
    return first = static_cast<u_int64_t>(first) - static_cast<u_int64_t>(second);
  }
  u_int32_t debt = 0u;
  for (size_t i = 0; i < second.storage_.size(); ++i) {
    u_int64_t available = static_cast<u_int64_t>(first.storage_[i]);
    u_int64_t subtracted = static_cast<u_int64_t>(second.storage_[i]) + static_cast<u_int64_t>(debt);
    if (available >= subtracted) {
      first.storage_[i] -= second.storage_[i] + debt;
      debt = 0u;
      continue;
    }
    debt = 1u;
    first.storage_[i] = static_cast<u_int32_t>((available + (1ull << 32)) - subtracted);
  }
  for (size_t i = second.storage_.size(); debt > 0 && i < first.storage_.size(); i++) {
    if (first.storage_[i] >= debt) {
      first.storage_[i] -= debt;
      break;
    }
    first.storage_[i] = (1ull << 32) + static_cast<u_int64_t>(first.storage_[i]) - debt;
  }
  first.Shrink();
  return first;
}

UBigInt &operator*=(UBigInt &first, const UBigInt &second) {
  if (&first == &second) {
    UBigInt copy = second;
    return first *= copy;
  }
  if (second <= std::numeric_limits<u_int64_t>::max()) {
    return first *= static_cast<u_int64_t>(second);
  }
  UBigInt buffer(0);
  for (size_t i = 0; i < first.storage_.size(); ++i) {
    buffer += second * first.storage_[i];
    first.storage_[i] = buffer.storage_[0];
    if (!buffer.storage_.empty()) {
      buffer.storage_.pop_front();
    }
  }
  for (size_t i = 0; i < buffer.storage_.size(); ++i) {
    first.storage_.push_back(buffer.storage_[i]);
  }
  first.Shrink();
  return first;
}

u_int32_t InPlaceBigDiv(UBigInt &remainder, const UBigInt &div) {
  if (remainder < div) {
    return 0;
  }
  if (remainder == div) {
    remainder = 0;
    return 1;
  }
  auto rem_approx = remainder.GetApproximation();
  auto div_approx = div.GetApproximation();
  u_int32_t result = rem_approx.multiplier / div_approx.multiplier * powl(10, rem_approx.power - div_approx.power);
  remainder -= (div * result);
  if (remainder >= div) {
    ++result;
    remainder -= div;
  }
  return result;
}

UBigInt &operator/=(UBigInt &first, const UBigInt &second) {
  if (&first == &second || first == second) {
    return first = 1;
  }
  if (first <= std::numeric_limits<u_int64_t>::max() && second <= std::numeric_limits<u_int64_t>::max()) {
    return first = static_cast<u_int64_t>(first) / static_cast<u_int64_t>(second);
  }
  if (second < (1ull << 32)) {
    return first /= static_cast<u_int32_t>(second);
  }
  if (first < second) {
    first = 0;
    return first;
  }
  UBigInt remainder;
  for (size_t i = 0; i < first.storage_.size(); ++i) {
    remainder.storage_.push_front(first.storage_[first.storage_.size() - i - 1]);
    first.storage_[first.storage_.size() - i - 1] = InPlaceBigDiv(remainder, second);
  }
  first.Shrink();
  return first;
}

UBigInt &operator%=(UBigInt &first, const UBigInt &second) {
  if (&first == &second || first == second) {
    return first = 0;
  }
  if (first <= std::numeric_limits<u_int64_t>::max() && second <= std::numeric_limits<u_int64_t>::max()) {
    return first = static_cast<u_int64_t>(first) % static_cast<u_int64_t>(second);
  }
  if (second < (1ull << 32)) {
    return first %= static_cast<u_int32_t>(second);
  }
  UBigInt remainder;
  for (size_t i = 0; i < first.storage_.size(); ++i) {
    remainder.storage_.push_front(first.storage_[first.storage_.size() - i - 1]);
    InPlaceBigDiv(remainder, second);
  }
  return first = remainder;
}

UBigInt operator+(const UBigInt &first, const UBigInt &second) {
  UBigInt copy = first;
  return copy += second;
}
UBigInt operator-(const UBigInt &first, const UBigInt &second) {
  if (&first == &second || first == second) {
    return UBigInt(0);
  }
  UBigInt copy = first;
  return copy -= second;
}
UBigInt operator*(const UBigInt &first, const UBigInt &second) {
  UBigInt copy = first;
  return copy *= second;
}
UBigInt operator/(const UBigInt &first, const UBigInt &second) {
  if (&first == &second || first == second) {
    return UBigInt(1);
  }
  UBigInt copy = first;
  return copy /= second;
}
UBigInt operator%(const UBigInt &first, const UBigInt &second) {
  if (&first == &second || first == second) {
    return UBigInt(0);
  }
  UBigInt copy = first;
  return copy %= second;
}

UBigInt &operator+=(UBigInt &bigint, u_int64_t number) {
  if (bigint == 0) {
    return bigint = number;
  }
  if (number < (1ull << 32)) {
    u_int32_t carryover = static_cast<u_int32_t>(number);
    for (size_t pos = 0; pos < bigint.storage_.size() && carryover > 0; ++pos) {
      u_int64_t sum = static_cast<u_int64_t>(bigint.storage_[pos]) + static_cast<u_int64_t>(carryover);
      bigint.storage_[pos] = static_cast<u_int32_t>(sum % (1ull << 32));
      carryover = static_cast<u_int32_t>(sum >> 32);
    }
    if (carryover > 0) {
      bigint.storage_.push_back(carryover);
    }
    return bigint;
  }
  u_int32_t head = bigint.storage_[0];
  bigint.storage_.pop_front();
  bigint += (number >> 32);
  bigint.storage_.push_front(head);
  return bigint += (number % (1ull << 32));
}

UBigInt &operator-=(UBigInt &bigint, u_int64_t number) {
  if (number == 0) {
    return bigint;
  }
  if (bigint < number) {
    throw UBigIntUnderflow();
  }
  if (bigint <= std::numeric_limits<u_int64_t>::max()) {
    return bigint = static_cast<u_int64_t>(bigint) - number;
  }
  if (number < (1ull << 32)) {
    u_int32_t debt = static_cast<u_int32_t>(number);
    for (size_t pos = 0;; ++pos) {
      if (bigint.storage_[pos] >= debt) {
        bigint.storage_[pos] -= debt;
        break;
      }
      bigint.storage_[pos] = static_cast<u_int32_t>(((1ull << 32) + bigint.storage_[pos]) - debt);
      debt = 1u;
    }
    bigint.Shrink();
    return bigint;
  }
  u_int32_t head = bigint.storage_[0];
  bigint.storage_.pop_front();
  bigint -= (number >> 32);
  bigint.storage_.push_front(head);
  return bigint -= (number % (1ull << 32));
}

UBigInt &operator*=(UBigInt &bigint, u_int64_t number) {
  if (bigint == 0 || number == 1) {
    return bigint;
  }
  if (number == 0) {
    return bigint = 0;
  }
  if (number < (1ull << 32)) {
    u_int32_t carryover = 0;
    for (size_t pos = 0; pos < bigint.storage_.size(); ++pos) {
      u_int64_t prod = static_cast<u_int64_t>(bigint.storage_[pos]) * number + static_cast<u_int64_t>(carryover);
      bigint.storage_[pos] = static_cast<u_int32_t>(prod % (1ull << 32));
      carryover = static_cast<u_int32_t>(prod >> 32);
    }
    if (carryover > 0) {
      bigint.storage_.push_back(carryover);
    }
    return bigint;
  }
  {
    UBigInt copy = bigint;
    bigint.storage_.push_front(0);
    bigint *= (number >> 32);
    return bigint += (copy *= (number % (1ull << 32)));
  }
}

UBigInt &operator/=(UBigInt &bigint, u_int64_t div) {
  if (div == 0) {
    throw BigIntDivisionByZero();
  }
  if (div == 1 || bigint == 0) {
    return bigint;
  }
  if (bigint <= std::numeric_limits<u_int64_t>::max()) {
    return bigint = static_cast<u_int64_t>(bigint) / div;
  }
  if (div < (1ull << 32)) {
    u_int32_t remainder = 0;
    for (size_t i = 0; i < bigint.storage_.size(); ++i) {
      u_int64_t num = static_cast<u_int64_t>(bigint.storage_[bigint.storage_.size() - i - 1]) +
                      (static_cast<u_int64_t>(remainder) << 32);
      bigint.storage_[bigint.storage_.size() - i - 1] = static_cast<u_int32_t>(num / div);
      remainder = static_cast<u_int32_t>(num % div);
    }
    bigint.Shrink();
    return bigint;
  }
  return bigint /= UBigInt(div);
}
UBigInt &operator%=(UBigInt &bigint, u_int64_t div) {
  if (div == 0) {
    throw BigIntDivisionByZero();
  }
  if (div == 1 || bigint == 0) {
    return bigint = 0;
  }
  if (bigint <= std::numeric_limits<u_int64_t>::max()) {
    return bigint = static_cast<u_int64_t>(bigint) % div;
  }
  if (div < (1ull << 32)) {
    u_int32_t remainder = 0;
    for (size_t i = 0; i < bigint.storage_.size(); ++i) {
      u_int64_t num = static_cast<u_int64_t>(bigint.storage_[bigint.storage_.size() - i - 1]) +
                      (static_cast<u_int64_t>(remainder) << 32);
      remainder = static_cast<u_int32_t>(num % div);
    }
    return bigint = remainder;
  }
  return bigint %= UBigInt(div);
}

UBigInt operator+(const UBigInt &bigint, u_int64_t number) {
  UBigInt copy = bigint;
  return copy += number;
}
UBigInt operator-(const UBigInt &bigint, u_int64_t number) {
  UBigInt copy = bigint;
  return copy -= number;
}
UBigInt operator*(const UBigInt &bigint, u_int64_t number) {
  UBigInt copy = bigint;
  return copy *= number;
}
UBigInt operator/(const UBigInt &bigint, u_int64_t number) {
  UBigInt copy = bigint;
  return copy /= number;
}
UBigInt operator%(const UBigInt &bigint, u_int64_t div) {
  UBigInt copy = bigint;
  return copy %= div;
}

bool operator<(const UBigInt &first, const UBigInt &second) {
  if (&first == &second) {
    return false;
  }
  if (first.storage_.size() != second.storage_.size()) {
    return first.storage_.size() < second.storage_.size();
  }
  size_t size = first.storage_.size();
  for (size_t i = 0; i < first.storage_.size(); ++i) {
    if (first.storage_[size - i - 1] != second.storage_[size - i - 1]) {
      return first.storage_[size - i - 1] < second.storage_[size - i - 1];
    }
  }
  return false;
}
bool operator>(const UBigInt &first, const UBigInt &second) {
  return second < first;
}
bool operator<=(const UBigInt &first, const UBigInt &second) {
  return !(second < first);
}
bool operator>=(const UBigInt &first, const UBigInt &second) {
  return !(first < second);
}
bool operator==(const UBigInt &first, const UBigInt &second) {
  if (&first == &second) {
    return true;
  }
  if (first.storage_.size() != second.storage_.size()) {
    return false;
  }
  size_t size = first.storage_.size();
  for (size_t i = 0; i < first.storage_.size(); ++i) {
    if (first.storage_[size - i - 1] != second.storage_[size - i - 1]) {
      return false;
    }
  }
  return true;
}
bool operator!=(const UBigInt &first, const UBigInt &second) {
  return !(first == second);
}

bool operator<(const UBigInt &bigint, u_int64_t number) {
  if (bigint.storage_.size() > 2) {
    return false;
  }
  return static_cast<u_int64_t>(bigint) < number;
}
bool operator>(const UBigInt &bigint, u_int64_t number) {
  return number < bigint;
}
bool operator<=(const UBigInt &bigint, u_int64_t number) {
  return !(number < bigint);
}
bool operator>=(const UBigInt &bigint, u_int64_t number) {
  return !(bigint < number);
}
bool operator==(const UBigInt &bigint, u_int64_t number) {
  if (bigint.storage_.size() > 2) {
    return false;
  }
  return static_cast<u_int64_t>(bigint) == number;
}
bool operator!=(const UBigInt &bigint, u_int64_t number) {
  return !(bigint == number);
}

bool operator<(u_int64_t number, const UBigInt &bigint) {
  if (bigint.storage_.size() > 2) {
    return true;
  }
  return number < static_cast<u_int64_t>(bigint);
}
bool operator>(u_int64_t number, const UBigInt &bigint) {
  return bigint < number;
}
bool operator<=(u_int64_t number, const UBigInt &bigint) {
  return !(bigint < number);
}
bool operator>=(u_int64_t number, const UBigInt &bigint) {
  return !(number < bigint);
}
bool operator==(u_int64_t number, const UBigInt &bigint) {
  return bigint == number;
}
bool operator!=(u_int64_t number, const UBigInt &bigint) {
  return !(bigint == number);
}

UBigInt &operator<<=(UBigInt &bigint, size_t shift) {
  if (bigint == 0) {
    return bigint;
  }
  while (shift >= 32) {
    bigint.storage_.push_front(0);
    shift -= 32;
  }
  if (shift > 0) {
    bigint *= (1ull << shift);
  }
  return bigint;
}
UBigInt &operator>>=(UBigInt &bigint, size_t shift) {
  if (bigint == 0) {
    return bigint;
  }
  while (shift >= 32 && !bigint.storage_.empty()) {
    bigint.storage_.pop_front();
    shift -= 32;
  }
  if (shift > 0) {
    bigint /= (1ull << shift);
  }
  return bigint;
}
UBigInt operator<<(const UBigInt &bigint, size_t shift) {
  UBigInt copy = bigint;
  return copy <<= shift;
}
UBigInt operator>>(const UBigInt &bigint, size_t shift) {
  UBigInt copy = bigint;
  return copy >>= shift;
}

UBigInt::operator u_int64_t() const {
  if (storage_.size() == 0) {
    return 0;
  }
  if (storage_.size() == 1) {
    return storage_[0];
  }
  return (static_cast<u_int64_t>(storage_[1]) << 32) + static_cast<u_int64_t>(storage_[0]);
}

UBigInt::operator u_int32_t() const {
  if (storage_.size() == 0) {
    return 0;
  }
  return storage_[0];
}

UBigInt Power(const UBigInt &base, size_t power) {
  switch (power) {
    case 0:
      return UBigInt(1);
    case 1:
      return base;
    case 2:
      return base * base;
  }
  static std::map<UBigInt, std::vector<UBigInt>> powers;
  auto location = powers.emplace(base, std::vector<UBigInt>(1, base)).first;
  while ((1ull << location->second.size()) <= power) {
    location->second.push_back(location->second.back() * location->second.back());
  }
  UBigInt result(1);
  for (size_t i = 0; i < location->second.size(); ++i) {
    if (power >= (1ull << (location->second.size() - i - 1))) {
      result *= location->second[location->second.size() - i - 1];
      power -= (1ull << (location->second.size() - i - 1));
    }
  }
  return result;
}

UBigInt GCD(const UBigInt &first, const UBigInt &second) {
  if (first == second) {
    return first;
  }
  if (first == 1 || second == 1) {
    return UBigInt(1);
  }
  if (first == 0) {
    return second;
  }
  if (second == 0) {
    return first;
  }
  if (first <= std::numeric_limits<u_int64_t>::max() && second <= std::numeric_limits<u_int64_t>::max()) {
    return UBigInt(std::gcd(static_cast<u_int64_t>(first), static_cast<u_int64_t>(second)));
  }
  UBigInt f = first;
  UBigInt s = second;
  size_t power_of_2 = 0;
  while (f != 0 && s != 0) {
    auto f_mod2 = static_cast<u_int32_t>(f % 2);
    auto s_mod2 = static_cast<u_int32_t>(s % 2);
    if ((f_mod2 == 0) && (s_mod2 == 0)) {
      power_of_2++;
      f >>= 1;
      s >>= 1;
    } else if (f_mod2 == 0) {
      f >>= 1;
    } else if (s_mod2 == 0) {
      s >>= 1;
    } else {
      const_cast<UBigInt &>(std::max(f, s)) -= std::min(f, s);
    }
  }
  return std::max(f, s) * Power(UBigInt(2), power_of_2);
}

std::ostream &operator<<(std::ostream &out, const UBigInt &bigint) {
  if (bigint < 1'000'000) {
    return out << static_cast<u_int32_t>(bigint);
  }
  auto approx = bigint.GetApproximation();
  if (approx.multiplier >= 10 - 5e-6) {
    approx.multiplier = 1;
    ++approx.power;
  }
  return out << approx.multiplier << 'e' << approx.power;
}

std::istream &operator>>(std::istream &in, UBigInt &bigint) {
  std::string input;
  in >> input;
  size_t power = 0;
  if (input.find('e') != std::string::npos) {
    size_t sep = input.find('e');
    try {
      std::istringstream(input.c_str() + sep + 1) >> power;
    } catch (...) {
      throw BigIntInvalidString();
    }
    input.resize(sep);
    size_t dot = input.find('.');
    if (dot != std::string::npos) {
      input.erase(dot, 1);
      if (sep - dot - 1 > power) {
        if (input.size() > dot + power) {
          if (input[dot + power] - '0' >= 5) {
            int64_t pos = dot + power - 1;
            while (pos >= 0 && input[pos] == '9') {
              input[pos] = 0;
              --pos;
            }
            if (pos == -1) {
              input.insert(0, 1, '1');
            } else {
              input[pos]++;
            }
          }
        }
        input.resize(dot + power);
        power = 0;
      } else {
        power -= (sep - dot - 1);
      }
    }
  }
  bigint = 0;
  size_t pos = 0;
  while (input.size() > pos + 9) {
    u_int32_t buffer = 0;
    std::istringstream(input.substr(pos, 9)) >> buffer;
    bigint *= 1'000'000'000;
    bigint += buffer;
    pos += 9;
  }
  if (input.size() > pos) {
    u_int32_t buffer = 0;
    std::istringstream(input.substr(pos)) >> buffer;
    while (input.size() > pos) {
      bigint *= 10;
      pos++;
    }
    bigint += buffer;
  }
  bigint *= Power(UBigInt(10), power);
  return in;
}

std::string UBigInt::Exact() const {
  std::string buffer;
  UBigInt copy = *this;
  while (copy > 0) {
    buffer.push_back('0' + static_cast<u_int32_t>(copy % 10));
    copy /= 10;
  }
  std::reverse(buffer.begin(), buffer.end());
  return buffer;
}

void UBigInt::Shrink() noexcept {
  while (!storage_.empty() && storage_.back() == 0u) {
    storage_.pop_back();
  }
  if (storage_.max_size() > (storage_.size() * 8)) {
    storage_.shrink_to_fit();
  }
}

UBigInt::Approximation UBigInt::GetApproximation() const {
  if (storage_.size() == 0) {
    return {0, 0};
  }
  static const long double kPowerRatio = log2f64x(10);
  size_t power_of_2 = 32 * (storage_.size());
  u_int32_t greatest = storage_.back();
  while (greatest < (1u << 31)) {
    greatest <<= 1;
    --power_of_2;
  }
  --power_of_2;
  const long double raw = static_cast<long double>(power_of_2) / kPowerRatio;
  size_t power_of_10 = static_cast<size_t>(raw);
  long double mult = powl(10, raw - static_cast<long double>(power_of_10));
  if (storage_.size() == 1) {
    mult *= static_cast<long double>(storage_[0]) / static_cast<long double>(1ull << power_of_2);
  } else if (storage_.size() == 2) {
    mult *= static_cast<long double>((static_cast<u_int64_t>(storage_[1]) << 32) + storage_[0]) /
            static_cast<long double>(1ull << power_of_2);
  } else {
    size_t shift = 31 - (power_of_2 % 32);
    mult *= static_cast<long double>((static_cast<u_int64_t>(storage_.back()) << (32 + shift)) +
                                     (static_cast<u_int64_t>(storage_[storage_.size() - 2]) << shift) +
                                     static_cast<u_int64_t>(storage_[storage_.size() - 3] >> (32 - shift))) /
            static_cast<long double>(1ull << 63);
  }
  if (mult >= 10 - 5e-12) {
    ++power_of_10;
    mult /= 10;
  }
  return {mult, power_of_10};
}