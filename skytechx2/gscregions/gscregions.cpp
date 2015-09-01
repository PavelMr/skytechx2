#include "gscregions.h"
#include "skfile.h"

GscRegion::GscRegion(int depth)
{
  m_head = NULL;
  m_ocTreeDepth = depth;
}

void GscRegion::checkVisibleRegions(SKPLANE *frustum)
{
  m_visList.clear();
  m_visList2.clear();
  m_frustum = frustum;
  getVisibleRec(m_head);

  memset(rendered, 0, sizeof(rendered));

  foreach (int i, m_visList)
  {
    if (rendered[i])
    {
      continue;
    }
    rendered[i] = true;
    if (!isRegionVisible(i, frustum))
    {
      continue; // region already rendered
    }
    m_visList2.append(i);
  }
}

bool GscRegion::isRegionVisible(int reg, SKPLANE *frustum)
{
  return(gscRegionBBox[reg].checkFrustum(frustum));
}

QList<int> *GscRegion::getVisibleRegions()
{
  return &m_visList2;
}

gscRegion_t *GscRegion::getRegion(int reg)
{
  return(&gscRegionSector[reg]);
}

regNode_t *GscRegion::createNode(const Vector3 *pos, const Vector3 &size)
{
  regNode_t *node = new regNode_t;

  for (int i = 0; i < 8; i++)
  {
    node->child[i] = NULL;
  }

  node->bbox.reset();
  node->bbox.addPt((double *)pos);
  node->bbox.addPt(pos->x + size.x, pos->y + size.y, pos->z + size.z);

  return(node);
}

void GscRegion::getVisibleRec(regNode_t *node)
{
  if (node->child[0] == NULL)
  { // add to list;
    m_visList.append(node->tRegList);
    return;
  }

  for (int i = 0; i < 8; i++)
  {
    if (node->child[i]->bbox.checkFrustum(m_frustum))
    { // visible
      getVisibleRec(node->child[i]);
    }
  }
}

void GscRegion::createOcTreeRec(regNode_t *node, int depth)
{
  if (depth == m_ocTreeDepth)
  {
    for (int i = 0; i < NUM_GSC_REGS; i++)
    {
      if (node->bbox.intersect(gscRegionBBox[i]))
      {
        node->tRegList.append(i);
      }
    }
    return;
  }

  Vector3 size;

  size.x = node->bbox.size(0) / 2.0f;
  size.y = node->bbox.size(1) / 2.0f;
  size.z = node->bbox.size(2) / 2.0f;

  Vector3 vec0(node->bbox.mins[0], node->bbox.mins[1], node->bbox.mins[2]);
  Vector3 vec1(node->bbox.mins[0], node->bbox.mins[1], node->bbox.mins[2] + size.z);
  Vector3 vec2(node->bbox.mins[0] + size.x, node->bbox.mins[1], node->bbox.mins[2] + size.z);
  Vector3 vec3(node->bbox.mins[0] + size.x, node->bbox.mins[1], node->bbox.mins[2]);
  Vector3 vec4(node->bbox.mins[0], node->bbox.mins[1] + size.y, node->bbox.mins[2]);
  Vector3 vec5(node->bbox.mins[0], node->bbox.mins[1] + size.y, node->bbox.mins[2] + size.z);
  Vector3 vec6(node->bbox.mins[0] + size.x, node->bbox.mins[1] + size.y, node->bbox.mins[2] + size.z);
  Vector3 vec7(node->bbox.mins[0] + size.x, node->bbox.mins[1] + size.y, node->bbox.mins[2]);

  node->child[0] = createNode(&vec0, size);
  node->child[1] = createNode(&vec1, size);
  node->child[2] = createNode(&vec2, size);
  node->child[3] = createNode(&vec3, size);

  node->child[4] = createNode(&vec4, size);
  node->child[5] = createNode(&vec5, size);
  node->child[6] = createNode(&vec6, size);
  node->child[7] = createNode(&vec7, size);

  for (int i = 0; i < 8; i++)
  {
    createOcTreeRec(node->child[i], depth + 1);
  }
}


void GscRegion::createOcTree(void)
{
  Vector3 vec(-1.1, -1.1, -1.1);
  m_head = createNode(&vec, Vector3(2.2, 2.2, 2.2));
  createOcTreeRec(m_head, 0);
}



void GscRegion::loadRegions(void)
{
  int count;

  SkFile f("resources/regions/gscregions.dat");

  f.open(SkFile::ReadOnly);

  f.read((char *)&count, sizeof(int));
  f.read((char *)gscRegionSector, sizeof(gscRegionSector));

  f.close();

  for (int i = 0; i < NUM_GSC_REGS; i++)
  {
    gscRegionBBox[i].reset();
    for (int j = 0; j < 4; j++)
    {
      gscRegionBBox[i].addPt(gscRegionSector[i].p[j][0],
                             gscRegionSector[i].p[j][2],
                             gscRegionSector[i].p[j][1]);
    }
  }

  createOcTree();
}

