#ifndef VECMATH_H
#define VECMATH_H

#include <QMatrix4x4>

class Vector3
{
public:
  Vector3() {}
  Vector3(double vx, double vy, double vz)
  {
    x = vx;
    y = vy;
    z = vz;
  }
  QVector3D toQVector()
  {
    return QVector3D(x, y, z);
  }

  double x, y, z;
};

double SKVECLength(Vector3 *vec);
double SKVECLengthSqrt(Vector3 *vec);

Vector3 *SKVecNormalize(Vector3 *out, Vector3 *vec);

Vector3 *SKVecAdd(Vector3 *out, Vector3 *a1, Vector3 *a2);
Vector3 *SKVecSub(Vector3 *out, const Vector3 &a1, const Vector3 &a2);

Vector3 *SKVECTransform(Vector3 *out, Vector3 *vec, QMatrix4x4 &mat);
Vector3 *SKVECTransform3(Vector3 *out, Vector3 *vec, QMatrix4x4 &mat);

Vector3 *SKVECProject(double x, double y, double z, QMatrix4x4 *m, Vector3 *out);

double SKVecDot(Vector3 *a1, Vector3 *a2);
Vector3 *SKVecCross(Vector3 *out, Vector3 *a1, Vector3 *a2);

#endif // VECMATH_H
