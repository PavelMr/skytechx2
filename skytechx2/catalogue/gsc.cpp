#include "gsc.h"
#include "skfile.h"
#include "skmath.h"

#include <QDateTime>

static int l_band[10] = {0, 1, 6, 8, 10, 11, 12, 13, 14, 18};

GSC::GSC() :
  m_tableLoaded(false)
{
}

gscStarRegion_t *GSC::getRegion(int region)
{
  if (m_regionMap.contains(region))
  {
    return &m_regionMap[region];
  }

  gscStarRegion_t gscRegion;

  if (loadRegion(region, &gscRegion))
  {
    if (m_regionMap.size() > 100)
    {
      deleteOldestRegion();
    }

    gscRegion.timer = QDateTime::currentMSecsSinceEpoch();

    m_regionMap[region] = gscRegion;
    return &m_regionMap[region];
  }

  return NULL;
}

bool GSC::loadRegion(int index, gscStarRegion_t *region)
{
  gscHeader_t *header;
  char *tmp;
  int lheader;

  header = &region->h;

  SkFile file("resources/stars/gsc/gsc12.dat");
  if (!file.open(SkFile::ReadOnly))
   {
     qDebug("FN_GSCMAINFILE not found!!!\n");
     return false;
   }

  if (!m_tableLoaded)
  {
    file.read((char *)&m_gscTable, sizeof(m_gscTable));
    m_tableLoaded = true;
  }

  file.seek(m_gscTable[index]);

  tmp = m_tmp;

  memset(header, 0, sizeof(gscHeader_t));
  memset(m_tmp, 0, sizeof(m_tmp));

  if (file.read((char *)tmp, 4) != 4)
  {
    return false;
  }

  lheader = atoi(tmp);
  tmp += 4;
  file.read((char *)tmp, lheader - 4);
  tmp[lheader] = '\0';

  header->len = lheader;
  header->vers = atoi(tmp);
  while(*++tmp != ' ');
  header->region = atoi(tmp);
  while(*++tmp != ' ');
  header->nobj = atoi(tmp);
  while(*++tmp != ' ');
  header->amin = atof(tmp);
  while(*++tmp != ' ');
  header->amax = atof(tmp);
  while(*++tmp != ' ');
  header->dmin = atof(tmp);
  while(*++tmp != ' ');
  header->dmax = atof(tmp);
  while(*++tmp != ' ');
  header->magoff = atof(tmp);
  while(*++tmp != ' ');
  header->scale_ra = atof(tmp);
  while(*++tmp != ' ');
  header->scale_dec = atof(tmp);
  while(*++tmp != ' ');
  header->scale_pos = atof(tmp);
  while(*++tmp != ' ');
  header->scale_mag = atof(tmp);
  while(*++tmp != ' ');
  header->npl = atoi(tmp);
  while(*++tmp != ' ');
  header->list = tmp;

  region->gsc = (gscStar_t *)malloc(sizeof(gscStar_t) * region->h.nobj);

  unsigned char c[12];

  for (int i = 0; i < region->h.nobj; i++)
  {
    file.read((char *)c, 12);
    decode(c, header, &region->gsc[i]);
  }

  return true;
}

void GSC::decode(BYTE *c, gscHeader_t *h, gscStar_t *r)
{
  int id;
  int da,dd,dp,mag,dm,pl,mul;
  int ba;
  char *p;

  id = (c[0] & 127);
  id <<= 7;
  id |= (c[1] >> 1);
  r->id = id;

  da = ((c[1]&1)<< 8) | c[2];
  da <<= 8;
  da |= c[3];
  da <<= 8;
  da |= c[4];
  da >>= 3;

  dd = c[4] & 7;
  dd <<= 8;
  dd |= c[5];
  dd <<= 8;
  dd |= c[6];

  dp = c[7];
  dp <<= 1;
  dp |= c[8] >> 7;

  mag = c[9];
  mag <<= 3;
  mag |= c[10] >> 5;

  dm = c[8] & 127;

  ba = (c[10] >> 1) & 15;
  r->magBand = l_band[ba];

  r->oClass = (c[11] >> 4) & 7;

  pl = c[11] & 15;
  p = h->list;
  strncpy(r->plate, p+(5*pl) + 1, 4);
  r->plate[4] = '\0';

  p = h->list + 5 * h->npl;
  p += 9 * pl ;
  if (isdigit(p[1]))
  {
    r->epoch = atof(p + 1) - 2000.0;
  }
  else
  {
    r->epoch = 0;
  }

  mul = c[10] & 1;

  if(mul == 0)
  {
     r->mult = 'F';
  }
  else
  if (mul == 1)
  {
    r->mult = 'T';
  }

  r->rd.ra = SkMath::toRad(da / h->scale_ra + h->amin);
  r->rd.dec = SkMath::toRad(dd / h->scale_dec + h->dmin);

  SkMath::range(r->rd.ra, R360);

  r->posErr = (float)(dp / h->scale_pos);
  r->pMag = (float)(mag / h->scale_mag +h->magoff);
  r->eMag = (float)(dm / h->scale_mag);
  r->reg = h->region;
}

void GSC::deleteOldestRegion()
{
  QMap <int, gscStarRegion_t>::iterator i;
  int key = -1;
  qint64 oldestTime = std::numeric_limits<qint64>::max();

  for (i = m_regionMap.begin(); i != m_regionMap.end(); ++i)
  {
    if (i.value().timer < oldestTime)
    {
      oldestTime = i.value().timer;
      key = i.key();
    }
  }

  if (key >= 0)
  {
    m_regionMap.remove(key);
  }
  else
  {
    qDebug() << "deleteOldestRegion() failed()";
  }

}



