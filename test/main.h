/* Copyright (C) 2004-2010 Daniel Verite

   This file is part of Manitou-Mail (see http://www.manitou-mail.org)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License version 2 as
   published by the Free Software Foundation.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#ifndef INC_MAIN_H
#define INC_MAIN_H

#define TODO_WEBKIT

#ifdef _MSC_VER
#pragma warning (disable:4786)
#define strcasecmp stricmp
#define strncasecmp strnicmp
#endif

#include "config.h"
#include "dbtypes.h"

#ifndef CHECK_PTR
#define CHECK_PTR(p) do { if (!p) abort(); } while(0)
#endif


#define DBG_PRINTF(l,...) debug_printf(l,__FILE__,__LINE__, __VA_ARGS__)
#define ERR_PRINTF(...) err_printf(__FILE__,__LINE__, __VA_ARGS__)


void debug_printf(int level, const char* file, int line, const char *fmt, ...);
void err_printf(const char* file, int line, const char *fmt, ...);



#endif // INC_MAIN_H
