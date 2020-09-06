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

#include "nct6687d.h"
#include "superio.h"
#include <stdint.h>
#include <stdio.h>
#include <sys/io.h>

#define NUVOTON_VENDOR_ID 0x5CA3
// ID: 0xd5, Revision: 0x92
#define NCTT6687D_DEVICE_ID 0xd592

// Logical device number for Hardware Monitor
#define WINBOND_NUVOTON_HARDWARE_MONITOR_LDN 0x0B

// Registers to access hwmon data
#define REG_BASE_ADDRESS 0x60
#define REG_PAGE_OFFSET 0x04
#define REG_INDEX_OFFSET 0x05
#define REG_DATA_OFFSET 0x06
#define REG_PAGE_SELECT 0xFF

/* I find it confusing that outb has the wrong argument order ... */
#define PortRead(address) inb(address)
#define PortWrite(address, value) outb(value, address)

static uint16_t g_base_address;

static const char *temp_names[] = {
    "CPU",
    "System",
    "MOS",
    "PCH",
    "CPU Socket",
    "PCIE_1",
    "M2_1",
};

static const char *volt_names[] = {
    "VIN0 +12V",
    "VIN1 +5V",
    "VIN2 VCore",
    "VIN3 SIO",
    "VIN4 DRAM",
    "VIN5 CPU IO",
    "VIN6 CPU SA",
    "VIN7 SIO",
    "3VCC I/O +3.3",
    "SIO VTT",
    "SIO VREF",
    "SIO VSB",
    "SIO AVSB",
    "SIO VBAT",
    "CPU",
    "System",
    "MOS",
    "PCH",
    "CPU Socket",
    "PCIE_1",
    "M2_1",
};

static const char *fan_names[] = {
    "CPU Fan",
    "PUMP Fan",
    "SYS Fan 1",
    "SYS Fan 2",
    "SYS Fan 3",
    "SYS Fan 4",
    "SYS Fan 5",
    "SYS Fan 6",
};

static unsigned short _temperatureRegister[] = { 0x100, 0x102, 0x104, 0x106, 0x108, 0x10A, 0x10C };
static unsigned short _voltageRegisters[] = { 0x120, 0x122, 0x124, 0x126, 0x128, 0x12A, 0x12C, 0x12E, 0x130, 0x13A, 0x13E, 0x136, 0x138, 0x13C };
static unsigned short _fanRpmRegister[] = { 0x140, 0x142, 0x144, 0x146, 0x148, 0x14A, 0x14C, 0x14E };

static uint8_t ReadByte(unsigned short address)
{
    uint8_t page = (uint8_t)(address >> 8);
    uint8_t index = (uint8_t)(address & 0xFF);

    PortWrite(g_base_address + REG_PAGE_OFFSET, REG_PAGE_SELECT);
    PortWrite(g_base_address + REG_PAGE_OFFSET, page);
    PortWrite(g_base_address + REG_INDEX_OFFSET, index);
    return PortRead(g_base_address + REG_DATA_OFFSET);
}

static void WriteByte(unsigned short address, uint8_t value)
{
    uint8_t page = (uint8_t)(address >> 8);
    uint8_t index = (uint8_t)(address & 0xFF);

    PortWrite(g_base_address + REG_PAGE_OFFSET, REG_PAGE_SELECT);
    PortWrite(g_base_address + REG_PAGE_OFFSET, page);
    PortWrite(g_base_address + REG_INDEX_OFFSET, index);
    PortWrite(g_base_address + REG_DATA_OFFSET, value);
}

void nuvoton_init(void)
{
    unsigned short initRegister = 0x180;
    uint8_t data;

    sio_enter();

    // select hardware monitor logical device
    sio_select(WINBOND_NUVOTON_HARDWARE_MONITOR_LDN);

    // get base address for reading hwmon values
    g_base_address = sio_read(REG_BASE_ADDRESS);
    g_base_address <<= 8;
    g_base_address |= sio_read(REG_BASE_ADDRESS + 1);
    printf("HWMon base address: 0x%04x\n", g_base_address);

    data = ReadByte(initRegister);
    printf("init byte = 0x%02x\n", data);
    if ((data & 0x80) == 0) {
        printf("enable init bit\n");
        WriteByte(initRegister, (uint8_t)(data | 0x80));
    }
#if 0
    // enable SIO voltage
    WriteByte(0x1BB, 0x61);
    WriteByte(0x1BC, 0x62);
    WriteByte(0x1BD, 0x63);
    WriteByte(0x1BE, 0x64);
    WriteByte(0x1BF, 0x65);
#endif
    sio_exit();
}

int nuvoton_temperature_count(void)
{
    return countof(_temperatureRegister);
}

const char* nuvoton_temperature_name(unsigned int i)
{
    if (i < countof(temp_names)) return temp_names[i];
    return NULL;
}

float nuvoton_temperature_read(unsigned int i)
{
    int value;
    int half;
    float temp;

    if (i >= countof(_temperatureRegister)) return 0.0f;

    sio_enter();
    value = (int)ReadByte(_temperatureRegister[i]);
    half = (ReadByte((_temperatureRegister[i] + 1)) >> 7) & 0x1;
    sio_exit();

    temp = value + (0.5f * half);
    return temp;
}

int nuvoton_voltage_count(void)
{
    return countof(_voltageRegisters);
}

const char* nuvoton_voltage_name(unsigned int i)
{
    if (i < countof(volt_names)) return volt_names[i];
    return NULL;
}

float nuvoton_voltage_read(unsigned int i)
{
    uint8_t high, low;
    float value;

    if (i >= countof(_voltageRegisters)) return 0.0f;

    sio_enter();
    high = ReadByte(_voltageRegisters[i]);
    low  = ReadByte(_voltageRegisters[i] + 1);
    sio_exit();

    value = 0.001f * (16 * high + (low >> 4));

    switch (i) {
    case 0:
        value *= 12.0f; // 12V
        break;
    case 1:
        value *= 5.0f; // 5V
        break;
    case 4:
        value *= 2.0f; // DRAM
        break;
    default:
        break;
    }

    return value;
}

int nuvoton_rpm_count(void)
{
    return countof(_fanRpmRegister);
}

const char* nuvoton_rpm_name(unsigned int i)
{
    if (i < countof(fan_names)) return fan_names[i];
    return NULL;
}

int nuvoton_rpm_read(unsigned int i)
{
    uint8_t high, low;
    int value;

    if (i >= countof(fan_names)) return -1;

    sio_enter();
    high = ReadByte(_fanRpmRegister[i]);
    low = ReadByte(_fanRpmRegister[i] + 1);
    sio_exit();

    value = ((int)high << 8) | (int)low;

    return value;
}

