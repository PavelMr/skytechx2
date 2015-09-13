#include "mapwidget.h"
#include "renderer.h"
#include "mapparam.h"
#include "skmath.h"
#include "debugwidget.h"
#include "dataresources.h"
#include "stringutils.h"
#include "mapobject.h"

#include "tycho.h"
#include "gsc.h"
#include "ucac4.h"

#include <QPainter>
#include <QOpenGLBuffer>
#include <QVector3D>
#include <QOpenGLTexture>
#include <QMouseEvent>
#include <QOpenGLDebugLogger>
#include <QTime>

#include <QDebug>

static QOpenGLDebugLogger *lg;

static QImage *m_overlayImage = NULL;

static int gpuTotalMemory;

MapWidget::MapWidget(QWidget *parent) : QOpenGLWidget(parent)
{
  setMouseTracking(true);

  m_width = 0;
  m_height = 0;

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

  QOpenGLContext *ctx = QOpenGLContext::currentContext();
  Q_UNUSED(ctx);
  lg = new QOpenGLDebugLogger(context());
  lg->initialize();
  lg->startLogging(QOpenGLDebugLogger::SynchronousLogging);
  lg->enableMessages();
  connect(lg, SIGNAL(messageLogged(QOpenGLDebugMessage)), this, SLOT(glLog(QOpenGLDebugMessage)));

  qDebug() << "Driver Version String:" << versionString;
  qDebug() << "Current Context:" << ctx->format();

  glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &gpuTotalMemory);
  qDebug() << "total" << gpuTotalMemory;

  m_renderer = new Renderer();
  m_renderer->createStaticResources();

  m_painterOverlay = new PainterOverlay();
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
  g_debugWidget->reset();

  g_dataResource->getMapObject()->newFrame();

  if (m_width < 1 || m_height < 1)
  {
    return;
  }

  QTime time;
  time.start();

  QOpenGLFunctions gl(context());
  m_transform.setGl(&gl);
  m_transform.setTransform(m_width, m_height, *m_transform.getMapParam());


  glClearColor(0.05, 0.05, 0.1f, 1);
  glClear(GL_COLOR_BUFFER_BIT);
  glEnable(GL_MULTISAMPLE);

  double starPlus = m_transform.getMapParam()->m_starMagAdd;
  QEasingCurve curve(QEasingCurve::InExpo);
  m_transform.getMapParam()->m_maxStarMag = starPlus + 5 + 12.0 * curve.valueForProgress(FRAC(m_transform.getMapParam()->m_fov, SkMath::toRad(90), SkMath::toRad(0.5)));

  m_transform.getMapParam()->m_fov = CLAMP(m_transform.getMapParam()->m_fov, SkMath::toRad(0.01), R90);

  //qDebug() << m_transform.getMapParam()->m_maxStarMag;
  //qDebug() << SkMath::toDeg(m_transform.getMapParam()->m_fov);

  m_renderer->render(&m_transform);

  /*
  QPainter p;

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


  //qDebug() << time.elapsed() << 1000 / (float)time.elapsed() << g_dataResource->getMapObject()->getObjectCount();
  //qDebug() << SkMath::toDeg(mapParam.m_fov);

  writeDebug();
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
    m_transform.getMapParam()->m_fov *= 0.8;
  else
    m_transform.getMapParam()->m_fov *= 1.2;
  update();
}


void MapWidget::mousePressEvent(QMouseEvent *e)
{
  m_lastPoint = e->pos();

  if (e->buttons() & Qt::RightButton)
  {
    double ra, dec;
    m_transform.XYToRaDec(e->pos().x(), e->pos().y(), ra, dec);
    g_dataResource->getMapObject()->objList(RaDec(ra, dec), &m_transform);
  }

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

    m_transform.getMapParam()->m_x += rad;
    m_transform.getMapParam()->m_y += ded;

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
    m_transform.getMapParam()->m_x -= 0.01;
  }
  else
  if (e->key() == Qt::Key_Right)
  {
    m_transform.getMapParam()->m_x += 0.01;
  }
  else
  if (e->key() == Qt::Key_Up)
  {
    m_transform.getMapParam()->m_y -= 0.01;
  }
  else
  if (e->key() == Qt::Key_Down)
  {
    m_transform.getMapParam()->m_y += 0.01;
  }
  else
  if (e->key() == Qt::Key_Insert)
  {
    m_transform.getMapParam()->m_roll -= 0.05;
  }
  else
  if (e->key() == Qt::Key_PageUp)
  {
    m_transform.getMapParam()->m_roll += 0.05;
  }

  update();
}

void MapWidget::addStarMagnitude(double add)
{
   m_transform.getMapParam()->m_starMagAdd += add;
  update();
}

void MapWidget::writeDebug()
{
  int mem;
  glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &mem);
  g_debugWidget->addText("GPU Usage", QString("%1 kB").arg(StringUtils::addComma(gpuTotalMemory - mem)));

  g_debugWidget->addText("Tycho", QString("%1 kB").arg(StringUtils::addComma(g_dataResource->getTycho()->getMemoryUsage() / 1024)));

  g_debugWidget->addText("GSC 1.2", QString("%1 kB\nRegions used : %2").arg(StringUtils::addComma(g_dataResource->getGSC()->getMemoryUsage() / 1024)).
                                                                        arg(StringUtils::addComma(g_dataResource->getGSC()->getRegionUsed())));

  g_debugWidget->addText("UCAC4", QString("%1 kB\nRegions used : %2").arg(StringUtils::addComma(g_dataResource->getUCAC4()->getMemoryUsage() / 1024)).
                                                                      arg(StringUtils::addComma(g_dataResource->getUCAC4()->getRegionUsed())));
}

void MapWidget::glLog(QOpenGLDebugMessage msg)
{
  if (msg.type() != QOpenGLDebugMessage::OtherType)
    qDebug() << msg.message();
}

