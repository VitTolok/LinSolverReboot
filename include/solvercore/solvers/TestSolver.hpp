#include "Solver.hpp"
#include <thread>

struct TestSolvable : public Solvable {
  int32_t number;
  explicit TestSolvable(int32_t number) : number(number) {
  }
  ~TestSolvable() override {
  }
};

struct TestSolution : public Solution {
  std::string result;
  explicit TestSolution(const std::string &string) : result(string) {
  }
  ~TestSolution() override {
  }
};

class TestSolver : public Solver {
 public:
  TestSolver(Progress &progress, const std::function<void(Solution &&)> &finisher,
             const std::function<void(std::exception &&)> &terminator)
      : Solver(progress, finisher, terminator) {
  }

  virtual void operator()(const Solvable &solvable) const override try {
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
        for (size_t prog = 0; prog < 100; prog++) {
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
          progress_ = prog;
        }
        Finish(TestSolution("one"));
        break;
      default:
        Terminate(MachineError{});
    }
  } catch (std::exception &exception) {
    Terminate(std::move(exception));
  }

  ~TestSolver() override {
  }
};