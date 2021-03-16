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
** map018.c: Jaleco SS8806 mapper interface
**
*/

#include <nofrendo.h>
#include <mmc.h>

// Shouldn't that be packed? (It wasn't packed in SNSS...)
typedef struct
{
   unsigned char irqCounterLowByte;
   unsigned char irqCounterHighByte;
   unsigned char irqCounterEnabled;
} mapper18Data;

#define  VRC_PBANK(bank, value, high) \
do { \
   if ((high)) \
      highprgnybbles[(bank)] = (value) & 0x0F; \
   else \
      lowprgnybbles[(bank)] = (value) & 0x0F; \
   mmc_bankrom(8, 0x8000 + ((bank) << 13), (highprgnybbles[(bank)] << 4)+lowprgnybbles[(bank)]); \
} while (0)

#define VRC_VBANK(bank, value, high) \
{ \
   if ((high)) \
      highnybbles[(bank)] = (value) & 0x0F; \
   else \
      lownybbles[(bank)] = (value) & 0x0F; \
   mmc_bankvrom(1, (bank) << 10, (highnybbles[(bank)] << 4)+lownybbles[(bank)]); \
}

static struct
{
   int counter, enabled;
   uint8 nybbles[4];
   int clockticks;
} irq;

static uint8 lownybbles[8];
static uint8 highnybbles[8];
static uint8 lowprgnybbles[3];
static uint8 highprgnybbles[3];


static void map18_init(rom_t *cart)
{
   UNUSED(cart);
   irq.counter = irq.enabled = 0;
}

static void map18_write(uint32 address, uint8 value)
{
   switch (address)
   {
   case 0x8000: VRC_PBANK(0, value, 0); break;
   case 0x8001: VRC_PBANK(0, value, 1); break;
   case 0x8002: VRC_PBANK(1, value, 0); break;
   case 0x8003: VRC_PBANK(1, value, 1); break;
   case 0x9000: VRC_PBANK(2, value, 0); break;
   case 0x9001: VRC_PBANK(2, value, 1); break;
   case 0xA000: VRC_VBANK(0, value, 0); break;
   case 0xA001: VRC_VBANK(0, value, 1); break;
   case 0xA002: VRC_VBANK(1, value, 0); break;
   case 0xA003: VRC_VBANK(1, value, 1); break;
   case 0xB000: VRC_VBANK(2, value, 0); break;
   case 0xB001: VRC_VBANK(2, value, 1); break;
   case 0xB002: VRC_VBANK(3, value, 0); break;
   case 0xB003: VRC_VBANK(3, value, 1); break;
   case 0xC000: VRC_VBANK(4, value, 0); break;
   case 0xC001: VRC_VBANK(4, value, 1); break;
   case 0xC002: VRC_VBANK(5, value, 0); break;
   case 0xC003: VRC_VBANK(5, value, 1); break;
   case 0xD000: VRC_VBANK(6, value, 0); break;
   case 0xD001: VRC_VBANK(6, value, 1); break;
   case 0xD002: VRC_VBANK(7, value, 0); break;
   case 0xD003: VRC_VBANK(7, value, 1); break;
   case 0xE000:
      irq.nybbles[0]=value&0x0F;
      irq.clockticks= (irq.nybbles[0]) | (irq.nybbles[1]<<4) |
                     (irq.nybbles[2]<<8) | (irq.nybbles[3]<<12);
      irq.counter=(uint8)(irq.clockticks/114);
      if(irq.counter>15) irq.counter-=16;
      break;
   case 0xE001:
      irq.nybbles[1]=value&0x0F;
      irq.clockticks= (irq.nybbles[0]) | (irq.nybbles[1]<<4) |
                     (irq.nybbles[2]<<8) | (irq.nybbles[3]<<12);
      irq.counter=(uint8)(irq.clockticks/114);
      if(irq.counter>15) irq.counter-=16;
      break;
   case 0xE002:
      irq.nybbles[2]=value&0x0F;
      irq.clockticks= (irq.nybbles[0]) | (irq.nybbles[1]<<4) |
                     (irq.nybbles[2]<<8) | (irq.nybbles[3]<<12);
      irq.counter=(uint8)(irq.clockticks/114);
      if(irq.counter>15) irq.counter-=16;
      break;
   case 0xE003:
      irq.nybbles[3]=value&0x0F;
      irq.clockticks= (irq.nybbles[0]) | (irq.nybbles[1]<<4) |
                     (irq.nybbles[2]<<8) | (irq.nybbles[3]<<12);
      irq.counter=(uint8)(irq.clockticks/114);
      if(irq.counter>15) irq.counter-=16;
      break;
   case 0xF000:
      if(value&0x01) irq.enabled=true;
      break;
   case 0xF001:
      irq.enabled=value&0x01;
      break;
   case 0xF002:
      switch(value&0x03)
      {
      case 0: ppu_setmirroring(PPU_MIRROR_HORI); break;
      case 1: ppu_setmirroring(PPU_MIRROR_VERT); break;
      case 2: ppu_setmirroring(PPU_MIRROR_SCR1); break;
      case 3: ppu_setmirroring(PPU_MIRROR_SCR1); break; // should this be zero?
      }
      break;
   default:
      break;
   }
}

static void map18_getstate(void *state)
{
   ((mapper18Data*)state)->irqCounterLowByte = irq.counter & 0xFF;
   ((mapper18Data*)state)->irqCounterHighByte = irq.counter >> 8;
   ((mapper18Data*)state)->irqCounterEnabled = irq.enabled;
}

static void map18_setstate(void *state)
{
   irq.counter = (((mapper18Data*)state)->irqCounterHighByte << 8)
                       | ((mapper18Data*)state)->irqCounterLowByte;
   irq.enabled = ((mapper18Data*)state)->irqCounterEnabled;
}

static const mem_write_handler_t map18_memwrite[] =
{
   { 0x8000, 0xFFFF, map18_write },
   LAST_MEMORY_HANDLER
};

mapintf_t map18_intf =
{
   18,               /* mapper number */
   "Jaleco SS8806",  /* mapper name */
   map18_init,       /* init routine */
   NULL,             /* vblank callback */
   NULL,             /* hblank callback */
   map18_getstate,   /* get state (snss) */
   map18_setstate,   /* set state (snss) */
   NULL,             /* memory read structure */
   map18_memwrite,   /* memory write structure */
   NULL              /* external sound device */
};
