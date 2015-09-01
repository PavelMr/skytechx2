#include "sidebarquickinfo.h"
#include "ui_sidebarquickinfo.h"

SideBarQuickInfo::SideBarQuickInfo(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::SideBarQuickInfo)
{
  ui->setupUi(this);
}

SideBarQuickInfo::~SideBarQuickInfo()
{
  delete ui;
}
