/*    Copyright (C) 2014 GP Orcullo
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#ifndef PICNC_GRBL_H
#define PICNC_GRBL_H

#define SPICLKRATE		0x1004		/* 10.2 Mhz */
#define NUMAXES			3		/* X Y Z */

#define SPIBUFSIZE		20		/* SPI buffer size */
#define BUFSIZE			(SPIBUFSIZE/4)

#define STEPBIT			23		/* bit location in DDS accum */
#define STEP_MASK		(1<<STEPBIT)

#define BASEFREQ		80000ul		/* Base freq of the PIC stepgen in Hz */
#define SYS_FREQ		(48000000ul)    /* 48 MHz */

#define PERIODFP 		((double)1.0 / (double)(BASEFREQ))
#define VELSCALE		((double)STEP_MASK * PERIODFP)
#define ACCELSCALE		(VELSCALE * PERIODFP)

#define get_position(a)		(rxBuf[1 + (a)])
#define update_velocity(a, b)	(txBuf[1 + (a)] = (b))

/* Sunxi defines */

#define SUNXI_PIO_BASE		(0x01C20000)
#define SUNXI_SPI2_BASE		(0x01C17000)

#define OFS			(gpio + 0x224)	/* port PE offset */

#define SUNXI_PE_CFG0		*(OFS)		/* configure register 0 */
#define SUNXI_PE_CFG1		*(OFS + 0x01)	/* configure register 1 */ 
#define SUNXI_PE_CFG2		*(OFS + 0x02)	/* configure register 2 */ 
#define SUNXI_PE_CFG3		*(OFS + 0x03)	/* configure register 3 */ 
#define SUNXI_PE_DAT		*(OFS + 0x04)	/* data register */ 
#define SUNXI_PE_PUL0		*(OFS + 0x07)	/* pull register 0 */ 
#define SUNXI_PE_PUL1		*(OFS + 0x08)	/* pull register 1 */

#define SUNXI_CCM_SPI2_CLK_CFG	*(gpio + 0x2A)	/* CCM register */
#define SUNXI_CCMU_AHB_GATE0	*(gpio + 0x18)

#define SPI2_RXDATA		*((unsigned char *)spi) /* rx data register */
#define SPI2_TXDATA		*((unsigned char *)(spi + 0x01)) /* tx data register */
#define SPI2_CTL		*(spi + 0x02)	/* control register */
#define SPI2_INT_CTL		*(spi + 0x03)	/* interrupt control register */
#define SPI2_STATUS		*(spi + 0x04)	/* status register */
#define SPI2_DMA_CTL		*(spi + 0x05)	/* dma control register */
#define SPI2_WAIT		*(spi + 0x06)	/* wait clock counter register */
#define SPI2_CLK_RATE		*(spi + 0x07)	/* clock rate control register */
#define SPI2_BC			*(spi + 0x08)	/* burst counter register   */
#define SPI2_TC			*(spi + 0x09)	/* transmit counter register */
#define SPI2_FIFO_STA		*(spi + 0x0A)	/* fifo status register */

/* lifted from arch/arm/mach-sun5i/include/mach/spi.h */
#define SPI_CTL_EN		(0x1 << 0)	/* SPI module enable control */
#define SPI_CTL_FUNC_MODE	(0x1 << 1)	/* SPI function mode select */
#define SPI_CTL_PHA		(0x1 << 2)	/* SPI Clock polarity control */
#define SPI_CTL_POL		(0x1 << 3)	/* SPI Clock/Data phase control */
#define SPI_POL_PHA_BIT_POS	(2)
#define SPI_CTL_SSPOL		(0x1 << 4)	/* SPI Chip select signal polarity control */
#define SPI_CTL_DMAMOD		(0x1 << 5)	/* SPI dma mode select */
#define SPI_CTL_LMTF		(0x1 << 6)	/* LSB/MSB transfer first select */
#define SPI_CTL_SSCTL		(0x1 << 7)	/* SPI chip select control */
#define SPI_CTL_RST_TXFIFO	(0x1 << 8)	/* SPI reset rxFIFO */
#define SPI_CTL_RST_RXFIFO	(0x1 << 9)	/* SPI reset txFIFO */
#define SPI_CTL_XCH		(0x1 << 10)	/* Exchange burst */
#define SPI_CTL_RAPIDS		(0x1 << 11)	/* Rapids transfer mode */
#define SPI_CTL_SS_MASK		(0x3 << 12)	/* SPI chip select */
#define SPI_SS_BIT_POS		(12)
#define SPI_CTL_DDB		(0x1 << 14)	/* Dummy burst Type */
#define SPI_CTL_DHB		(0x1 << 15)	/* Discard Hash Burst */
#define SPI_CTL_SS_CTRL		(0x1 << 16)	/* SS output mode select */
#define SPI_CTL_SS_LEVEL	(0x1 << 17)
#define SPI_CTL_T_PAUSE_EN	(0x1 << 18)	/* Transmit Pause Enable */
#define SPI_CTL_MASTER_SDC	(0x1 << 19)	/* master sample data control */

#define PAGE_SIZE		(4*1024)
#define BLOCK_SIZE		(4*1024)

#endif
