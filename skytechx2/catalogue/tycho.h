#ifndef TYCHO_H
#define TYCHO_H

#include <QtCore>
#include <QtGui>

#define TYC_MAG_TO_SHORT(m)      (((m) * 1000) + 10000)
#define TYC_SHORT_TO_MAG(m)      (((m) - 10000) / (float)1000.0)

#pragma pack(4)

typedef struct
{
  uchar   id[4];             // TYC2
  int     regCount;          // GSC region count
  int     numStars;          // total number of stars
  int     numSupplements;
  int     offSupplements;
  int     offNames;
  int     numNames;
} tychoHead_t;

typedef struct
{
  int     regNo;             // GSC region number
  int     numStars;          // number of stars within region
} tychoRegion_t;

// spectral type
// O, B, A, F, G, K, M
// 1  2  3  4  5  6  7

typedef struct
{
  int      hd;       // HD cat. no.
  uchar    spt;      // spectral type
  uchar    spn;      // spectral number (char)
  uchar    fl;       // flamsteed number
  uchar    dummy;
  uchar    ba[2];    // bayer leter/number
  ushort   pnOffs;   // offset to proper name 0xffff = none
} tychoSupp_t;

typedef struct
{
  short     tyc1, tyc2;
  short     tyc3, dummy;
  long      supIndex;        // bayer flam. etc.  .. -1 no supp.
  short     BTmag, VTmag;
  double    ra;
  double    dec;
} tychoStar_t;

#pragma pack()

typedef struct
{
  tychoRegion_t  region;
  tychoStar_t   *stars;
} tychoRegion2_t;

#define TS_FLAMSTEED   0
#define TS_BAYER       1
#define TS_TYC         2
#define TS_HD          3

class Tycho : public QObject
{
  Q_OBJECT

  public:
    Tycho();
    bool load();

    tychoSupp_t             *m_supplement;
    QList   <tychoStar_t *>  tNames;         // list ptrs. to stars with proper name

    static QString getGreekChar(int i);
    static float getMagnitude(tychoStar_t &s)
    {
      float bt = TYC_SHORT_TO_MAG(s.BTmag);
      float vt = TYC_SHORT_TO_MAG(s.VTmag);

      return(vt - 0.09f * (bt - vt));
    }

    QString getStarName(tychoSupp_t *supp);
    QString getFlamsteedString(tychoSupp_t *supp, bool &found);
    QString getBayerString(tychoSupp_t *supp, bool &found);

    int getMemoryUsage();

public:
    tychoHead_t       m_head;
    tychoRegion2_t   *m_region;
    QByteArray        m_names;

signals:
};

#endif // TYCHO_H



