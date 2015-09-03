#include "mapwidget.h"
#include "renderer.h"
#include "mapparam.h"
#include "skmath.h"

#include <QPainter>
#include <QOpenGLBuffer>
#include <QVector3D>
#include <QOpenGLTexture>
#include <QMouseEvent>
#include <QOpenGLDebugLogger>
#include <QTime>

#include <QDebug>

static QOpenGLDebugLogger *lg;

static MapParam mapParam;

static QImage *m_overlayImage = NULL;

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

  setFocusPolicy(Qt::StrongFocus);
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
  Q_UNUSED(ctx);
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

  m_painterOverlay = new PainterOverlay();

  glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &pp);
  qDebug() << "freem" << pp;
}

void MapWidget::resizeGL(int w, int h)
{
  qDebug() << "resize GL" << w << h;

  glViewport(0, 0, w, h);

  m_width = w;
  m_height = h;

  if (m_overlayImage)
  {
    delete m_overlayImage;
  }

  m_overlayImage = new QImage(w, h, QImage::Format_RGBA8888_Premultiplied);

  m_painterOverlay->createTexture(m_overlayImage);
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


  glClearColor(0.05, 0.05, 0.1f, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  glEnable(GL_MULTISAMPLE);

  m_renderer->render(&m_transform);

  QPainter p;

  /*
  m_overlayImage->fill(Qt::transparent);

  static int a = 100;
  a++;

  p.begin(m_overlayImage);
  p.setRenderHint(QPainter::Antialiasing);
  p.setPen(Qt::green);
  p.drawLine(0, 0, 1000, 1000);
  p.drawLine(500, 10, 5000, 1000);
  p.fillRect(QRect(10, 10, 100, 100 + a), QColor(255, 255, 0, 128));
  p.end();

  m_painterOverlay->render(&m_transform, m_overlayImage);
  */

  //qDebug() << time.elapsed() << 1000 / (float)time.elapsed();
  //qDebug() << SkMath::toDeg(mapParam.m_fov);
}


/*
void MapWidget::paintEvent(QPaintEvent *e)
{
  return;
  QPainter p;

  p.begin(this);
  p.setRenderHint(QPainter::Antialiasing);
  p.setPen(Qt::yellow);
  //p.drawLine(0, 0, 100, 100);
  p.fillRect(QRect(0,0, 100, 100), Qt::red);
  p.end();
}
*/


void MapWidget::wheelEvent(QWheelEvent *e)
{
  //QImage i = grabFramebuffer();
  //i.save("ppp.png", "PNG");

  if (e->delta() > 0)
    mapParam.m_fov *= 0.8;
  else
    mapParam.m_fov *= 1.2;
  update();
}


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
    RaDec rd1;
    RaDec rd2;

    m_transform.XYToRaDec(e->pos().x(), e->pos().y(), rd1.ra, rd1.dec);
    m_transform.XYToRaDec(m_lastPoint.x(), m_lastPoint.y(), rd2.ra, rd2.dec);

    double rad = rd1.ra - rd2.ra;
    double ded = rd1.dec - rd2.dec;

    mapParam.m_x += rad;
    mapParam.m_y += ded;

    //rangeDbl(&m_mapView.x, R360);
    //m_mapView.y = CLAMP(m_mapView.y, -R90, R90);

    /*
    double x = e->pos().x() - m_lastPoint.x();
    double y = e->pos().y() - m_lastPoint.y();

    mapParam.m_x -= x * 0.005;
    mapParam.m_y -= y * 0.005;
    */

    m_lastPoint = e->pos();

    update();
  }
}

void MapWidget::keyPressEvent(QKeyEvent *e)
{
  if (e->key() == Qt::Key_Left)
  {
    mapParam.m_x -= 0.01;
  }
  else
  if (e->key() == Qt::Key_Right)
  {
    mapParam.m_x += 0.01;
  }
  else
  if (e->key() == Qt::Key_Up)
  {
    mapParam.m_y -= 0.01;
  }
  else
  if (e->key() == Qt::Key_Down)
  {
    mapParam.m_y += 0.01;
  }
  else
  if (e->key() == Qt::Key_Insert)
  {
    mapParam.m_roll -= 0.05;
  }
  else
  if (e->key() == Qt::Key_PageUp)
  {
    mapParam.m_roll += 0.05;
  }

  update();
}

void MapWidget::glLog(QOpenGLDebugMessage msg)
{
  if (msg.type() != QOpenGLDebugMessage::OtherType)
    qDebug() << msg.message();
}

