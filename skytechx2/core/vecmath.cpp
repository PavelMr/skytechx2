#include <math.h>

#include "vecmath.h"

/////////////////////////////////
double SKVECLength(Vector3 *vec)
/////////////////////////////////
{
  double d = vec->x * vec->x +
             vec->y * vec->y +
             vec->z * vec->z;

  return(sqrt(d));
}


/////////////////////////////////////
double SKVECLengthSqrt(Vector3 *vec)
/////////////////////////////////////
{
  double d = vec->x * vec->x +
             vec->y * vec->y +
             vec->z * vec->z;

  return(d);
}

//////////////////////////////////////////////////////
Vector3 *SKVecNormalize(Vector3 *out, Vector3 *vec)
//////////////////////////////////////////////////////
{
  double len = SKVECLength(vec);
  double iLen;

  if (len == 0)
  {
    out->x = 0;
    out->y = 0;
    out->z = 0;

    return(out);
  }
  else
  {
    iLen = 1 / len;
  }

  out->x = vec->x * iLen;
  out->y = vec->y * iLen;
  out->z = vec->z * iLen;

  return(out);
}


/////////////////////////////////////////////////////////////
Vector3 *SKVecAdd(Vector3 *out, Vector3 *a1, Vector3 *a2)
/////////////////////////////////////////////////////////////
{
  out->x = a1->x + a2->x;
  out->y = a1->y + a2->y;
  out->z = a1->z + a2->z;

  return(out);
}


/////////////////////////////////////////////////////////////
Vector3 *SKVecSub(Vector3 *out, const Vector3 &a1, const Vector3 &a2)
/////////////////////////////////////////////////////////////
{
  out->x = a1.x - a2.x;
  out->y = a1.y - a2.y;
  out->z = a1.z - a2.z;

  return(out);
}


///////////////////////////////////////////
double SKVecDot(Vector3 *a1, Vector3 *a2)
///////////////////////////////////////////
{
  double d = a1->x * a2->x +
             a1->y * a2->y +
             a1->z * a2->z;
  return(d);
}


///////////////////////////////////////////////////////////////
Vector3 *SKVecCross(Vector3 *out, Vector3 *a1, Vector3 *a2)
///////////////////////////////////////////////////////////////
{
  out->x = a1->y * a2->z - a1->z * a2->y;
  out->y = a1->z * a2->x - a1->x * a2->z;
  out->z = a1->x * a2->y - a1->y * a2->x;

  return(out);
}


/////////////////////////////////////////////////////////////////////
Vector3 *SKVECTransform(Vector3 *out, Vector3 *vec, QMatrix4x4 &mat)
/////////////////////////////////////////////////////////////////////
{
  double px = vec->x * mat(0, 0) +
              vec->y * mat(1, 0) +
              vec->z * mat(2, 0) +
                       mat(3, 0);

  double py = vec->x * mat(0, 1) +
              vec->y * mat(1, 1) +
              vec->z * mat(2, 1) +
                       mat(3, 1);

  double pz = vec->x * mat(0, 2) +
              vec->y * mat(1, 2) +
              vec->z * mat(2, 2) +
                       mat(3, 2);

  double pw = vec->x * mat(0, 3) +
              vec->y * mat(1, 3) +
              vec->z * mat(2, 3) +
                       mat(3, 3);

  double iw = 1 / pw;

  out->x = px * iw;
  out->y = py * iw;
  out->z = pz * iw;

  return(out);
}


//////////////////////////////////////////////////////////////////////
Vector3 *SKVECTransform3(Vector3 *out, Vector3 *vec, QMatrix4x4 &mat)
//////////////////////////////////////////////////////////////////////
{
  double px = vec->x * mat(0, 0) +
              vec->y * mat(1, 0) +
              vec->z * mat(2, 0) +
                       mat(3, 0);

  double py = vec->x * mat(0, 1) +
              vec->y * mat(1, 1) +
              vec->z * mat(2, 1) +
                       mat(3, 1);

  double pz = vec->x * mat(0, 2) +
              vec->y * mat(1, 2) +
              vec->z * mat(2, 2) +
                       mat(3, 2);

  out->x = px;
  out->y = py;
  out->z = pz;

  return(out);
}
