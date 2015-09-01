#include "transform.h"
#include "skmath.h"

#include <math.h>
#include <GL/glu.h>

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

  projection.perspective(SkMath::toDeg(param.m_fov), width / height, 0.01, 1.1);
  projectionFrustum.perspective(SkMath::toDeg(param.m_fov * 1.2), width / height, 0.01, 1.1);

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

/* transformation from cartesian to spherical coordinates */
void cartsph (
double x, double y, double z,		/* source: rectangular coordinates */
double *l, double *b, double *r)	/* result: spherical coordinates */
{
  double rho = x*x + y*y;

  if (rho > 0) {	/* standard case: off axis */
      *l = atan2(y, x);
      SkMath::range(*l, 2* M_PI);
      *b = atan2(z, sqrt(rho));
      *r = sqrt(rho + z*z);
  } else {		/* degenerate case; avoid math error */
      *l = 0.0;
      if (z == 0.0)
    *b = 0.0;
      else
    *b = (z > 0.0) ? M_PI/2. : -M_PI/2.;
      *r = fabs(z);
  }
}

void Transform::XYToRaDec(double sx, double sy, double &ra, double &dec)
{
  Vector3 v;
  Vector3 out;

  v.x = ( ( ( 2.0f * sx ) / m_width ) - 1.0 ) / m_projMatrix(1, 1);
  v.y = ( ( ( 2.0f * sy ) / m_height) - 1.0 ) / m_projMatrix(0, 0);
  v.z = 1.0;

  //qDebug() << v.x << v.y;

  QMatrix4x4 m = m_matViewNoPrecess;//.inverted();

  out.x = v.x * m(0, 0) + v.y * m(1, 0) + v.z * m(2, 0);
  out.y = v.x * m(0, 1) + v.y * m(1, 1) + v.z * m(2, 1);
  out.z = v.x * m(0, 2) + v.y * m(1, 2) + v.z * m(2, 2);

  //qDebug() << out.x << out.y << out.z;

  double tx  =  atan2(out.z, out.x) - R90;
  double ty  = -atan2(out.y, sqrt(out.x * out.x + out.z * out.z));

  SkMath::range(tx, R360);

  ra = tx;
  dec = ty;

  qDebug() << SkMath::toDeg(tx) << SkMath::toDeg(ty);


  /*
  float mouseX = -( ( ( 2.0f * sx ) / m_width ) - 1.0 ) / m_projMatrix(0, 0);
  float mouseY = -( ( ( 2.0f * sy ) / m_height) - 1.0 ) / m_projMatrix(1, 1);

  //mouseX = 0;//sx / (m_width  * 0.5f) - 1.0f;
  //mouseY = 0;//sy / (m_height * 0.5f) - 1.0f;

  //qDebug() << mouseX << mouseY;

  QMatrix4x4 matrix = m_matViewNoPrecess;
  QMatrix4x4 invVP = matrix.inverted();
  QVector3D screenPos = QVector3D(mouseX, mouseY, 1);
  QVector3D out = invVP * screenPos;
  QVector3D out2;

  out2 = out;//QVector3D(out.z(), -out.x(), out.y());

//  qDebug() << out;
  //qDebug() << out2;

  double tx, ty, tz;

  //out2.normalize();

  cartsph(out2.x(), out2.y(), out2.z(), &tx, &ty, &tz);

  //qDebug() << SkMath::toDeg(tx) << SkMath::toDeg(ty) << tz << sqrt(pow(out2.x(), 2) + pow(out2.y(),2) + pow(out2.z(),2));

  ra = tx;
  dec = ty;

   //*/

  /*
  // Compute the vector of the pick ray in screen space
  Vector3 v;
  Vector3 out;

  qDebug() << "----------------------";

  qDebug() << sx << sy;

  v.x = ( ( ( 2.0f * sx ) / m_width ) - 1.0 ) / m_projMatrix(0, 0);
  v.z = -( ( ( 2.0f * sy ) / m_height) - 1.0 ) / m_projMatrix(1, 1);
  v.y = 1.0f;

  qDebug() << v.x << v.y << v.z;

  // Get the inverse view matrix with no precession
  QMatrix4x4 m = m_matViewNoPrecess.inverted().transposed();

  //qDebug() << m;

  // Transform the screen space ray into 3D space
  out.x  = v.x * m(0, 0) + v.y * m(1, 0) + v.z * m(2, 0);
  out.y  = v.x * m(0, 1) + v.y * m(1, 1) + v.z * m(2, 1);
  out.z  = v.x * m(0, 2) + v.y * m(1, 2) + v.z * m(2, 2);

  qDebug() << out.x << out.y << out.z;

  QVector3D vv(v.x, v.y, v.z);

  vv = vv * m;
  //out = Vector3(vv.x(), vv.y(), vv.z());

  //rdToVector(RaDec(0.5, 0), out);

  //qDebug() << vv.x() << vv.y() << vv.z();
  //qDebug() << out.x << out.y << out.z;

  double tx  =  atan2(out.z, out.x) - R90;
  double ty  = -atan2(out.y, sqrt(out.x * out.x + out.z * out.z));

  SkMath::range(tx, R360);

  //if (currentMapView.epochJ2000 && currentMapView.coordType == SMCT_RA_DEC)
  //{
//    precess(&tx, &ty, JD2000, currentMapView.jd);
//  }

  ra = tx;
  dec = ty;
  */
}


