#include <math.h>

#include "plane.h"
#include "vecmath.h"

#define FRUSTUM_COUNT   5

enum
{
    polygonInterior = 1,
    polygonBoundary = 0,
    polygonExterior = -1
};

//////////////////////////////////////////////////////////////////////////////
SKPLANE *SKPLANEFromPoint(SKPLANE *out, Vector3 *a, Vector3 *b, Vector3 *c)
//////////////////////////////////////////////////////////////////////////////
{
  Vector3 d1, d2;
  Vector3 normal;

  SKVecSub(&d1, *b, *a);
  SKVecSub(&d2, *c, *a);

  SKVecCross(&normal, &d1, &d2);
  SKVecNormalize(&normal, &normal);

  out->dist = -(a->x * normal.x +
                a->y * normal.y +
                a->z * normal.z);

  out->x = normal.x;
  out->y = normal.y;
  out->z = normal.z;

  return(out);
}

//////////////////////////////////////////////////////////////////
bool SKPLANECheckFrustumToPoint(SKPLANE *frustum, Vector3 *point)
//////////////////////////////////////////////////////////////////
{
  for (int i = 0; i < FRUSTUM_COUNT; i++)
  {
    double dist = point->x * frustum[i].x +
                  point->y * frustum[i].y +
                  point->z * frustum[i].z + frustum[i].dist;
    if (dist < 0)
      return(false);
  }
  return(true);
}

//////////////////////////////////////////////////////////////////////////////////
bool SKPLANECheckFrustumToSphere(SKPLANE *frustum, Vector3 *point, double radius)
//////////////////////////////////////////////////////////////////////////////////
{
  // angle to size
  radius = tan(radius);

  for (int i = 0; i < FRUSTUM_COUNT; i++)
  {
    double dist = point->x * frustum[i].x +
                  point->y * frustum[i].y +
                  point->z * frustum[i].z + frustum[i].dist;
    if (dist < -radius)
      return(false);
  }
  return(true);
}


void SKPLANENormalize(SKPLANE *plane)
{
  double mag = sqrt(plane->x * plane->x + plane->y * plane->y + plane->z * plane->z);

  plane->x /= mag;
  plane->y /= mag;
  plane->z /= mag;
  plane->dist /= mag;
}


bool SKPLANECheckFrustumToPolygon(SKPLANE *frustum, Vector3 *pts, int count, double offset)
{
  for (int i = 0; i < FRUSTUM_COUNT; i++)
  {
    int p;
    for (p = 0; p < count; p++)
    {
      double dist = pts[p].x * frustum[i].x +
                    pts[p].y * frustum[i].y +
                    pts[p].z * frustum[i].z + frustum[i].dist;
      if (dist > offset)
        break;
    }
    if (p == count)
      return(false);
  }
  return(true);
}
