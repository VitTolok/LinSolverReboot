#pragma once
#include "Solver.hpp"
#include <QtWidgets/QWidget>
#include <QtWidgets/QProgressBar>
#include <mutex>
#include <thread>
namespace Ui {
class Process;
}

class Interface;

struct ProgressReport : public Progress {
 public:
  ProgressReport(QProgressBar *);
  void operator=(int16_t prog) override;
  void Report();
  ~ProgressReport() override {
  }

 private:
  int16_t progress_;
  QProgressBar *progress_bar_;
};

class Process : public QWidget {
  Q_OBJECT

 public:
  enum class ProcType { kDecompProc, kTestProc };
  Process(ProcType, Solvable *, Interface *, QWidget *parent = nullptr);
  ~Process() override;
  bool operator<(const Process &) const;
  void ReleaseInterface();
  void ReportToInterface();

 private:
  Solvable *solvable_;
  Ui::Process *ui_;
  ProcType type_;
  std::thread thread_;
  std::unique_ptr<Solver> solver_;
  std::chrono::steady_clock::time_point start_;
  std::chrono::steady_clock::time_point end_;
  ProgressReport progress_;
  bool finished_;
  std::exception *error_;
  Solution *solution_;
  std::mutex mutex_;

  friend void Finisher(Process *process, Solution *solution);
  friend void Terminator(Process *process, std::exception *solution);
};