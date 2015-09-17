#include "mainwindow.h"
#include "dataresources.h"
#include "debugwidget.h"
#include "xmlsetting.h"

#include <QApplication>
#include <QSurfaceFormat>

#include "QEasingCurve"
#include "QDebug"
#include "math.h"

#include <QVector>
#include <QList>
#include <QTime>

XmlSetting g_systemSetting;

QVector <int> vec;
QList <int> list;

int count = 10000000;
int bb;

void test1()
{
  for (int i = 0; i < count; i++)
  {
    vec.append(i);
  }
}

void test2()
{
  for (int i = 0; i < count; i++)
  {
    list.append(i);
  }
}

void test11()
{
  for (int i = 0; i < count; i++)
  {
    int a = vec[i];
    bb = a;
  }
}

void test22()
{
  for (int i = 0; i < count; i++)
  {
    int a = list[i];
    bb = a;
  }
}

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  QSurfaceFormat fmt;


  fmt.setOption(QSurfaceFormat::DebugContext);
  fmt.setVersion(2, 1);
  fmt.setProfile(QSurfaceFormat::CoreProfile);
  fmt.setStencilBufferSize(0);
  fmt.setDepthBufferSize(0);
  fmt.setSamples(16);

  QSurfaceFormat::setDefaultFormat(fmt);

  qDebug() << "depth" << fmt.depthBufferSize();

  g_systemSetting.load("settings/system.xml");

  g_dataResource = new DataResources();

  g_debugWidget = new DebugWidget();
  g_debugWidget->show();

  MainWindow *mainWindow = new MainWindow();

  mainWindow->show();
  mainWindow->resize(1600, 900);
  mainWindow->move(100, 100);

  return a.exec();
}
