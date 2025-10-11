#pragma once
#include <memory>
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
