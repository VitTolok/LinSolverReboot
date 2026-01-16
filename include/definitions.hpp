#pragma once
#include <charconv>
#include <array>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <memory>
#include <utility>

class MachineError : public std::exception {
 public:
  MachineError() : message_ptr(kMessage) {
  }

  [[nodiscard]] const char *what() const noexcept final {
    return message_ptr;
  }

  virtual std::unique_ptr<MachineError> Detach() {
    return std::make_unique<MachineError>(std::move<MachineError &>(*this));
  }

  ~MachineError() override = default;

 protected:
  const char *message_ptr;

 private:
  inline static constexpr char kMessage[] = "Unknown Machine Error";
};

template <typename T>
  requires requires(const T &v) {
    v < 0;
    -v;
  }
constexpr T Abs(const T &value) {
  if (value < 0) {
    return -value;
  }
  return value;
}

consteval auto PowersOf10() noexcept {
  std::array<uint64_t, 20> powers;
  powers[0] = 1;
  for (size_t i = 1; i < 20; i++) {
    powers[i] = powers[i - 1] * 10;
  }
  return powers;
}

constexpr inline uint64_t TenToIntPower(uint64_t power) noexcept {
  constexpr static auto kPowers = PowersOf10();
  return kPowers[power];
}

constexpr char *DoublePrinter(char *const start, const size_t length, const long double number) noexcept {
  char *const end = start + length;
  auto whole = Abs(static_cast<int64_t>(number));
  if (length == 0) {
    return nullptr;
  }
  char *const whole_begin = start + (number < 0 ? 1 : 0);
  if (number < 0) {
    *start = '-';
  }
  char *whole_end = nullptr;
  int64_t decimal_length = 0;
  {
    auto [whole_end_ptr, errc] = std::to_chars(whole_begin, end, whole);
    if (errc != std::errc{}) {
      return nullptr;
    }
    whole_end = whole_end_ptr;
    decimal_length = end - whole_end_ptr - 1;
  }
  auto decimal = Abs(number - whole);
  {
    long double margin = 0.5L;
    for (int64_t i = 0; i < decimal_length; i++) {
      margin /= 10;
    }
    if (decimal + margin >= 1) {
      whole++;
      decimal -= 1;
    }
    auto [whole_end_ptr, errc] = std::to_chars(whole_begin, end, whole);
    if (errc != std::errc{}) {
      return nullptr;
    }
    if (whole_end_ptr != whole_end) {
      decimal = decimal + margin * 10;
    } else {
      decimal += margin;
    }
    whole_end = whole_end_ptr;
    decimal_length = end - whole_end_ptr - 1;
  }
  if (decimal_length <= 0) {
    return whole_end;
  }
  *whole_end = '.';
  char *ptr = whole_end + 1;
  while (ptr < end) {
    decimal *= 10;
    *ptr = static_cast<char>('0' + static_cast<char>(decimal));
    decimal -= static_cast<char>(decimal);
    ptr++;
  }
  ptr--;
  while (*ptr == '0') {
    *ptr = '\0';
    ptr--;
  }
  if (*ptr == '.') {
    *ptr = '\0';
    ptr--;
  }
  return ptr + 1;
}

consteval void ConstAssert(bool expr) {
  if (!expr) {
    std::unreachable();
  }
}
consteval void ConstAssertEQ(const auto &first, const auto &second) {
  ConstAssert(first == second);
}
consteval void ConstAssertLT(const auto &first, const auto &second) {
  ConstAssert(first < second);
}

template <class A, class B = A>
concept Addable = requires(std::decay_t<A> a, std::decay_t<B> b) { a + b; };
template <class A, class B = A>
concept Subtractable = requires(std::decay_t<A> a, std::decay_t<B> b) { a - b; };
template <class A, class B = A>
concept Multipliable = requires(std::decay_t<A> a, std::decay_t<B> b) { a * b; };
template <class A, class B = A>
concept Divisible = requires(std::decay_t<A> a, std::decay_t<B> b) { a / b; };
template <class A, class B = A>
concept Moddable = requires(std::decay_t<A> a, std::decay_t<B> b) { a % b; };

template <class A, class B = A>
concept CompoundAddable = requires(std::decay_t<A> &a, std::decay_t<B> b) { a += b; };
template <class A, class B = A>
concept CompoundSubtractable = requires(std::decay_t<A> &a, std::decay_t<B> b) { a -= b; };
template <class A, class B = A>
concept CompoundMultipliable = requires(std::decay_t<A> &a, std::decay_t<B> b) { a *= b; };
template <class A, class B = A>
concept CompoundDivisible = requires(std::decay_t<A> &a, std::decay_t<B> b) { a /= b; };
template <class A, class B = A>
concept CompoundModdable = requires(std::decay_t<A> &a, std::decay_t<B> b) { a %= b; };

