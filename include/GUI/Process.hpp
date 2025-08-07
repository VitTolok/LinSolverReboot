#pragma once
#include "Solver.hpp"
#include <QtWidgets/QWidget>
#include <thread>
namespace Ui {
class Process;
}

struct ProgressReport : public Progress {
 public:
  ProgressReport() : progress_(0) {
  }
  void operator=(int16_t prog) {
    progress_ = prog;
  }

  ~ProgressReport() override {
  }

 private:
  int16_t progress_;
};

class Process : public QWidget {
  Q_OBJECT

 private slots:
  void dummySlot() {
  }

 public:
  enum class ProcType { kDecompProc, kTestProc };
  Process(ProcType, std::unique_ptr<Solvable> &&, QWidget *parent = nullptr);
  int16_t Progress() const;
  bool Finished() const;
  const Solution *GetSolution() const;
  ProcType GetProcType() const;
  ~Process() override;
  bool operator<(const Process &) const;

 private:
  Ui::Process *ui;
  ProcType type_;
  std::thread thread_;
  std::unique_ptr<Solver> solver_;
  std::chrono::steady_clock::time_point start_;
  std::chrono::steady_clock::time_point end_;
  ProgressReport progress_;
  bool finished_;
  std::exception *error_;
};