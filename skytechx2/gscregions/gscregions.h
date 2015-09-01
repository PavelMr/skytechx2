#ifndef GscRegion_H
#define GscRegion_H

#include <QtCore>
#include "bbox.h"
#include "plane.h"

#define NUM_GSC_REGS         9537

#pragma pack(4)

typedef struct
{
  double p[4][3];
  double RaMin, DecMin;
  double RaMax, DecMax;
} gscRegion_t;

#pragma pack()

typedef struct regNode_s
{
  struct regNode_s *child[8];
  BBox              bbox;
  QList <int>       tRegList;
} regNode_t;


class GscRegion
{
  public:
    GscRegion(int depth = 3);
    void loadRegions(void);
    void checkVisibleRegions(SKPLANE *frustum);
    bool isRegionVisible(int reg, SKPLANE *frustum);
    QList<int> *getVisibleRegions();
    gscRegion_t *getRegion(int reg);

    BBox        gscRegionBBox[NUM_GSC_REGS];
    gscRegion_t gscRegionSector[NUM_GSC_REGS];
    uchar       rendered[NUM_GSC_REGS];

  protected:
    void createOcTree(void);
    void createOcTreeRec(regNode_t *node, int depth);
    regNode_t *createNode(const Vector3 *pos, const Vector3 &size);
    void getVisibleRec(regNode_t *node);

    regNode_t   *m_head;
    SKPLANE     *m_frustum;
    int          m_ocTreeDepth;
    QList <int>  m_visList;
    QList <int>  m_visList2;
};

//extern GscRegion GscRegion;

#endif // GscRegion_H
