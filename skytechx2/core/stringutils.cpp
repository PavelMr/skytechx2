#include "stringutils.h"

StringUtils::StringUtils()
{
}

QString StringUtils::addComma(int value)
{
  QString number = QString::number(value);

  int i = number.size() - 3;

  if (i >= 0)
  {
    while(i > 0)
    {
      number.insert(i, "'");
      i -= 3;
    }
  }


  return number;
}

