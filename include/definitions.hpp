#pragma once
#include <stdexcept>

class MachineError : public std::exception {
 public:
  [[nodiscard]] const char *what() const noexcept override {
    return message_.c_str();
  }

 private:
  std::string message_ = "Unknown Machine Error";
};
