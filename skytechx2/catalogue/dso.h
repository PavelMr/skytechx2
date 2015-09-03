#ifndef DSO_H
#define DSO_H

#include "dso_def.h"

class DSO
{
public:
  DSO();
  void load();

public:
  dsoHeader_t m_dsoHead;
};

#endif // DSO_H
