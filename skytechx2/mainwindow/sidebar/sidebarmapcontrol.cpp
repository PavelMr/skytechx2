#include "sidebarmapcontrol.h"
#include "ui_sidebarmapcontrol.h"

SideBarMapControl::SideBarMapControl(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::SideBarMapControl)
{
  ui->setupUi(this);
}

SideBarMapControl::~SideBarMapControl()
{
  delete ui;
}