template <class A, class B = A>
concept NoexceptAddable = requires {
  requires Addable<A, B>;
  requires noexcept(std::declval<std::decay_t<A>>() + std::declval<std::decay_t<B>>());
};
template <class A, class B = A>
concept NoexceptSubtractable = requires {
  requires Subtractable<A, B>;
  requires noexcept(std::declval<std::decay_t<A>>() - std::declval<std::decay_t<B>>());
};
template <class A, class B = A>
concept NoexceptMultipliable = requires {
  requires Multipliable<A, B>;
  requires noexcept(std::declval<std::decay_t<A>>() * std::declval<std::decay_t<B>>());
};
template <class A, class B = A>
concept NoexceptDivisible = requires {
  requires Divisible<A, B>;
  requires noexcept(std::declval<std::decay_t<A>>() / std::declval<std::decay_t<B>>());
};
template <class A, class B = A>
concept NoexceptModdable = requires {
  requires Moddable<A, B>;
  requires noexcept(std::declval<std::decay_t<A>>() % std::declval<std::decay_t<B>>());
};

template <class A, class B = A>
concept NoexceptCompoundAddable = requires {
  requires CompoundAddable<A, B>;
  requires noexcept(std::declval<std::decay_t<A> &>() += std::declval<std::decay_t<B>>());
};
template <class A, class B = A>
concept NoexceptCompoundSubtractable = requires {
  requires CompoundSubtractable<A, B>;
  requires noexcept(std::declval<std::decay_t<A> &>() -= std::declval<std::decay_t<B>>());
};
template <class A, class B = A>
concept NoexceptCompoundMultipliable = requires {
  requires CompoundMultipliable<A, B>;
  requires noexcept(std::declval<std::decay_t<A> &>() *= std::declval<std::decay_t<B>>());
};
template <class A, class B = A>
concept NoexceptCompoundDivisible = requires {
  requires CompoundDivisible<A, B>;
  requires noexcept(std::declval<std::decay_t<A> &>() /= std::declval<std::decay_t<B>>());
};
template <class A, class B = A>
concept NoexceptCompoundModdable = requires {
  requires CompoundModdable<A, B>;
  requires noexcept(std::declval<std::decay_t<A> &>() %= std::declval<std::decay_t<B>>());
};

template <class A, class B = A>
concept Comparable = requires(const std::decay_t<A> &a, const std::decay_t<B> &b) { a == b; };
template <class A, class B = A>
concept NoexceptComparable = requires {
  requires Comparable<A, B>;
  requires noexcept(std::declval<const std::decay_t<A> &>() == std::declval<const std::decay_t<B> &>());
};

template <class A, class B = A>
using SumType = decltype(std::declval<std::decay_t<A>>() + std::declval<std::decay_t<B>>());
template <class A, class B = A>
using SubType = decltype(std::declval<std::decay_t<A>>() - std::declval<std::decay_t<B>>());
template <class A, class B = A>
using MulType = decltype(std::declval<std::decay_t<A>>() * std::declval<std::decay_t<B>>());
template <class A, class B = A>
using DivType = decltype(std::declval<std::decay_t<A>>() / std::declval<std::decay_t<B>>());
template <class A, class B = A>
using ModType = decltype(std::declval<std::decay_t<A>>() % std::declval<std::decay_t<B>>());

template <class A, class B = A>
concept Convertible = requires(std::decay_t<B> b) { static_cast<std::decay_t<A>>(b); };
template <class A, class B = A>
concept NoexceptConvertible = requires {
  requires Convertible<A, B>;
  requires noexcept(static_cast<std::decay_t<A>>(std::declval<std::decay_t<B>>()));
};

template <class T>
concept Movable = requires(std::decay_t<T> &a, std::decay_t<T> &b) { a = std::move(b); };
template <class T>
concept NoexceptMovable = requires {
  requires Movable<T>;
  requires noexcept(std::declval<std::decay_t<T> &>() = std::move(std::declval<std::decay_t<T> &&>()));
};

template <class T>
concept Copyable = requires(std::decay_t<T> &a, const std::decay<T> &b) { a = b; };
template <class T>
concept NoexceptCopyable = requires {
  requires Copyable<T>;
  requires noexcept(std::declval<std::decay_t<T> &>() = std::declval<const std::decay_t<T> &>());
};

template <class A, class B = A>
concept Assignable = requires(std::decay_t<A> &a, std::decay_t<B> b) { a = b; };