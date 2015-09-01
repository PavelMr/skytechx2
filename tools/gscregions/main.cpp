#include <QtCore>

#define NUM_GSC_REGS      9537

#define MPI 3.14159265358979323846

#define INVRAD        (1 / (180.0 / MPI))

#pragma pack(4)

typedef struct
{
  double p[4][3];
  double RaMin, DecMin;
  double RaMax, DecMax;
} sector_t;

typedef struct
{
  double Ra, Dec;
} radec_t;

#pragma pack()

long gscTable[NUM_GSC_REGS];
sector_t gscRegionSector[NUM_GSC_REGS];

/////////////////////////////////
int readDouble(double *d,QFile *f)
/////////////////////////////////
{
  char tmp[32];
  char c;
  int num = 0;
  int q = 0;

  while (f->getChar(&c), c != ' ' || q == 0)
  {
    if (c != ' ') q = 1;
    tmp[num] = c;
    num++;
    if (num >= 32) return(0);
  }
  tmp[num] = 0;
  *d = atof(tmp);

  return(1);
}

////////////////////////////////////////////
int readDoublePoint(double *d,QFile *f,int p)
////////////////////////////////////////////
{
  char tmp[32];
  char c;
  int num = 0;
  int pp = 0;
  int np = -1;
  int q = 0;

  while (f->getChar(&c), c != ' ' || q == 0)
  {
    if (c != ' ') q = 1;
    tmp[num] = c;
    num++;
    if (num >= 32) return(0);
    if (c == '.') pp = 1;
    if (pp)
    {
      np++;
      if (np == p) break;
    }
  }

  tmp[num] = 0;
  *d = atof(tmp);

  return(1);
}


void trfRaDecToPointNoCorect(radec_t *rd, double *p)
{
  /*
  double cd = cos(rd->Dec);

  p[0] = cd * cos(rd->Ra);
  p[1] = cd * sin(rd->Ra);
  p[2] = sin(rd->Dec);
  */

  double ra = rd->Ra - (MPI * 0.5);
  double cd = cos(rd->Dec);

  p[0] = -cos(ra) * cd;
  p[1] = sin(rd->Dec);
  p[2] = sin(ra) * cd;
}


////////////////////
int readGSCRegions()
////////////////////
{
  int a;
  double reg,d,h,m,s,RaMin,RaMax,DecMin,DecMax;
  radec_t rd;
  double  p[3];

  QFile f("regions.tbl");
  if (!f.open(QFile::ReadOnly))
    return(false);

  f.seek(0x3840);

  for (a=0;a<NUM_GSC_REGS;a++)
  {
    // region number
    readDouble(&reg, &f);
    //gscRegion[a].loaded = FALSE;

    // RaMin
    readDouble(&h,&f);
    readDouble(&m,&f);
    readDouble(&s,&f);
    RaMin = (h + (m / 60.0) + (s/3600.0)) * 15 * INVRAD;

    // RaMax
    readDouble(&h,&f);
    readDouble(&m,&f);
    readDouble(&s,&f);
    RaMax = (h + (m / 60.0) + (s/3600.0)) * 15 * INVRAD;

    // DecMin
    readDouble(&d,&f);
    readDouble(&m,&f);
    DecMin = (fabs(d) + (m / 60.0)) * INVRAD;
    if (d < 0) DecMin = -DecMin;

    // DecMax
    readDouble(&d,&f);
    readDoublePoint(&m,&f,1);
    DecMax = (fabs(d) + (m / 60.0)) * INVRAD;
    if (d < 0) DecMax = -DecMax;


    gscRegionSector[a].RaMin = RaMin;
    gscRegionSector[a].RaMax = RaMax;
    gscRegionSector[a].DecMin = DecMin;
    gscRegionSector[a].DecMax = DecMax;

    rd.Ra = RaMin;
    rd.Dec = DecMax;
    trfRaDecToPointNoCorect(&rd, p);

    gscRegionSector[a].p[0][0] = p[0];
    gscRegionSector[a].p[0][1] = p[2];
    gscRegionSector[a].p[0][2] = p[1];

    rd.Ra = RaMax;
    rd.Dec = DecMax;
    trfRaDecToPointNoCorect(&rd, p);

    gscRegionSector[a].p[1][0] = p[0];
    gscRegionSector[a].p[1][1] = p[2];
    gscRegionSector[a].p[1][2] = p[1];

    rd.Ra = RaMax;
    rd.Dec = DecMin;
    trfRaDecToPointNoCorect(&rd, p);

    gscRegionSector[a].p[2][0] = p[0];
    gscRegionSector[a].p[2][1] = p[2];
    gscRegionSector[a].p[2][2] = p[1];

    rd.Ra = RaMin;
    rd.Dec = DecMin;
    trfRaDecToPointNoCorect(&rd, p);

    gscRegionSector[a].p[3][0] = p[0];
    gscRegionSector[a].p[3][1] = p[2];
    gscRegionSector[a].p[3][2] = p[1];
  }

  f.close();

  return(true);
}


void save(void)
{
  int count = NUM_GSC_REGS;

  QFile f("gscregions.dat");

  f.open(QFile::WriteOnly);

  f.write((char *)&count, sizeof(int));
  f.write((char *)gscRegionSector, sizeof(gscRegionSector));

  f.close();
}


int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  readGSCRegions();
  save();

  return a.exec();
}
