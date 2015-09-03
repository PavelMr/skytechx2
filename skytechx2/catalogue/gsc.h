#ifndef GSC_H
#define GSC_H

#include "transform.h"
#include "gscregions.h"

#include <QtGlobal>

#include <QOpenGLBuffer>

typedef struct
{
  int    len, vers, region, nobj;
  double amin, amax, dmin, dmax, magoff;
  double scale_ra, scale_dec, scale_pos, scale_mag;
  int    npl;
  char  *list;
} gscHeader_t;


typedef struct
{
  RaDec  rd;
  short  reg, id;
  float  posErr;
  float  pMag;
  float  eMag;
  char   magBand;   // 0 - 18
  char   oClass;    // 0 - 3
  char   plate[5];
  double epoch;
  char   mult;  // TF
} gscStar_t;

typedef struct
{
    gscHeader_t  h;
      gscStar_t *gsc;
         qint64  m_timer;
  QOpenGLBuffer  m_buffer;
            int  m_size;
} gscStarRegion_t;

class GSC
{
public:
  GSC();

  gscStarRegion_t *getRegion(int region);
  int getMemoryUsage();
  int getRegionUsed();
  void clearCache();

private:
  bool loadRegion(int index, gscStarRegion_t *region);
  void decode(BYTE *c, gscHeader_t *h, gscStar_t *r);
  void deleteOldestRegion();

  QMap <int, gscStarRegion_t> m_regionMap;
  bool m_tableLoaded;
  long m_gscTable[NUM_GSC_REGS];

  char m_tmp[512];

  int m_memoryUsage;
};

#endif // GSC_H
