#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MapWidget;
class SideBarQuickInfo;
class SideBarMapControl;

class QDockWidget;
class QToolBox;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();

private:
  Ui::MainWindow *ui;

  MapWidget *m_mapWidget;

  QDockWidget *m_sideBar;
  QToolBox *m_sideBarToolBox;

  SideBarQuickInfo *m_sideBarQuickInfo;
  SideBarMapControl *m_sideBarMapControl;

  void createSideBar();
};

#endif // MAINWINDOW_H
