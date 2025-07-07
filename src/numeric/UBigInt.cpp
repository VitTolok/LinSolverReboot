#include "UBigInt.hpp"
#include <limits>

UBigInt::UBigInt() : storage_() { Shrink(); }
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
  first.storage_.resize(std::max(first.storage_.size(), second.storage_.size()),
                        0u);
  u_int32_t carryover = 0u;
  for (size_t i = 0; i < second.storage_.size(); ++i) {
    u_int64_t sum = static_cast<u_int64_t>(first.storage_[i]) +
                    static_cast<u_int64_t>(second.storage_[i]) +
                    static_cast<u_int64_t>(carryover);
    first.storage_[i] = static_cast<u_int32_t>(sum % (1ull << 32));
    carryover = static_cast<u_int32_t>(sum >> 32);
  }
  if (carryover > 0u) {
    for (size_t i = second.storage_.size(); carryover > 0u; ++i) {
      while (i >= first.storage_.size()) {
        first.storage_.push_back(0u);
      }
      u_int64_t sum = static_cast<u_int64_t>(first.storage_[i]) +
                      static_cast<u_int64_t>(carryover);
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
    throw UBigIntOverflow();
  }
  u_int32_t debt = 0u;
  for (size_t i = 0; i < second.storage_.size(); ++i) {
    u_int64_t available = static_cast<u_int64_t>(first.storage_[i]);
    u_int64_t subtracted = static_cast<u_int64_t>(second.storage_[i]) +
                           static_cast<u_int64_t>(debt);
    if (available >= subtracted) {
      first.storage_[i] -= second.storage_[i] + debt;
      debt = 0u;
      continue;
    }
    debt = 1u;
    first.storage_[i] =
        static_cast<u_int32_t>((available + (1ull << 32)) - subtracted);
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
    buffer.storage_.pop_front();
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
  u_int64_t upper = (1ull << 32);
  u_int64_t lower = 1ull;
  while (upper - lower > 1) {
    u_int64_t mid = lower + (upper - lower) / 2;
    UBigInt prod = div * mid;
    if (remainder < prod) {
      upper = mid;
      continue;
    }
    if (remainder > prod) {
      lower = mid;
      continue;
    }
    remainder = 0;
    return static_cast<u_int32_t>(mid);
  }
  remainder -= div * lower;
  return static_cast<u_int32_t>(lower);
}

UBigInt &operator/=(UBigInt &first, const UBigInt &second) {
  if (&first == &second || first == second) {
    return first = 1;
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
    remainder.storage_.push_front(
        first.storage_[first.storage_.size() - i - 1]);
    first.storage_[first.storage_.size() - i - 1] =
        InPlaceBigDiv(remainder, second);
  }
  first.Shrink();
  return first;
}

UBigInt &operator%=(UBigInt &first, const UBigInt &second) {
  if (&first == &second || first == second) {
    return first = 0;
  }
  if (second < (1ull << 32)) {
    return first %= static_cast<u_int32_t>(second);
  }
  UBigInt remainder;
  for (size_t i = 0; i < first.storage_.size(); ++i) {
    remainder.storage_.push_front(
        first.storage_[first.storage_.size() - i - 1]);
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
  return copy += second;
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
      u_int64_t sum = static_cast<u_int64_t>(bigint.storage_[pos]) +
                      static_cast<u_int64_t>(carryover);
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
    throw UBigIntOverflow();
  }
  if (number < (1ull << 32)) {
    u_int32_t debt = static_cast<u_int32_t>(number);
    for (size_t pos = 0;; ++pos) {
      if (bigint.storage_[pos] >= debt) {
        bigint.storage_[pos] -= debt;
        break;
      }
      bigint.storage_[pos] =
          static_cast<u_int32_t>(((1ull << 32) + bigint.storage_[pos]) - debt);
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
      u_int64_t prod = static_cast<u_int64_t>(bigint.storage_[pos]) * number +
                       static_cast<u_int64_t>(carryover);
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
    throw UBigIntDivisionByZero();
  }
  if (div == 1 || bigint == 0) {
    return bigint;
  }
  if (div < (1ull << 32)) {
    u_int32_t remainder = 0;
    for (size_t i = 0; i < bigint.storage_.size(); ++i) {
      u_int64_t num = static_cast<u_int64_t>(
                          bigint.storage_[bigint.storage_.size() - i - 1]) +
                      (static_cast<u_int64_t>(remainder) << 32);
      bigint.storage_[bigint.storage_.size() - i - 1] =
          static_cast<u_int32_t>(num / div);
      remainder = static_cast<u_int32_t>(num % div);
    }
    bigint.Shrink();
    return bigint;
  }
  return bigint /= UBigInt(div);
}
UBigInt &operator%=(UBigInt &bigint, u_int64_t div) {
  if (div == 0) {
    throw UBigIntDivisionByZero();
  }
  if (div == 1 || bigint == 0) {
    return bigint = 0;
  }
  if (div < (1ull << 32)) {
    u_int32_t remainder = 0;
    for (size_t i = 0; i < bigint.storage_.size(); ++i) {
      u_int64_t num = static_cast<u_int64_t>(
                          bigint.storage_[bigint.storage_.size() - i - 1]) +
                      (static_cast<u_int64_t>(remainder) << 32);
      remainder = static_cast<u_int32_t>(num % div);
    }
    return bigint = remainder;
  }
  return bigint /= UBigInt(div);
}

UBigInt operator+(const UBigInt &bigint, u_int64_t number) {
  UBigInt copy = bigint;
  return copy += number;
}
UBigInt operator-(const UBigInt &bigint, u_int64_t number) {
  UBigInt copy = bigint;
  return copy += number;
}
UBigInt operator*(const UBigInt &bigint, u_int64_t number) {
  UBigInt copy = bigint;
  return copy += number;
}
UBigInt operator/(const UBigInt &bigint, u_int64_t number) {
  UBigInt copy = bigint;
  return copy += number;
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
    return false;
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
  return (static_cast<u_int64_t>(storage_[1]) << 32) +
         static_cast<u_int64_t>(storage_[0]);
}

UBigInt::operator u_int32_t() const {
  if (storage_.size() == 0) {
    return 0;
  }
  return storage_[0];
}

std::ostream &operator<<(std::ostream &out, const UBigInt &bigint) {
  if (bigint <= 1'000'000) {
    return out << static_cast<u_int32_t>(bigint);
  }
  size_t power_count = 32 * (bigint.storage_.size());
  u_int32_t greatest = bigint.storage_.back();
  while (greatest < (1u << 31)) {
    greatest <<= 1;
    --power_count;
  }
  power_count = static_cast<double>(power_count) / 3.321928094887362 + 1;
  size_t count = 0;
  UBigInt div(1);
  while (power_count - count >= 9) {
    div *= 1'000'000'000u;
    count += 9;
  }
  while (power_count > count) {
    div *= 10;
    ++count;
  }
  while (bigint < div) {
    div /= 10;
    --count;
  }
  div /= 1'000'000;
  u_int32_t simplified = static_cast<u_int32_t>(bigint / div);
  div = 0;
  div.Shrink();
  if (simplified % 10 >= 5) {
    simplified += 5;
  }
  simplified /= 10;
  return out << simplified / 100'000 << '.' << simplified % 100'000 << 'e'
             << count;
}

void UBigInt::Shrink() {
  while (!storage_.empty() && storage_.back() == 0u) {
    storage_.pop_back();
  }
  if (storage_.max_size() > (storage_.size() * 8)) {
    storage_.shrink_to_fit();
  }
}