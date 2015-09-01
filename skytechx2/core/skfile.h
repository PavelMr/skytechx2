#ifndef SKFILE_H
#define SKFILE_H

#include <QFile>

class SkFile : public QFile
{
public:
  SkFile(const QString fileName);
 ~SkFile();
  bool open(OpenMode flag);
  void close();
};

#endif // SKFILE_H
