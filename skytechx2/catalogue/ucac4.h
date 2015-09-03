#ifndef UCAC4_H
#define UCAC4_H

#include "skfile.h"

#include <QMap>
#include <QVector>
#include <QOpenGLBuffer>

#pragma pack(1)

typedef struct
{
  int ra, spd;         /* RA/dec at J2000.0,  ICRS,  in milliarcsec */
  unsigned short mag1, mag2;     /* UCAC fit model & aperture mags, .001 mag */
  unsigned char mag_sigma;
  unsigned char obj_type, double_star_flag;
  char ra_sigma, dec_sigma;    /* sigmas in RA and dec at central epoch */
  unsigned char n_ucac_total;      /* Number of UCAC observations of this star */
  unsigned char n_ucac_used;      /* # UCAC observations _used_ for this star */
  unsigned char n_cats_used;      /* # catalogs (epochs) used for prop motion */
  unsigned short epoch_ra;        /* Central epoch for mean RA, minus 1900, .01y */
  unsigned short epoch_dec;       /* Central epoch for mean DE, minus 1900, .01y */
  short pm_ra;            /* prop motion, .1 mas/yr = .01 arcsec/cy */
  short pm_dec;           /* prop motion, .1 mas/yr = .01 arcsec/cy */
  char pm_ra_sigma;       /* sigma in same units */
  char pm_dec_sigma;
  unsigned int twomass_id;        /* 2MASS pts_key star identifier */
  unsigned short mag_j, mag_h, mag_k;  /* 2MASS J, H, K_s mags,  in millimags */
  unsigned char icq_flag[3];
  unsigned char e2mpho[3];          /* 2MASS error photometry (in centimags) */
  unsigned short apass_mag[5];      /* in millimags */
  unsigned char apass_mag_sigma[5]; /* also in millimags */
  unsigned char yale_gc_flags;      /* Yale SPM g-flag * 10 + c-flag */
  unsigned int catalog_flags;
  unsigned char leda_flag;          /* LEDA galaxy match flag */
  unsigned char twomass_ext_flag;   /* 2MASS extended source flag */
  unsigned int id_number;
  unsigned short ucac2_zone;
  unsigned int ucac2_number;
} UCAC4_Star_t;

#pragma pack()

typedef struct
{
  int                    m_size;
  QVector <UCAC4_Star_t> m_stars;
  qint64                 m_timer;
  QOpenGLBuffer          m_buffer;
} ucac4Region_t;

typedef struct
{
  int     ra;
  int     index;
  quint64 offset;
} ucac4AccFile_t;

class UCAC4
{
public:
  UCAC4();
  ucac4Region_t *getRegion(int region);
  int getMemoryUsage();
  int getRegionUsed();
  void clearCache();

private:
  QMap <int, ucac4Region_t> m_regionMap;
  QList <ucac4AccFile_t> m_accList;

  int m_memoryUsage;
  QString m_folder;

  bool readAccFile(SkFile &file);
  void deleteOldestRegion();
};

#endif // UCAC4_H
