#include "dso.h"
#include "skfile.h"

DSO::DSO()
{
}

void DSO::load()
{
  SkFile pf("resources/dso/dso.dat");

  if (!pf.open(SkFile::ReadOnly))
  {
    return;
  }

  pf.read((char *)&m_dsoHead.id, 4);
  pf.read((char *)&m_dsoHead.numDso, sizeof(qint32));
  pf.read((char *)&m_dsoHead.textSegSize, sizeof(qint32));
  pf.read((char *)&m_dsoHead.galClassSize, sizeof(qint32));
  pf.read((char *)&m_dsoHead.catNamesSize, sizeof(qint32));


}

