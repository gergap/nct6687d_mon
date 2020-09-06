/*****************************************************************************
 * NCT6687D monitoring program.
 * Copyright (C) 2020  Gerhard Gappmeier <gappy1502@gmx.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.EOF
 *****************************************************************************/

#ifndef __SUPERIO_H__
#define __SUPERIO_H__

#include <stdint.h>

#define countof(x) (sizeof(x) / sizeof(*(x)))

#define SIO_INDEX_REG 0x4e /* SIO Index Register. Normally 0x2e or 0x4e */
#define SIO_DATA_REG (SIO_INDEX_REG + 1) /* SIO Data Register */
#define SIO_LDSEL 0x07                   /* Logical device select */
#define SIO_CHIPID 0x20                  /* Chip ID (2 bytes) */
#define SIO_UNLOCK_KEY 0x87              /* key to enable SuperIO*/
#define SIO_LOCK_KEY 0xAA                /* key to disable SuperIO */

// base IO functions
uint8_t sio_read(uint8_t address);
void sio_write(uint8_t address, uint8_t val);
void sio_select(uint8_t dev);
void sio_enter(void);
void sio_exit(void);

// high level functions
uint16_t sio_read_chipid(void);

#endif /* __SUPERIO_H__ */

