#include "mainwindow.h"
#include "dataresources.h"

#include <QApplication>

#include "QEasingCurve"
#include "QDebug"
#include "math.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  g_dataResource = new DataResources();

  MainWindow w;
  w.show();

  QEasingCurve cc(QEasingCurve::OutCubic);

  for (double i = 0; i < 1; i += 0.1)
  {
    double t = i;
    double b = 0;
    double c = 1;
    double d = 1;
    qDebug() << i << cc.valueForProgress(i) << pow (t - 1, 3) + 1;
  }

  /*
  TextRenderer r;

  r.createTexture(512, 5);

  for (int i = 0; i < 80; i++)
    r.addText("ABCDE ", RaDec(0, 0), 0);
  //r.addText("1234567", RaDec(0, 0), 0);
  //r.addText("x123456y", RaDec(0, 0), 0);
  //r.addText("qwertu ", RaDec(0, 0), 0);
  //r.addText("Pavel Mraz", RaDec(0, 0), 0);
  //r.addText("Ninja 223 rerer rere", RaDec(0, 0), 0);
  r.save();
  */

  return a.exec();
}
