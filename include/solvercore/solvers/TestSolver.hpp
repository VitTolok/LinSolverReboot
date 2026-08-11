#pragma once
#include "Solver.hpp"
#include "definitions.hpp"
#include <thread>

struct TestSolvable : public Solvable {
  int32_t number;
  explicit TestSolvable(int32_t number) : number(number) {
  }
  ~TestSolvable() override {
  }
};

struct TestSolution : public ISolution {
  std::string result;
  explicit TestSolution(const std::string &string) : result(string) {
  }
  std::unique_ptr<ISolution> Detach() override {
    return std::make_unique<TestSolution>(std::move(*this));
  }
  ~TestSolution() override {
  }
};

class TestSolver : public ISolver {
 public:
  TestSolver(IProgress &progress, const std::function<void(ISolution &&)> &finisher,
             const std::function<void(IMachineError &&)> &terminator)
      : ISolver(progress, finisher, terminator) {
  }

  void operator()(const Solvable &solvable) const override {
    auto input = dynamic_cast<const TestSolvable *>(&solvable);
    if (input == nullptr) {
      Terminate(InvalidSolutionType{});
      return;
    }
    switch (input->number) {
      case 0:
        Finish(TestSolution("zero"));
        break;
      case 1:
        for (int16_t prog = 0; prog < 100; prog++) {
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
          progress_ = prog;
        }
        Finish(TestSolution("one"));
        break;
      default:
        Terminate(UnknownMachineError{});
    }
  }

  ~TestSolver() override = default;
};