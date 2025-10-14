#pragma once
#include <memory>
#include <stdexcept>
#include <memory>

template <class A, class B>
concept Addable = requires(A a, B b) { a + b; };
template <class A, class B>
concept Subtractable = requires(A a, B b) { a - b; };
template <class A, class B>
concept Multipliable = requires(A a, B b) { a * b; };
template <class A, class B>
concept Divisible = requires(A a, B b) { a / b; };
template <class A, class B>
concept Moddable = requires(A a, B b) { a % b; };

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
