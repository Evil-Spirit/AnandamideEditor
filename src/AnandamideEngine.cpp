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

#include "AnandamideEngine.h"

#include <QCoreApplication>

namespace Anandamide {
	
	/**
	 * @brief The AnandamideEngine class
	 */
	AnandamideEngine::AnandamideEngine(QObject *parent) :
		QObject(parent)
	{

	}
	
	bool AnandamideEngine::run(Anandamide::Script *script)
	{
		if(script == NULL) return false;
		if(!script->compile()) return false;
		
		emit started();
		QCoreApplication::processEvents();
		script->run();
		
		return true;
	}

}
