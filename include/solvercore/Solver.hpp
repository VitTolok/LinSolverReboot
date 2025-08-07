#pragma once
#include "definitions.hpp"
#include <cstdint>

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
  Solver(Progress *progress, void (*finisher)(Solution *), void (*terminator)(std::exception *))
      : progress_(progress), finisher_(finisher), terminator_(terminator) {
  }
  virtual void operator()(const Solvable *) const = 0;
  virtual ~Solver() {
  }

  void Terminate(std::exception *exception) const {
    terminator_(exception);
  }

  void Finish(Solution *solution) const {
    finisher_(solution);
  }

  Progress *progress_;

 private:
  void (*finisher_)(Solution *);
  void (*terminator_)(std::exception *);
};