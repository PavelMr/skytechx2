#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include "renderer.h"
#include "transform.h"
#include "painteroverlay.h"

#include <QWidget>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLDebugMessage>

class MapWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
  Q_OBJECT
public:
  explicit MapWidget(QWidget *parent = 0);
  ~MapWidget();

protected:
  void initializeGL() Q_DECL_OVERRIDE;
  void resizeGL(int w, int h) Q_DECL_OVERRIDE;
  void paintGL() Q_DECL_OVERRIDE;
  //void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;

  void wheelEvent(QWheelEvent *e);
  void mousePressEvent(QMouseEvent *e);
  void mouseMoveEvent(QMouseEvent *e);

  void keyPressEvent(QKeyEvent *e);

private:
  PainterOverlay *m_painterOverlay;
  Renderer *m_renderer;
  double m_width;
  double m_height;
  Transform m_transform;
  QPoint m_lastPoint;

  void writeDebug();

signals:

public slots:
  void glLog(QOpenGLDebugMessage msg);


};

#endif // MAPWIDGET_H
