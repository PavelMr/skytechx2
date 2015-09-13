#ifndef SKMATH_H
#define SKMATH_H

#include <QtGlobal>

#include <math.h>

#define MPI            3.141592653589793
#define R45            (MPI * 0.25)
#define R90            (MPI * 0.5)
#define R180           (MPI)
#define R270           (R180 + R90)
#define R360           (MPI * 2)

#define DMS2RAD(d,m,s)      (double)SkMath::toRad((d) + ((m) / 60.0) + ((s) / 3600.0))
#define HMS2RAD(h,m,s)      (DMS2RAD(h, m, s) * 15.0)

#define D_UNDEF        qQNaN()
#define D_IS_UNDEF(v)  qIsNaN(v)

#define FRAC(f, from, to)     ((((f) - (from)) / (double)((to) - (from))))
#define CLAMP(v, mi, ma)       (((v) < (mi)) ? (mi) : ((v) > (ma)) ? (ma) : (v))
#define SIGN(x)                ((x) >= 0 ? 1.0 : -1.0)
#define LERP(f, mi, ma)        ((mi) + (f) * ((ma) - (mi)))

#define MAX_MAG_LIMIT       1000
#define NO_MAG_LIMIT       -1000

class SkMath
{
public:
  SkMath();

  static bool isNear(double val1, double val2, double tresshold = 0.000001)
  {
    return qAbs(val1 - val2) < tresshold;
  }

  static void range(double &val, double rng)
  {
    val -= floor(val / rng) * rng;
  }

  static double distance(double R1, double D1, double R2, double D2)
  {
    double D, H1, A, H2, H3, S1, C1, S;

    D = sin((D1 - D2) / 2);
    H1 = D * D;
    A = sin((R1 - R2) / 2);
    H2 = A * A;
    H3 = H1 + cos(D1) * cos(D2) * H2;
    S1 = sqrt(H3);
    C1 = sqrt(1 - S1 * S1);
    S = 2 * atan(S1 / C1);

    return S;
  }

  static inline double toDeg(double rad)
  {
    return rad * (180 / MPI);
  }

  static inline double toRad(double deg)
  {
    return deg * (1 / (180 / MPI));
  }
};

#endif // MATH_H
