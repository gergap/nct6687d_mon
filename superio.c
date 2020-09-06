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

#include "superio.h"
#include <sys/io.h>

/* Notes about SuperIO.
 * All registers are access through two ports: SIO_INDEX_REG and SIO_DATA_REG
 * SIO_INDEX_REG sets the register name to access, SIO_DATA_REG is used for
 * reading and writing the values if the selected register.
 * Register 0x00-0x2f are global registers.
 * 0x30-0xff are devices specific, so the correct logical device must be selected first.
 * Using the register 0x07 the logical device can be selected.
 */

/** Reads a SIO register at the given address.
 * @param address register address
 * @return read value
 */
uint8_t sio_read(uint8_t address)
{
    outb(address, SIO_INDEX_REG);
    return inb(SIO_DATA_REG);
}

/** Writes a byte to a SIO register.
 * @param address register address
 * @param val the byte value to write
 */
void sio_write(uint8_t address, uint8_t val)
{
    outb(address, SIO_INDEX_REG);
    outb(val, SIO_DATA_REG);
}

/** Selects the given logical device.
 * This makes the registers 0x30-0xff available of the device.
 */
void sio_select(uint8_t dev)
{
    sio_write(SIO_LDSEL, dev);
}

/** Unlocks the registers so that we can read and write it. */
void sio_enter(void)
{
    /* nuvotun enter extended function mode to be able to change values.
     * therefor we need to write 87h twice to SIO_INDEX_REG
     */
    outb(SIO_UNLOCK_KEY, SIO_INDEX_REG);
    outb(SIO_UNLOCK_KEY, SIO_INDEX_REG);
}

/** Locks the registers to protect them from changes. */
void sio_exit(void)
{
    outb(SIO_LOCK_KEY, SIO_INDEX_REG);
}

/**
 * Reads the SIO chip id.
 */
uint16_t sio_read_chipid(void)
{
    uint16_t id;

    sio_enter();
    id = sio_read(SIO_CHIPID);
    id <<= 8;
    id |= sio_read(SIO_CHIPID + 1);
    sio_exit();

    return id;
}
