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

#ifndef __NCT6687D_H__
#define __NCT6687D_H__

void nuvoton_init(void);

int nuvoton_temperature_count(void);
const char* nuvoton_temperature_name(unsigned int i);
float nuvoton_temperature_read(unsigned int i);

int nuvoton_voltage_count(void);
const char* nuvoton_voltage_name(unsigned int i);
float nuvoton_voltage_read(unsigned int i);

int nuvoton_rpm_count(void);
const char* nuvoton_rpm_name(unsigned int i);
int nuvoton_rpm_read(unsigned int i);


#endif /* __NCT6687D_H__ */

