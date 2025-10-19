#pragma once
#include <stdexcept>
#include <memory>

class MachineError : public std::exception {
 public:
  MachineError() : message_ptr(message_) {
  }

  [[nodiscard]] const char *what() const noexcept final {
    return message_ptr;
  }

  virtual std::unique_ptr<MachineError> Detach() {
    return std::make_unique<MachineError>(std::move<MachineError &>(*this));
  }

  virtual ~MachineError() = default;

 protected:
  const char *message_ptr;

 private:
  inline static constexpr const char message_[] = "Unknown Machine Error";
};

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