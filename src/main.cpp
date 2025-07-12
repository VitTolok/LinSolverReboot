#include <qt6/QtWidgets/QApplication>
#include <qt6/QtWidgets/QMessageBox>
#include <qt6/QtWidgets/QPushButton>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  auto button = std::make_unique<QPushButton>("Click me!");
  button->setWindowTitle("Qt Button Example");
  button->resize(400, 300);

  // Connect the button's clicked signal to a lambda function
  QObject::connect(button.get(), &QPushButton::clicked, [&]() {
    QMessageBox::information(button.get(), "Button Clicked",
                             "You clicked the button!");
  });

  button->show();
  return app.exec();
}