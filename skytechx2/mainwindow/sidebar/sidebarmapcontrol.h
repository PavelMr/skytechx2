#ifndef SIDEBARMAPCONTROL_H
#define SIDEBARMAPCONTROL_H

#include <QWidget>

namespace Ui {
class SideBarMapControl;
}

class SideBarMapControl : public QWidget
{
  Q_OBJECT

public:
  explicit SideBarMapControl(QWidget *parent = 0);
  ~SideBarMapControl();

private:
  Ui::SideBarMapControl *ui;
};

#endif // SIDEBARMAPCONTROL_H
