#pragma once
#include "definitions.hpp"
#include <cstdint>
#include <functional>

class InvalidSolutionType final : public MachineError<InvalidSolutionType> {
 public:
  InvalidSolutionType() : MachineError("Solver received input of the wrong type") {
  }
};

struct Solvable {
  virtual ~Solvable() = default;
};

struct IProgress {
  virtual void operator=(int16_t) = 0;
  virtual ~IProgress() = default;
};

struct ISolution {
  virtual std::unique_ptr<ISolution> Detach() = 0;
  virtual ~ISolution() = default;
};

class ISolver {
 public:
  ISolver(IProgress &progress, const std::function<void(ISolution &&)> &finisher,
          const std::function<void(IMachineError &&)> &terminator)
      : progress_(progress), finisher_(finisher), terminator_(terminator) {
  }
  virtual void operator()(const Solvable &) const = 0;
  virtual ~ISolver() = default;

  void Terminate(IMachineError &&exception) const {
    terminator_(std::move(exception));
  }

  void Finish(ISolution &&solution) const {
    finisher_(std::move(solution));
  }

 protected:
  IProgress &progress_;

 private:
  std::function<void(ISolution &&)> finisher_;
  std::function<void(IMachineError &&)> terminator_;
};

enum class NumericType { kLongDouble, kRational };
enum class OperatorType { kAdd, kSub, kMul, kDiv, kMod };
