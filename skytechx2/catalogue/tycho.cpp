#include "tycho.h"
#include "skfile.h"

typedef struct
{
  int reg;
  int no;
} hd_t;

static hd_t hdCross[360000];

static const char greekChar[25] = {'a','b','g','d','e','z','h','J','i','k',
                                   'l','m','n','x','o','p','r','s','t','u',
                                   'j','c','y','w','?'};


Tycho::Tycho()
{
}

bool Tycho::load()
{
  SkFile file("resources/stars/tycho2/tycho2.dat");

  if (!file.open(SkFile::ReadOnly))
  { // fatal error
    return false;
  }

  file.read((char *)&m_head, sizeof(m_head));

  if (m_head.id[0] != 'T' ||
      m_head.id[1] != 'Y' ||
      m_head.id[2] != 'C' ||
      m_head.id[3] != '2')
  {
    return false;
  }

  m_region = (tychoRegion2_t *)malloc(m_head.regCount * sizeof(tychoRegion2_t));
  m_supplement = (tychoSupp_t *)malloc(m_head.numSupplements * sizeof(tychoSupp_t));

  qint64 pos = file.pos();

  file.seek(m_head.offSupplements);
  file.read((char *)m_supplement, m_head.numSupplements * sizeof(tychoSupp_t));

  file.seek(m_head.offNames);
  m_names = file.read(m_head.numNames);

  file.seek(pos);
  int maxhd = 0;

  for (int i = 0; i < m_head.regCount; i++)
  {
    tychoRegion_t  reg;

    file.read((char *)&reg, sizeof(reg));

    m_region[i].region.numStars = reg.numStars;
    m_region[i].stars = (tychoStar_t *)malloc(reg.numStars * sizeof(tychoStar_t));

    file.read((char *)m_region[i].stars, sizeof(tychoStar_t) * reg.numStars);

    for (int j = 0; j < reg.numStars; j++)
    {
      if (m_region[i].stars[j].supIndex >= 0)
      {
        int supp = m_region[i].stars[j].supIndex;
        int hd = m_supplement[supp].hd;

        if (hd >= 1 && hd <= 359083)
        { // max. HD je 359083
          hdCross[m_supplement[supp].hd].reg = i;
          hdCross[m_supplement[supp].hd].no = j;
        }
        else if (hd > maxhd) maxhd = hd;

        if (m_supplement[supp].pnOffs != 0xffff)
        {
          tNames.append(&m_region[i].stars[j]);
        }
      }
    }
  }

  return true;
}

QString Tycho::getBayerString(tychoSupp_t *supp, bool &found)
{
  if (supp->ba[0] == 0)
  {
    found = false;
    return("");
  }

  QString str = QString((greekChar[supp->ba[0] - 1]));

  if (supp->ba[1] != 0)
    str += supp->ba[1] + '0';

  found = true;
  return(str);
}



QString Tycho::getFlamsteedString(tychoSupp_t *supp, bool &found)
{
  if (supp->fl == 0)
  {
    found = false;
    return("");
  }

  found = true;
  return(QString::number(supp->fl));
}


QString Tycho::getStarName(tychoSupp_t *supp)
{
  QString str;

  if (supp->pnOffs == 0xffff)
    return("");

  for (int i = supp->pnOffs; ; i++)
  {
    char ch = m_names.at(i);
    if (ch == '\0')
      break;
    str += m_names.at(i);
  }

  return(str);
}



float Tycho::getMagnitude(tychoStar_t &s)
{
  return TYC_SHORT_TO_MAG(s.BTmag);//0.850 * (TYC_SHORT_TO_MAG(s.BTmag) - TYC_SHORT_TO_MAG(s.VTmag));
}
