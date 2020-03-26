/*
** Nofrendo (c) 1998-2000 Matthew Conte (matt@conte.com)
**
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of version 2 of the GNU Library General
** Public License as published by the Free Software Foundation.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.  To obtain a
** copy of the GNU Library General Public License, write to the Free
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
**
** Any permitted reproduction of these routines, in whole or in part,
** must bear this legend.
**
**
** nofrendo.h (c) 1998-2000 Matthew Conte (matt@conte.com)
**            (c) 2000 Neil Stevens (multivac@fcmail.com)
**
** Note: all architectures should call these functions
**
** $Id: nofrendo.h,v 1.2 2001/04/27 11:10:08 neil Exp $
*/

#ifndef _NOFRENDO_H_
#define _NOFRENDO_H_

#define  APP_STRING     "Nofrendo"
#define  APP_VERSION    "2.0"

#include <nes.h>

extern nes_t *console;
extern bitmap_t *primary_buffer;

extern int nofrendo_start(const char *filename, int region);
extern void nofrendo_stop(void);
extern void nofrendo_refresh(void);
extern void nofrendo_notify(char *format, ...);

#endif /* !_NOFRENDO_H_ */
