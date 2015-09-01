#include "mapwidget.h"
#include "renderer.h"
#include "mapparam.h"
#include "skmath.h"

#include <QPainter>
#include <QOpenGLBuffer>
#include <QVector3D>
#include <QOpenGLTexture>
#include <QMouseEvent>
#include <QGLFormat>
#include <QOpenGLDebugLogger>
#include <QTime>

#include <QDebug>

static QOpenGLDebugLogger *lg;

static MapParam mapParam;


MapWidget::MapWidget(QWidget *parent) : QOpenGLWidget(parent)
{
  setMouseTracking(true);

  m_width = 0;
  m_height = 0;

  QSurfaceFormat fmt;

  fmt.setSamples(16);
  fmt.setOption(QSurfaceFormat::DebugContext);
  fmt.setVersion(2, 1);
  fmt.setProfile(QSurfaceFormat::CoreProfile);
  setFormat(fmt);
}

MapWidget::~MapWidget()
{
  delete lg;
}

void MapWidget::initializeGL()
{
  initializeOpenGLFunctions();

  QString versionString(QLatin1String(reinterpret_cast<const char*>(glGetString(GL_VERSION))));
  qDebug() << "Driver Version String:" << versionString;
  qDebug() << "Current Context:" << format();

  QOpenGLContext *ctx = QOpenGLContext::currentContext();
  lg = new QOpenGLDebugLogger(context());
  lg->initialize();
  lg->startLogging(QOpenGLDebugLogger::SynchronousLogging);
  lg->enableMessages();
  connect(lg, SIGNAL(messageLogged(QOpenGLDebugMessage)), this, SLOT(glLog(QOpenGLDebugMessage)));


  int pp;
  glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &pp);
  qDebug() << "freem" << pp;

  m_renderer = new Renderer();
  m_renderer->createStaticResources();

  glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &pp);
  qDebug() << "freem" << pp;
}

void MapWidget::resizeGL(int w, int h)
{
  glViewport(0, 0, w, h);

  m_width = w;
  m_height = h;
}

void MapWidget::paintGL()
{
  if (m_width < 1 || m_height < 1)
  {
    return;
  }

  QTime time;
  time.start();

  QOpenGLFunctions gl(context());

  m_transform.setGl(&gl);
  m_transform.setTransform(m_width, m_height, mapParam);

  //QOpenGLFunctions f(context());
  //f.glActiveTexture(GL_TEXTURE0);
  //f.glActiveTexture(GL_TEXTURE1);

  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  glEnable(GL_MULTISAMPLE);

  //double ra, dec;
  //m_transform.XYToRaDec(m_width / 2., m_height / 2., ra, dec);

  m_renderer->render(&m_transform);

  //qDebug() << time.elapsed() << 1000 / (float)time.elapsed();
}

void MapWidget::wheelEvent(QWheelEvent *e)
{
  if (e->delta() > 0)
    mapParam.m_fov *= 0.8;
  else
    mapParam.m_fov *= 1.2;
  update();
}

/*
void MapWidget::paintEvent(QPaintEvent *e)
{
  QPainter p(// QOpenGLContext::currentContext());

  p.beginNativePainting();
  p.setPen(Qt::yellow);
  p.drawLine(0, 0, 100, 100);
  p.endNativePainting();
}
*/

void MapWidget::mousePressEvent(QMouseEvent *e)
{
  m_lastPoint = e->pos();

  double ra, dec;

  m_transform.XYToRaDec(e->pos().x(), e->pos().y(), ra, dec);

  //qDebug() << SkMath::toDeg(ra) << SkMath::toDeg(dec);
}

void MapWidget::mouseMoveEvent(QMouseEvent *e)
{
  if (e->buttons() & Qt::LeftButton)
  {
    double x = e->pos().x() - m_lastPoint.x();
    double y = e->pos().y() - m_lastPoint.y();

    mapParam.m_x -= x * 0.01;
    mapParam.m_y -= y * 0.01;

    m_lastPoint = e->pos();

    update();
  }
}

void MapWidget::glLog(QOpenGLDebugMessage msg)
{
  if (msg.type() != QOpenGLDebugMessage::OtherType)
    qDebug() << msg.message();
}

