#ifndef PLANE_H
#define PLANE_H

#include "vecmath.h"

typedef struct
{
  double x, y, z;
  double dist;
} SKPLANE;

SKPLANE *SKPLANEFromPoint(SKPLANE *out, Vector3 *a, Vector3 *b, Vector3 *c);

bool SKPLANECheckFrustumToPoint(SKPLANE *frustum, Vector3 *point);
bool SKPLANECheckFrustumToSphere(SKPLANE *frustum, Vector3 *point, double radius);
void SKPLANENormalize(SKPLANE *plane);
bool SKPLANECheckFrustumToPolygon(SKPLANE *frustum, Vector3 *pts, int count, double offset = 0);



#endif // PLANE_H
