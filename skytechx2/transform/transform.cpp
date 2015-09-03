#include "transform.h"
#include "skmath.h"

#include <math.h>

Transform::Transform()
{
}

void Transform::rdToVector(const RaDec &rd, Vector3 &vec)
{
  double ra = rd.ra - R90;
  double cd = cos(rd.dec);

  vec.x = -cos(ra) * cd;
  vec.y = sin(rd.dec);
  vec.z = sin(ra) * cd;
}

void Transform::setTransform(double width, double height, MapParam &param)
{
  QMatrix4x4 projection, projectionFrustum;
  QMatrix4x4 view;
  QMatrix4x4 vX;
  QMatrix4x4 vY;
  QMatrix4x4 vZ;

  m_mapParam = param;

  m_ratio = width / height;
  m_fov = param.m_fov;
  m_width = width;
  m_height = height;

  vX.rotate(SkMath::toDeg(param.m_x), 0, 1, 0);
  vY.rotate(SkMath::toDeg(param.m_y), 1, 0, 0);
  vZ.rotate(SkMath::toDeg(param.m_roll), 0, 0, 1);

  view = vZ * vY * vX;

  float aspect = width / height;

  projection.perspective(SkMath::toDeg(param.m_fov) / aspect, aspect, 0.01, 1.1);
  projectionFrustum.perspective(SkMath::toDeg(param.m_fov * 1.2) / aspect, aspect, 0.01, 1.1);

  m_viewMatrix = view;
  m_matViewNoPrecess = view;
  m_mvpMatrix = projection * view;
  m_projMatrix = projection;

  QMatrix4x4 planeMatrix = projectionFrustum * view;

  m_frustum[0].x = planeMatrix(3, 0) + planeMatrix(0, 0);
  m_frustum[0].y = planeMatrix(3, 1) + planeMatrix(0, 1);
  m_frustum[0].z = planeMatrix(3, 2) + planeMatrix(0, 2);
  m_frustum[0].dist = planeMatrix(3, 3) + planeMatrix(0, 3);
  SKPLANENormalize(&m_frustum[0]);

  m_frustum[1].x = planeMatrix(3, 0) - planeMatrix(0, 0);
  m_frustum[1].y = planeMatrix(3, 1) - planeMatrix(0, 1);
  m_frustum[1].z = planeMatrix(3, 2) - planeMatrix(0, 2);
  m_frustum[1].dist = planeMatrix(3, 3) - planeMatrix(0, 3);
  SKPLANENormalize(&m_frustum[1]);

  m_frustum[2].x = planeMatrix(3, 0) - planeMatrix(1, 0);
  m_frustum[2].y = planeMatrix(3, 1) - planeMatrix(1, 1);
  m_frustum[2].z = planeMatrix(3, 2) - planeMatrix(1, 2);
  m_frustum[2].dist = planeMatrix(3, 3) - planeMatrix(1, 3);
  SKPLANENormalize(&m_frustum[2]);

  m_frustum[3].x = planeMatrix(3, 0) + planeMatrix(1, 0);
  m_frustum[3].y = planeMatrix(3, 1) + planeMatrix(1, 1);
  m_frustum[3].z = planeMatrix(3, 2) + planeMatrix(1, 2);
  m_frustum[3].dist = planeMatrix(3, 3) + planeMatrix(1, 3);
  SKPLANENormalize(&m_frustum[3]);

  m_frustum[4].x = planeMatrix(3, 0) + planeMatrix(2, 0);
  m_frustum[4].y = planeMatrix(3, 1) + planeMatrix(2, 1);
  m_frustum[4].z = planeMatrix(3, 2) + planeMatrix(2, 2);
  m_frustum[4].dist = planeMatrix(3, 3) + planeMatrix(2, 3);
  SKPLANENormalize(&m_frustum[4]);
}

QMatrix4x4 Transform::getMVPMatrix()
{
  return m_mvpMatrix;
}

QMatrix4x4 Transform::getViewMatrix()
{
  return m_viewMatrix;
}

QMatrix4x4 Transform::getProjectionMatrix()
{
  return m_projMatrix;
}

SKPLANE *Transform::getFrustum()
{
  return m_frustum;
}

MapParam *Transform::getMapParam()
{
  return &m_mapParam;
}

QOpenGLFunctions *Transform::getGl()
{
  return m_gl;
}

void Transform::setGl(QOpenGLFunctions *gl)
{
  m_gl = gl;
}

void Transform::XYToRaDec(double sx, double sy, double &ra, double &dec)
{
  double x =  2.0 * sx / m_width - 1;
  double y = -2.0 * sy / m_height + 1;

  QMatrix4x4 viewProjectionInverse = m_projMatrix * m_matViewNoPrecess;

  viewProjectionInverse = viewProjectionInverse.inverted();

  QVector3D point = QVector3D(x, y, 0);
  QVector3D world = viewProjectionInverse * point;

  double tx  = atan2(world.x(), world.z()) - R180;
  double ty  = atan2(world.y(), sqrt(world.x() * world.x() + world.z() * world.z()));

  SkMath::range(tx, R360);

  ra = tx;
  dec = ty;
}


