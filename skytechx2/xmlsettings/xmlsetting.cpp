#include "xmlsetting.h"

#include <QDebug>

XmlSetting::XmlSetting()
{

}

QString XmlSetting::get(const QString &name)
{
  Q_ASSERT_X(m_map.contains(name), "get", name.toLatin1());

  return m_map[name];
}

QString XmlSetting::operator()(const QString &name)
{
  return get(name);
}


bool XmlSetting::load(const QString fileName)
{
  XMLSettingParser xml;

  if (xml.begin(fileName))
  {
    m_map = xml.map();
  }

  return true;
}


XMLSettingParser::XMLSettingParser()
{

}

bool XMLSettingParser::begin(const QString &fileName)
{
  QFile file(fileName);
  QXmlInputSource   xmlInputStream(&file);
  QXmlSimpleReader  xml;

  xml.setContentHandler(this);
  xml.setErrorHandler(this);

  xml.parse(&xmlInputStream);

  return true;
}

QMap<QString, QString> XMLSettingParser::map()
{
  return m_map;
}

bool XMLSettingParser::error(const QXmlParseException &exception)
{
  return true;
}

bool XMLSettingParser::characters(const QString &str)
{
  return true;
}

bool XMLSettingParser::endElement(const QString &namespaceURI, const QString &localName, const QString &qName)
{
  return true;
}

bool XMLSettingParser::startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &attributes)
{
  if (qName.toLower() == "settings")
  {
    for (int i = 0; i < attributes.count(); i++)
    {
      m_map[attributes.qName(i)] = attributes.value(i);
    }
  }

  return true;
}
