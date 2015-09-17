#ifndef XMLSETTING_H
#define XMLSETTING_H

#include <QString>
#include <QXmlDefaultHandler>

class XMLSettingParser : public QXmlDefaultHandler
{
public:
  explicit XMLSettingParser();

  bool begin(const QString& fileName);

  QMap <QString, QString> map();

private:

  bool error(const QXmlParseException &exception);
  bool characters(const QString &str);
  bool endElement(const QString &namespaceURI, const QString &localName, const QString &qName);
  bool startElement(const QString &namespaceURI,
                        const QString &localName,
                        const QString &qName,
                        const QXmlAttributes &attributes);

private:
  QMap <QString, QString> m_map;

};



class XmlSetting
{
public:
  XmlSetting();
  QString get(const QString &name);
  QString operator()(const QString &name);
  bool load(const QString fileName);

private:
  QMap <QString, QString> m_map;
};

#endif // XMLSETTING_H
