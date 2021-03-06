/*
 * \brief  Serial output driver for core
 * \author Stefan Kalkowski
 * \date   2012-10-24
 */

/*
 * Copyright (C) 2012-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

#ifndef _SERIAL_H_
#define _SERIAL_H_

/* Genode includes */
#include <base/stdint.h>

/**
 * Read byte from I/O port
 */
inline Genode::uint8_t inb(Genode::uint16_t port)
{
	Genode::uint8_t res;
	asm volatile ("inb %%dx, %0" :"=a"(res) :"Nd"(port));
	return res;
}


/**
 * Write byte to I/O port
 */
inline void outb(Genode::uint16_t port, Genode::uint8_t val)
{
	asm volatile ("outb %b0, %w1" : : "a" (val), "Nd" (port));
}


/**
 * Definitions of PC serial ports
 */
enum {
	COMPORT_DATA_OFFSET   = 0,
	COMPORT_STATUS_OFFSET = 5,

	STATUS_THR_EMPTY = 0x20,  /* transmitter hold register empty */
	STATUS_DHR_EMPTY = 0x40,  /* data hold register empty
	                             - data completely sent */
};


/**
 * Initialize serial port
 *
 * Based on 'init_serial' of L4ka::Pistachio's 'kdb/platform/pc99/io.cc'
 */
static void init_comport(Genode::uint16_t port, unsigned baud)
{
	if (!port)
		return;

	unsigned const ier  = port + 1;
	unsigned const eir  = port + 2;
	unsigned const lcr  = port + 3;
	unsigned const mcr  = port + 4;
	unsigned const lsr  = port + 5;
	unsigned const msr  = port + 6;
	unsigned const dllo = port + 0;
	unsigned const dlhi = port + 1;

	outb(lcr, 0x80);  /* select bank 1 */
	outb(dllo, (115200/baud) >> 0);
	outb(dlhi, (115200/baud) >> 8);
	outb(lcr, 0x03);  /* set 8,n,1 */
	outb(ier, 0x00);  /* disable interrupts */
	outb(eir, 0x07);  /* enable FIFOs */
	outb(mcr, 0x0b);  /* force data terminal ready */
	outb(ier, 0x01);  /* enable rx interrupts */
	inb(ier);
	inb(eir);
	inb(lcr);
	inb(mcr);
	inb(lsr);
	inb(msr);
}


/**
 * Output character to serial port
 */
inline void serial_out_char(Genode::uint16_t comport, Genode::uint8_t c)
{
	/* wait until serial port is ready */
	Genode::uint8_t ready = STATUS_THR_EMPTY;
	while ((inb(comport + COMPORT_STATUS_OFFSET) & ready) != ready);

	/* output character */
	outb(comport + COMPORT_DATA_OFFSET, c);
}


namespace Genode {

	/**
	 * Serial output driver for core
	 */
	class Serial
	{
		private:

			uint16_t _comport;

		public:

			void put_char(char c)
			{
				if (!_comport)
					return;

				if (c == '\n')
					serial_out_char(_comport, '\r');
				serial_out_char(_comport, c);
			}

			/**
			 * Constructor
			 *
			 * XXX: The 'baud_rate' argument is ignored for now.
			 */
			Serial(unsigned) : _comport(0x3f8)
			{
				init_comport(_comport, 115200);
			}

			Serial(const char *s) : _comport(0x3f8)
			{
				init_comport(_comport, 115200);

				while (*s)
					put_char(*s++);
			}
	};
}

#endif /* _SERIAL_H_ */
