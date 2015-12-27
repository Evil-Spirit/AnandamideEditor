//11111111111
//------------------------------------------------------------------------------
//
// This file is part of AnandamideEditor
//
// copyright:	(c) 2010 - 2016
// authors:		Alexey Egorov (FadeToBlack aka EvilSpirit)
// 				Zakhar Shelkovnikov
// 				Georgiy Kostarev
//
// mailto:		anandamide@mail.ru
// 				anandamide.script@gmail.com
//
// AnandamideEditor is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// AnandamideEditor is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with AnandamideEditor. If not, see <http://www.gnu.org/licenses/>.
//
//------------------------------------------------------------------------------

#include "XmlSettings.h"

#include <QDebug>
 
#define ROOT_NODE_NAME "Main"
 
CXml_Settings::CXml_Settings() : m_doc("") {
	createRootElement();
}

bool CXml_Settings::loadFromIO(QIODevice *dev) {
	
	if(!dev) return false;
	if(!dev->isOpen()) return false;
	if(((int)dev->openMode() & QIODevice::ReadOnly) == 0) return false;
	
	m_doc.clear();

	if(!m_doc.setContent(dev)) {
		createRootElement();
		return false;
	} else 
	{
		m_rootElement = m_doc.namedItem(ROOT_NODE_NAME).toElement();

		bool bNormalLoad = !m_rootElement.isNull();
		
		if (!bNormalLoad) {
			qDebug() << "Can't find root XML node with name =" << ROOT_NODE_NAME;
		}

		m_currentElement = m_rootElement;

		return bNormalLoad;
	}
}

bool CXml_Settings::saveToIO(QIODevice *dev) {
	
	if(!dev) return false;
	if(!dev->isOpen()) return false;
	if(((int)dev->openMode() & QIODevice::WriteOnly) == 0) return false;
	
	dev->write(m_doc.toByteArray());
	
	return true;
}
 
void CXml_Settings::beginGroup(const QString sName) {
	
	QDomNode node = m_currentElement.namedItem(sName);
 
	if (!node.isNull()) {
		m_currentElement = node.toElement();
	} else {
		QDomElement deNewElement = m_doc.createElement(sName);
		m_currentElement.appendChild(deNewElement);
		m_currentElement = deNewElement;
	};
}
 
void CXml_Settings::endGroup() {
	if (m_currentElement.nodeName() == ROOT_NODE_NAME) return;
	m_currentElement = m_currentElement.parentNode().toElement();
}
 
QVariant CXml_Settings::value(const QString & key, const QVariant & defaultValue) {
	
	QString keytype = QString("%1_type_str_identifier").arg(key);
	
	if (m_currentElement.hasAttribute(key) && m_currentElement.hasAttribute(keytype)) {
		QString keyid = m_currentElement.attribute(keytype);
		if(keyid == "string")  return m_currentElement.attribute(key);
		if(keyid == "int") return m_currentElement.attribute(key).toInt();
		if(keyid == "uint") return m_currentElement.attribute(key).toUInt();
		if(keyid == "longlong") return m_currentElement.attribute(key).toLongLong();
		if(keyid == "ulonglong") return m_currentElement.attribute(key).toULongLong();
		if(keyid == "double") return m_currentElement.attribute(key).toDouble();
		if(keyid == "bool") return m_currentElement.attribute(key) == "true";
		if(keyid == "bytearray") return QByteArray::fromHex(QByteArray(m_currentElement.attribute(key).toUtf8()));
	}
	
	setValue(key, defaultValue);
	return defaultValue;
}
 
void CXml_Settings::setValue (const QString & key, const QVariant & value) {
	
	QString keytype = QString("%1_type_str_identifier").arg(key);
	switch (value.type()) {
	case QVariant::String:
		m_currentElement.setAttribute(key, value.toString());
		m_currentElement.setAttribute(keytype, "string");
		break;
	case QVariant::Int:
		m_currentElement.setAttribute(key, QString::number(value.toInt()));
		m_currentElement.setAttribute(keytype, "int");
		break;
	case QVariant::UInt:
		m_currentElement.setAttribute(key, QString::number(value.toUInt()));
		m_currentElement.setAttribute(keytype, "uint");
		break;
	case QVariant::LongLong:
		m_currentElement.setAttribute(key, QString::number(value.toLongLong()));
		m_currentElement.setAttribute(keytype, "longlong");
		break;
	case QVariant::ULongLong:
		m_currentElement.setAttribute(key, QString::number(value.toULongLong()));
		m_currentElement.setAttribute(keytype, "ulonglong");
		break;
	case QVariant::Double:
		m_currentElement.setAttribute(key, QString::number(value.toDouble()));
		m_currentElement.setAttribute(keytype, "double");
		break;
	case QVariant::Bool:
		m_currentElement.setAttribute(key, value.toBool() ? "true" : "false");
		m_currentElement.setAttribute(keytype, "bool");
		break;
	case QVariant::ByteArray:
		m_currentElement.setAttribute(key, QString(value.toByteArray().toHex()));
		m_currentElement.setAttribute(keytype, "bytearray");
		break;
	}
}
 
void CXml_Settings::createRootElement() {
	m_rootElement    = m_doc.createElement(ROOT_NODE_NAME);
	m_doc.appendChild(m_rootElement);
	m_currentElement = m_rootElement;
}
