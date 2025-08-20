#pragma once
#include "Solver.hpp"
#include <QtWidgets/QWidget>
#include <QtWidgets/QProgressBar>
#include <mutex>
#include <QThread>
namespace Ui {
class Process;
}

class Interface;

class TerminationBreak : public MachineError {};

struct ProgressReport : public QObject, public Progress {
  Q_OBJECT
 public:
  ProgressReport(Interface &);
  void operator=(int16_t) override;
  void Report();
  bool Active() const;
  void ReportToInterface();
  void ReleaseInterface();
  void Terminate();
  ~ProgressReport() override {
  }
  void SetSolver(Solver *, const Solvable *);

 public slots:
  void Execute();

 signals:
  void ReportProgress(int16_t);
  void EndThread();

 private:
  int16_t progress_;
  bool terminate_signal_;
  bool active_;
  Interface &interface_;
  Solver *solver_;
  const Solvable *solvable_;
  std::mutex mutex_;
};

class Process : public QWidget {
  Q_OBJECT

 public:
  enum class ProcType { kDecompProc, kTestProc };
  Process(ProcType, std::unique_ptr<Solvable> &&, Interface &, QWidget *parent = nullptr);
  ~Process() override;
  void ReleaseInterface();
  void ReportToInterface();
  ProcType GetProcType() const;
  const Ui::Process *Ui() const;

 private:
  std::unique_ptr<Solvable> solvable_;
  Interface &interface_;
  Ui::Process *ui_;
  ProcType type_;
  QThread *thread_;
  std::unique_ptr<Solver> solver_;
  std::chrono::steady_clock::time_point start_;
  std::chrono::steady_clock::time_point end_;
  ProgressReport *progress_;
  std::unique_ptr<MachineError> error_;
  std::unique_ptr<Solution> solution_;
  std::mutex mutex_;

  friend void Finisher(Process &process, Solution &&solution);
  friend void Terminator(Process &process, MachineError &&solution);
};