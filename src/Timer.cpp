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

#include "Timer.h"

//------------------------------------------------------------------------------

#include <windows.h>

//------------------------------------------------------------------------------

LONGLONG	Timer::frmFrq;

//------------------------------------------------------------------------------

Timer::Timer() {
	LARGE_INTEGER frq;
	QueryPerformanceFrequency(&frq);
	frmFrq = frq.QuadPart;
	start();
}

//------------------------------------------------------------------------------

Timer::~Timer() { }

//------------------------------------------------------------------------------

void Timer::start() {
	LARGE_INTEGER start;
	QueryPerformanceCounter(&start);
	time = start.QuadPart;
}

//------------------------------------------------------------------------------

double Timer::elapsed() {
	LARGE_INTEGER end;
	QueryPerformanceCounter(&end);
	return (double)(end.QuadPart - time) / (double)frmFrq;
}

//------------------------------------------------------------------------------

FPSTimer::FPSTimer() {
	time = 0.0;
	dt = 0.0f;
	adt = 0.0f;
	fps = 0.0f;
	afps = 0.0f;
	frames = 0;
}

//------------------------------------------------------------------------------

float FPSTimer::finish() {

	double elaps = elapsed();
	dt = float(elaps);

	if (elaps > 1e-10) {
		fps	= float(1.0 / elaps);
	} else {
		fps = 0.0f;
	}

	time += elaps;
	frames ++;

	if (time > 1.0) {
		afps = float(double(frames) / time);
		time = 0.0;
		frames = 0;
	}
	adt = adt * 0.5f + dt * 0.5f;
	return fps;
}

//------------------------------------------------------------------------------
