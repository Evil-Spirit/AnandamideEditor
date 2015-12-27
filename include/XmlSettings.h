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

#ifndef __XML_SETTINGS_H__
#define __XML_SETTINGS_H__
 
#include <QDomDocument>
#include <QVariant>
 
class CXml_Settings
{
	QDomDocument m_doc;
	QDomElement  m_rootElement;
	QDomElement  m_currentElement;
 
public:
	CXml_Settings();
 
	bool loadFromIO(QIODevice* dev);
	bool saveToIO(QIODevice* dev);
 
	void beginGroup( const QString sName );
	void endGroup();
	QVariant value( const QString & sName, const QVariant & defaultValue = QVariant() );
	void setValue ( const QString & key, const QVariant & value );
 
	inline QDomElement& currentElement() { return m_currentElement; }
	inline QDomElement& rootElement()    { return m_rootElement; }
 
private:
	void createRootElement();
};
 
#endif // __XML_SETTINGS_H__
