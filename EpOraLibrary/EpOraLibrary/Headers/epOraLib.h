/*! 
@file epOraLib.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/eporalibrary>
@date May 24, 2012
@brief EP Oracle Library Interface
@version 1.0

@section LICENSE

Copyright (C) 2012  Woong Gyu La <juhgiyo@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

@section DESCRIPTION

An Interface for EP Oracle Library.
*/

#ifndef __EP_ORACLE_LIB_H__
#define __EP_ORACLE_LIB_H__

#ifdef EP_ORACLELIB_DLL_EXPORT

#define EP_ORACLELIB __declspec(dllexport)
#ifndef EP_LIBRARY_DLL_IMPORT
#define EP_LIBRARY_DLL_IMPORT
#endif //EP_LIBRARY_DLL_IMPORT

#else   //EP_ORACLELIB_DLL_EXPORT

#ifdef EP_ORACLE_DLL_IMPORT

#define EP_ORACLELIB __declspec(dllimport)
#ifndef EP_LIBRARY_DLL_IMPORT
#define EP_LIBRARY_DLL_IMPORT
#endif //EP_LIBRARY_DLL_IMPORT

#else   //EP_ORACLELIB_DLL_IMPORT

#define EP_ORACLELIB

#endif  //EP_ORACLELIB_DLL_IMPORT
#endif  //EP_ORACLELIB_DLL_EXPORT

#include "epl.h"
using namespace epl;

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // _CRTDBG_MAP_ALLOC
#endif //defined(_DEBUG)

#endif //__EP_ORACLE_LIB_H__