#include "skfile.h"
#include "define.h"

SkFile::SkFile(const QString fileName) : QFile(fileName)
{
}

SkFile::~SkFile()
{
#ifdef _DEBUG_SKFILE_
  //qDebug() << "SkFile (~)" << fileName() << openMode();
#endif
}

bool SkFile::open(OpenMode flag)
{
  bool rtv;

  rtv = QFile::open(flag);

#ifdef _DEBUG_SKFILE_
  //qDebug() << "Open file" << fileName() << openMode() << rtv;
#endif

  return rtv;
}

void SkFile::close()
{
#ifdef _DEBUG_SKFILE_
  //qDebug() << "Close file" << fileName() << openMode();
#endif
  QFile::close();
}

