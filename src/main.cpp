#include "mainwindow.h"

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  MainWindow window;
  window.resize(640, 360);
  window.setVisible(true);
  return app.exec();
}
