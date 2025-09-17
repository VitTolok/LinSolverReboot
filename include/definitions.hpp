#pragma once
#include <stdexcept>
#include <memory>

class MachineError : public std::exception {
 public:
  [[nodiscard]] const char *what() const noexcept override {
    return message_.c_str();
  }

  virtual std::unique_ptr<MachineError> Clone() const {
    return std::make_unique<MachineError>(*this);
  }

  virtual ~MachineError() = default;

 private:
  std::string message_ = "Unknown Machine Error";
};
