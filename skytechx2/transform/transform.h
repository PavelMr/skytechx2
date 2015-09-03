#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "mapparam.h"
#include "vecmath.h"
#include "plane.h"
#include "skmath.h"

#include <QMatrix4x4>
#include <QOpenGLFunctions>

class RaDec {
public:
  RaDec()
  {
  }

  RaDec(double r, double d)
  {
    ra = r;
    dec = d;
  }
  QString toString()
  {
    return QString("R.A : %1 Dec : %2").arg(SkMath::toDeg(ra)).arg(SkMath::toDeg(dec));
  }

  double ra, dec;
};

class Transform
{
public:
  Transform();

  static void rdToVector(const RaDec &rd, Vector3 &vec);
  void setTransform(double width, double height, MapParam &param);
  void XYToRaDec(double sx, double sy, double &ra, double &dec);

  QMatrix4x4 getMVPMatrix();
  QMatrix4x4 getViewMatrix();
  QMatrix4x4 getProjectionMatrix();
  SKPLANE *getFrustum();
  MapParam *getMapParam();

  QOpenGLFunctions *getGl();
  void setGl(QOpenGLFunctions *gl);

public:
  float m_ratio;
  float m_fov;
  float m_width;
  float m_height;

private:
  QMatrix4x4 m_mvpMatrix;
  QMatrix4x4 m_projMatrix;
  QMatrix4x4 m_viewMatrix;
  QMatrix4x4 m_matViewNoPrecess;
  SKPLANE    m_frustum[5];
  MapParam   m_mapParam;

  QOpenGLFunctions *m_gl;
};

#endif // TRANSFORM_H
