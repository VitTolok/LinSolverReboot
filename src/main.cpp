#include "MainWindow.hpp"
#include <qt6/QtWidgets/QApplication>

int main(int argc, char **argv) {
  QApplication app(argc, argv);
  MainWindow window;
  window.show();
  app.exec();
}