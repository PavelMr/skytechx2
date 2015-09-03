#include "debugwidget.h"
#include "ui_debugwidget.h"

DebugWidget *g_debugWidget;

static QList <QLabel *> grid[2];

DebugWidget::DebugWidget(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::DebugWidget)
{
  setWindowFlags(Qt::Dialog | Qt::WindowStaysOnTopHint);

  ui->setupUi(this);

  grid[0].append(ui->label_0); grid[1].append(ui->value_0);
  grid[0].append(ui->label_1); grid[1].append(ui->value_1);
  grid[0].append(ui->label_2); grid[1].append(ui->value_2);
  grid[0].append(ui->label_3); grid[1].append(ui->value_3);
  grid[0].append(ui->label_4); grid[1].append(ui->value_4);
  grid[0].append(ui->label_5); grid[1].append(ui->value_5);
  grid[0].append(ui->label_6); grid[1].append(ui->value_6);

  reset();
}

DebugWidget::~DebugWidget()
{
  delete ui;
}

void DebugWidget::reset()
{
  m_index = 0;
}

void DebugWidget::addText(const QString &title,const QString &value)
{
  grid[0][m_index]->setText(title);
  grid[1][m_index]->setText(value);

  m_index++;
}
