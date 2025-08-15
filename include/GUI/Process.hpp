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

class TerminationBreak : public std::exception {};

struct ProgressReport : public Progress {
 public:
  ProgressReport(Interface &);
  void operator=(int16_t prog) override;
  void Report();
  void Terminate();
  ~ProgressReport() override {
  }

 private:
  int16_t progress_;
  bool terminate_signal_;
  bool active_;
  Interface &interface_;
};

class Process : public QWidget {
  Q_OBJECT

 public:
  enum class ProcType { kDecompProc, kTestProc };
  Process(ProcType, std::unique_ptr<Solvable> &&, Interface &, QWidget *parent = nullptr);
  ~Process() override;
  void ReleaseInterface();
  void ReportToInterface();
  const Ui::Process *Ui() const;

 private:
  std::unique_ptr<Solvable> solvable_;
  Ui::Process *ui_;
  ProcType type_;
  std::thread thread_;
  std::unique_ptr<Solver> solver_;
  std::chrono::steady_clock::time_point start_;
  std::chrono::steady_clock::time_point end_;
  ProgressReport progress_;
  std::unique_ptr<std::exception> error_;
  std::unique_ptr<Solution> solution_;
  std::mutex mutex_;

  friend void Finisher(Process &process, Solution &&solution);
  friend void Terminator(Process &process, std::exception &&solution);
};