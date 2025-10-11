#pragma once
#include "definitions.hpp"
#include <cstdint>
#include <functional>

class InvalidSolutionType final : public MachineError {
 public:
  InvalidSolutionType() {
    message_ptr = message_;
  }
  ~InvalidSolutionType() final = default;

 private:
  inline static constexpr const char message_[] = "Solver received input of the wrong type";
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
  virtual std::unique_ptr<Solution> Detach() = 0;
  virtual ~Solution() {
  }
};

class Solver {
 public:
  Solver(Progress &progress, const std::function<void(Solution &&)> &finisher,
         const std::function<void(MachineError &&)> &terminator)
      : progress_(progress), finisher_(finisher), terminator_(terminator) {
  }
  virtual void operator()(const Solvable &) const = 0;
  virtual ~Solver() {
  }

  void Terminate(MachineError &&exception) const {
    terminator_(std::move(exception));
  }

  void Finish(Solution &&solution) const {
    finisher_(std::move(solution));
  }

 protected:
  Progress &progress_;

 private:
  std::function<void(Solution &&)> finisher_;
  std::function<void(MachineError &&)> terminator_;
};