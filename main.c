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

#include <stdio.h>
#include <stdlib.h>
#include <sys/io.h>
#include <getopt.h>
#include <unistd.h>
#include <math.h>
#include "superio.h"
#include "nct6687d.h"

int main(int argc, char *argv[])
{
    uint16_t id;
    int i;
    const char *name;
    float temp, volt;
    int rpm;
    float temp_cache[10] = { 0.0f };
    float volt_cache[10] = { 0.0f };
    int rpm_cache[10] = { 0 };

    int opt;
    int nsecs = 0;
    while ((opt = getopt(argc, argv, "n:")) != -1) {
        switch (opt) {
        case 'n':
            nsecs = atoi(optarg);
            break;
        default: /* '?' */
            fprintf(stderr, "Usage: %s [-n nsecs] name\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (iopl(3) < 0) {
        perror("Failed to get I/O privileges (are you root?)");
        return 2;
    }
    printf("Successfully gained I/O privileges.\n");

    id = sio_read_chipid();
    printf("chipid: 0x%04x\n", id);

    nuvoton_init();

    do {
        printf("\033[2J\033[1;1H");
        for (i = 0; i < nuvoton_temperature_count(); ++i) {
            name = nuvoton_temperature_name(i);
            temp = nuvoton_temperature_read(i);

            if (fabs(temp_cache[i] - temp) > 0.1) {
                temp_cache[i] = temp;
                printf("%-12s: \033[31m%.2f\033[0m C\n", name, temp);
            } else {
                printf("%-12s: %.2f C\n", name, temp);
            }
        }

        for (i = 0; i < nuvoton_voltage_count(); ++i) {
            name = nuvoton_voltage_name(i);
            volt = nuvoton_voltage_read(i);

            if (fabs(volt_cache[i] - volt) > 0.1) {
                volt_cache[i] = volt;
                printf("%-12s: \033[31m%.2f\033[0m V\n", name, volt);
            } else {
                printf("%-12s: %.2f V\n", name, volt);
            }
        }

        for (i = 0; i < nuvoton_rpm_count(); ++i) {
            name = nuvoton_rpm_name(i);
            rpm  = nuvoton_rpm_read(i);

            if (rpm_cache[i] != rpm) {
                rpm_cache[i] = rpm;
                printf("%-12s: \033[31m%i\033[0m rpm\n", name, rpm);
            } else {
                printf("%-12s: %i rpm\n", name, rpm);
            }
        }
        usleep(nsecs * 1000000);
    } while (nsecs > 0);

    return 0;
}


