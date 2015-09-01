#ifndef SIDEBARQUICKINFO_H
#define SIDEBARQUICKINFO_H

#include <QWidget>

namespace Ui {
class SideBarQuickInfo;
}

class SideBarQuickInfo : public QWidget
{
  Q_OBJECT

public:
  explicit SideBarQuickInfo(QWidget *parent = 0);
  ~SideBarQuickInfo();

private:
  Ui::SideBarQuickInfo *ui;
};

#endif // SIDEBARQUICKINFO_H
