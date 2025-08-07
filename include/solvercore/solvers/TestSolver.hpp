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
  TestSolver(Progress *progress, void (*finisher)(Solution *), void (*terminator)(std::exception *))
      : Solver(progress, finisher, terminator) {
  }

  virtual void operator()(const Solvable *solvable) const override {
    auto input = dynamic_cast<const TestSolvable *>(solvable);
    if (input == nullptr) {
      Terminate(new InvalidSolutionType{});
      return;
    }
    switch (input->number) {
      case 0:
        Finish(new TestSolution("zero"));
        break;
      case 1:
        for (size_t prog = 0; prog < 100; prog++) {
          std::this_thread::sleep_for(std::chrono::milliseconds(100));
          *progress_ = prog;
        }
        Finish(new TestSolution("one"));
        break;
      default:
        Terminate(new MachineError{});
    }
  }

  ~TestSolver() override {
  }
};