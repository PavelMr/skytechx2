#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "mapwidget.h"

#include "sidebarquickinfo.h"
#include "sidebarmapcontrol.h"

#include <QDockWidget>
#include <QToolBox>
#include <QGLFormat>

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);

  m_mapWidget = new MapWidget(this);
  setCentralWidget(m_mapWidget);

  createSideBar();
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::closeEvent(QCloseEvent *)
{
  QApplication::quit();
}

void MainWindow::createSideBar()
{
  m_sideBar = new QDockWidget(tr("Sidebar"));
  m_sideBar->setMinimumWidth(256);
  m_sideBar->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

  addDockWidget(Qt::RightDockWidgetArea, m_sideBar);

  m_sideBarToolBox = new QToolBox();
  m_sideBar->setWidget(m_sideBarToolBox);

  m_sideBarQuickInfo = new SideBarQuickInfo();
  m_sideBarMapControl = new SideBarMapControl();

  m_sideBarToolBox->addItem(m_sideBarQuickInfo, tr("Quick information"));
  m_sideBarToolBox->addItem(m_sideBarMapControl, tr("Map control"));
}
