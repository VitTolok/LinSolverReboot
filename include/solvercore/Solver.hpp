#pragma once
#include "definitions.hpp"
#include <cstdint>
#include <memory>

class InvalidSolutionType : public MachineError {
public:
  [[nodiscard]] const char *what() const noexcept override {
    return message_.c_str();
  }

private:
  std::string message_ = "Solver received input of the wrong type";
};

struct Solvable {
  virtual ~Solvable() = 0;
};

struct Solution {
  virtual ~Solution() = 0;
};

class Solver {
public:
  virtual void operator()(std::unique_ptr<Solvable> &&) = 0;
  virtual int16_t Progress() const = 0;
  virtual const std::shared_ptr<Solution> &GetResult() const = 0;
  virtual bool Finished() const = 0;
  virtual ~Solver() = 0;
};