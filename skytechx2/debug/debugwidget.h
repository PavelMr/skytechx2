#ifndef DEBUGWIDGET_H
#define DEBUGWIDGET_H

#include <QWidget>

namespace Ui {
class DebugWidget;
}

class DebugWidget : public QWidget
{
  Q_OBJECT

public:
  explicit DebugWidget(QWidget *parent = 0);
  ~DebugWidget();

  void reset();
  void addText(const QString &title, const QString &value);

private:
  Ui::DebugWidget *ui;
  int m_index;
};

extern DebugWidget *g_debugWidget;

#endif // DEBUGWIDGET_H
