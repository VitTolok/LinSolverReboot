#pragma once
#include "definitions.hpp"
#include <cstdint>
#include <functional>

class InvalidSolutionType : public MachineError {
 public:
  [[nodiscard]] const char *what() const noexcept override {
    return message_.c_str();
  }

 private:
  std::string message_ = "Solver received input of the wrong type";
};

struct Solvable {
  virtual ~Solvable() {
  }
};

struct Progress {
  virtual void operator=(int16_t) = 0;
  virtual ~Progress() {
  }
};

struct Solution {
  virtual ~Solution() {
  }
};

class Solver {
 public:
  Solver(Progress &progress, const std::function<void(Solution &&)> &finisher,
         const std::function<void(std::exception &&)> &terminator)
      : progress_(progress), finisher_(finisher), terminator_(terminator) {
  }
  virtual void operator()(const Solvable &) const = 0;
  virtual ~Solver() {
  }

  void Terminate(std::exception &&exception) const {
    terminator_(std::move(exception));
  }

  void Finish(Solution &&solution) const {
    finisher_(std::move(solution));
  }

 protected:
  Progress &progress_;

 private:
  std::function<void(Solution &&)> finisher_;
  std::function<void(std::exception &&)> terminator_;
};