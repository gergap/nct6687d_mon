# NCT6687D Monitor

Test program to read out system information from Nuvoton NCT6687D.

My new motherboard MSI Tomahawk B550 contains this chip, which monitors
temperatures, voltages and controls the fans. It is currently not supported
by the Linux hwmon drivers.

This test code serves as a playground before integrating it into the
driver drivers/hwmon/nct6683.c of the Linux kernel.

It also serves as an example for other people who want to test new SuperIO chips.
The main advantage is that you can test you HW without the hassle of creating
a kernel module, which adds another layer of complexity.

## Security

Because this userspace program needs IO privileges you need to run it as root.
It's just a test program, don't install it on production systems.

## File Overview

* superio.c: contains generic SuperIO code
* nct6687d.c: contains the main NCT6687D specific code
* main.c: a simple test program for displaying the values

## Building

The usual CMake steps...

    mkdir bld
    cd bld
    cmake ..
    make

## Usage

Simply call it to read out once:

    sudo ./bld/nct6687d_mon

Use the option `-n <sec>` to continue monitoring:

    sudo ./bld/nct6687d_mon -n 1

## Screenshots

![Screenshot](nct6687dmon.png?raw=true)
