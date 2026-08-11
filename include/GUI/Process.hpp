#pragma once
#include "Solver.hpp"
#include "definitions.hpp"
#include <QtWidgets/QWidget>
#include <QtWidgets/QProgressBar>
#include <mutex>
#include <QThread>
namespace Ui {
class Process;
}

class Interface;

class TerminationBreak : public MachineError<TerminationBreak> {
 public:
  TerminationBreak() : MachineError("Process Terminated") {
  }
};

struct ProgressReport : public QObject, public IProgress {
  Q_OBJECT
 public:
  explicit ProgressReport(Interface &);
  void operator=(int16_t) override;
  void Report();
  bool Active() const;
  void ReportToInterface();
  void ReleaseInterface();
  void Terminate();
  ~ProgressReport() override = default;
  void SetSolver(ISolver *, const Solvable *);

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
  ISolver *solver_;
  const Solvable *solvable_;
  std::mutex mutex_;
};

class Process : public QWidget {
  Q_OBJECT

 public slots:
  void SoftReportToInterface();

 public:
  enum class ProcType { kDecompProc, kTestProc, kMatAriProc };
  Process(ProcType, std::unique_ptr<Solvable> &&, Interface &, QWidget *parent = nullptr);
  ~Process() override;
  Interface *GetInterface();
  void ReleaseInterface();
  void ReportToInterface();
  const Ui::Process *Ui() const;

 private:
  std::unique_ptr<Solvable> solvable_;
  Interface &interface_;
  Ui::Process *ui_;
  QThread *thread_;
  std::unique_ptr<ISolver> solver_;
  std::chrono::steady_clock::time_point start_;
  std::chrono::steady_clock::time_point end_;
  std::unique_ptr<ProgressReport> progress_;
  std::unique_ptr<IMachineError> error_;
  std::unique_ptr<ISolution> solution_;
  std::mutex mutex_;

  friend void Finisher(Process &process, ISolution &&solution);
  friend void Terminator(Process &process, IMachineError &&solution);
};