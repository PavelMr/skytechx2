#ifndef SKSTRING_H
#define SKSTRING_H

#include <QObject>
#include <QString>
#include <QtGlobal>

class SkString : public QObject
{
public:
  SkString();

  static QString getMagnitude(double mag, bool prependSpace)
  {
    QString str;

    if (qIsNaN(mag))
    {
      return str;
    }

    if (prependSpace)
    {
      str += " ";
    }

    return str += QString::number(mag, 'f', 2) + tr(" mag.");
  }
};

#endif // SKSTRING_H
