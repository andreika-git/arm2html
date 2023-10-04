/*
 * linux/include/asm-arm/arch-atmel/hardware.h
 * for JASPER
 * Created 12/12/2001 Fabrice Gautier
 * Copyright 2001, Sigma Desings, Inc
 */

#ifndef __ASM_ARCH_HARDWARE_H
#define __ASM_ARCH_HARDWARE_H

#define IO_ADDRESS(x) (x)

// PLL input clock, typically 27 Mhz
#define JASPER_EXT_CLOCK 27000000

/* 0=TC0, 1=TC1, 2=TC2 */

//------------------------------------------------------
// SYSTEM CONTROLLER 0x0050_0000
//------------------------------------------------------
#define	    JASPER_SYSCTRL_BASE		0x00500000

#define	    SYSCTRL_CHIP_ID		0x00000000
#define	    SYSCTRL_REVISION_ID		0x00000008
#define	    SYSCTRL_CPUCFG		0x0000000C
#define	    SYSCTRL_TESTSTAT		0x00000010
#define	    SYSCTRL_ERRSTAT		0x00000014
#define	    SYSCTRL_BADADDR		0x00000018
#define	    SYSCTRL_RSTCTL		0x00000020
#define	    SYSCTRL_CPUTIMESLOT		0x00000024

//------------------------------------------------------
// TIMER 0 AND 1 0x0050_0100
//-------------------------TIMER_-----------------------
#define	    JASPER_TIMER_BASE		0x00500100

#define     TIMER_TMRSTAT		0x00000000
#define     TIMER_TMR0LOAD		0x00000010
#define     TIMER_TMR0VAL		0x00000014
#define     TIMER_TMR0CTL		0x00000018
#define     TIMER_TMR1LOAD		0x00000020
#define     TIMER_TMR1VAL		0x00000024
#define     TIMER_TMR1CTL		0x00000028

//------------------------------------------------------
// INT CONTROLLER 0x0050_0200
//------------------------------------------------------
#define     JASPER_INT_CONTROLLER_BASE	0x00500200

#define     INT_IRQSTAT			0x00000000
#define     INT_FIQSTAT			0x00000004
#define     INT_INTTYPE			0x00000010
#define     INT_INTPOLL			0x00000020
#define     INT_INTEN			0x00000024

//------------------------------------------------------
// MAC Registers 0x00500300
//------------------------------------------------------
#define     JASPER_MAC_BASE		0x00500300

#define     MAC_REFTIMER		0x00000000
#define     MAC_FLASH_CFG		0x00000014
#define     MAC_FLASH_ST		0x0000001c
#define     MAC_SDRAMCTL		0x00000020
#define     MAC_SDRAMCFG		0x00000024
#define     MAC_SDRAMDATA		0x00000028
#define     MAC_SDRAMST			0x0000002c
#define     MAC_ARBITERCTRL		0x00000090
#define     MAC_ARBITERSTATE		0x00000094
#define     MAC_WATCHDOG_CTRL		0x000000A0
#define     MAC_WATCHDOG_TMO0		0x000000A4
#define     MAC_WATCHDOG_TMO1		0x000000A8
#define     MAC_WATCHDOG_INT		0x000000Ac
#define     MAC_TIMESLOTCNT		0x000000B0


//------------------------------------------------------
// UART REGISTERs
//	UART0 0050_0500
//	UART1 0050_1300
//------------------------------------------------------
#define		JASPER_UART0_BASE	0x00500500
#define		JASPER_UART1_BASE	0x00501300
#define		UART_NR			2

#define		UART_RBR		0x00
#define		UART_TBR		0x04
#define		UART_IER		0x08
#define		UART_IIR		0x0C
#define		UART_FCR		0x10
#define		UART_LCR		0x14
#define		UART_MCR		0x18
#define		UART_LSR		0x1C
#define		UART_MSR		0x20
#define		UART_SCRATCH		0x24
#define		UART_CLKDIV		0x28
#define		UART_CLKSEL		0x2C

//------------------------------------------------------
// PIO0 block  0x0050_0600
// PIO1 block  0x0050_0A00
//------------------------------------------------------
#define     JASPER_PIO0_BASE		0x00500600
#define     JASPER_PIO1_BASE		0x00500A00

#define     PIO_INT_STATUS		0x00000000
#define     PIO_DATA			0x00000004
#define     PIO_DIR			0x00000008
#define     PIO_POL			0x0000000C
#define     PIO_INT_ENABLE		0x00000010


//------------------------------------------------------
//    I2C MASTER REGISTERs	0X0050_0800
//------------------------------------------------------
#define		JASPER_I2C_MASTER_BASE		0x00500800

#define		I2C_MASTER_CONFIG		0x00
#define		I2C_MASTER_CLK_DIV		0x04
#define		I2C_MASTER_DEV_ADDR		0x08
#define		I2C_MASTER_ADR			0x0C
#define		I2C_MASTER_DATAOUT		0x10
#define		I2C_MASTER_DATAIN		0x14
#define		I2C_MASTER_STATUS		0x18
#define		I2C_MASTER_STARTXFER		0x1C
#define		I2C_MASTER_BYTE_COUNT		0x20
#define		I2C_MASTER_INTEN		0x24
#define		I2C_MASTER_INT			0x28


//------------------------------------------------------
// I2C SLAVE REGISTERs	0X0050_0900
//------------------------------------------------------
#define		JASPER_I2C_SLAVE_BASE			0x00500900

#define		I2C_SLAVE_ADDR				0x00
#define		I2C_SLAVE_DATAOUT			0x04
#define		I2C_SLAVE_DATAIN			0x08
#define		I2C_SLAVE_STATUS			0x0C
#define		I2C_SLAVE_INTEN				0x10
#define		I2C_SLAVE_INT				0x14
#define		I2C_SLAVE_BUS_HOLD			0x18

//------------------------------------------------------
// IDE_REGISTERs 0x0050_0B00
//------------------------------------------------------
#define		JASPER_IDE_BASE				0x00500B00

#define		JASPER_IDE_DMA_BASE			0x00500E00

			// **** DMA CHANNEL REG ****
#define		IDE_BMIC				0x00	// ( 8 BIT) BMIC IDE COMMAND REG
#define		IDE_BMIS				0x04	// ( 8 BIT) BMIC IDE STATUS REG
#define		IDE_BMIDTP				0x08	// (32 BIT) BUSMASTER IDE DESCRIPTOR TABLE POINTER REG
#define		IDE_TIM					0x40	// (16 BIT) IDE TIMING Reg
#define		IDE_SIDETIM				0x48	// ( 8 BIT) SLAVE IDE TIMING Reg
#define		IDE_SRC					0x4C	// (16 BIT) SLEW RATE CTRL Reg (45h-46h)
#define		IDE_STATUS				0x50	// ( 8 BIT) IDESTATUS
#define		IDE_UDMACTL				0x54	// ( 8 BIT) ULTRA DMA CONTORL Reg
#define		IDE_UDMATIM				0x58	// (16 BIT) ULTRA DMA TIMING Reg (4A - 4B)
#define		IDE_PRI_DEVICE_CONTROL			0xE6	// (16 BIT) Device 0:
#define		IDE_PRI_DATA				0xF0	// (16 BIT) Device 0:
#define		IDE_PRI_SECTOR_COUNT			0xF2	// (16 BIT) Device 0:
#define		IDE_PRI_DEVICE_HEAD			0xF6	// (16 BIT) Device 0:
#define		IDE_PRI_CMD				0xF7	// (16 BIT) Device 0:


//------------------------------------------------------
// DVD-LOADER_REGISTERs 0x0050_0C00
//------------------------------------------------------
#define		JASPER_DVD_BASE				0x00500C00
#define		DVD_AV_CTRL				0x00	// (16 BIT) AUDIO/VIDEO PART FROM HOST
#define		DVD_AV_SEC_CNT				0x04	//
#define		DVD_AV_BYTE_CNT				0x08	//
#define		DVD_AV_INTMSK				0x0C	//
#define		DVD_AV_INT				0x10	//
#define		DVD_FIFO_LIM				0x14	//
#define		DVD_TIMOUT_LIM				0x18	//
#define		DVD_AV_X2C				0x1C	//
#define		DVD_HOST_CTRL				0x20	//
#define		DVD_HOST_SCLK				0x24	//
#define		DVD_HOST_TXREG				0x28	//
#define		DVD_HOST_RXREG				0x2C	//

//------------------------------------------------------
// FIP REGISTERs 0x0050_0D00
//------------------------------------------------------

#define		JASPER_FIP_BASE				0x00500D00

#define		FIP_COMMAND				0x00
#define		FIP_DISPLAY_DATA			0x04
#define		FIP_LED_DATA				0x08
#define		FIP_KEY_DATA1				0x0C
#define		FIP_KEY_DATA2				0x10
#define		FIP_SWITCH_DATA				0x14
#define		FIP_CLK_DIV				0x20
#define		FIP_TRISTATE_MODE			0x24

//------------------------------------------------------
//    DVD-DMA REGISTERs  0x0050_0F00
//------------------------------------------------------
#define		JASPER_DVD_DMA_BASE			0x00500F00
#define		DVD_DMACTL				0x00	//
#define		DVD_DMAMSK				0x04	//
#define		DVD_DMAINT				0x08	//
#define		DVD_DMARAW				0x0C	//
#define		DVD_RXADDR				0x10	//
#define		DVD_RXBYTES				0x14	//

//------------------------------------------------------
// RTC REGISTER	0x0050_1400
//------------------------------------------------------
#define		JASPER_RTC_BASE				0x00501400
#define		RTC_CTRL				0x00000000
#define		RTC_LOAD1				0x00000004
#define		RTC_LOAD2				0x00000008
#define		RTC_ALARM				0x0000000C
#define		RTC_INTEN				0x00000010
#define		RTC_INT0				0x00000014
#define		RTC_COUNT1				0x00000018
#define		RTC_COUNT2				0x0000001C

//------------------------------------------------------
// HOST/QUASAR SLAVE REGISTERS 0x0050_1500
//------------------------------------------------------
#define		JASPER_HOST_SLAVE_QUASAR_BASE	0x00501500

#define		HOST_SLAVE_WR_QUASAR_BYTE	0x00000000
#define		HOST_SLAVE_WR_QUASAR_1BYTE	0x00000000
#define		HOST_SLAVE_WR_QUASAR_2BYTE	0x00000004
#define		HOST_SLAVE_WR_QUASAR_3BYTE	0x00000008
#define		HOST_SLAVE_WR_QUASAR_4BYTE	0x0000000C

#define		HOST_SLAVE_RD_QUASAR_BYTE	0x00000000
#define		HOST_SLAVE_RD_QUASAR_1BYTE	0x00000000
#define		HOST_SLAVE_RD_QUASAR_2BYTE	0x00000004
#define		HOST_SLAVE_RD_QUASAR_3BYTE	0x00000008
#define		HOST_SLAVE_RD_QUASAR_4BYTE	0x0000000C

//------------------------------------------------------
// I2S REGISTERs 0x0050_1600
//------------------------------------------------------
#define		JASPER_I2S_BASE			0x00501600

#define		I2S_CTRL			0x0
#define		I2S_PROG_LEN			0x4
#define		I2S_STATUS			0x8
#define		I2S_FRAME_CNTR			0xC

#define		I2S_RESET			0x2
#define		I2S_ENABLE			0x1
#define		I2S_MASTER_MODE			0x4
#define		I2S_SCIN_DIV00			0x00
#define		I2S_SCIN_DIV01			0x08
#define		I2S_SCIN_DIV10			0x10
#define		I2S_SCIN_DIV11			0x18

//------------------------------------------------------
// SPI/I2S_DMA REGISTERs 0x0050_1700
//------------------------------------------------------
#define		JASPER_SPI_I2S_DMA_BASE		0x00501700

#define		SPI_I2S_DMA_CTRL_REG		0x0
#define		SPI_I2S_DMA_BASE_ADD_REG	0x4
#define		SPI_I2S_DMA_SIZE_REG		0x8
#define		SPI_I2S_DMA_WR_PTR_REG		0xC
#define		SPI_I2S_DMA_RD_PTR_REG		0x10
#define		SPI_I2S_DMA_TRSH_REG		0x14
#define		SPI_I2S_DMA_INT_EN_REG		0x18
#define		SPI_I2S_DMA_INT_REG			0x1C
#define		SPI_I2S_DMA_INT_POLL_REG	0x20
#define		SPI_I2S_DMA_INTER_FIFO_REG	0x24
#define		SPI_I2S_DMA_CNT_REG			0x28

//------------------------------------------------------
// SPI REGISTERs 0x0050_1800
//------------------------------------------------------
#define		JASPER_SPI_BASE			0x00501800

#define		SPI_CNTR			0x0
#define		SPI_REC_COUNTER			0x4
#define		SPI_INT_EN			0x8
#define		SPI_INT_STATUS			0xC
#define		SPI_ERROR_CNT			0x10


//------------------------------------------------------
// QUASAR PM/DM AREA
//------------------------------------------------------
#define		JASPER_QUASAR_BASE		0x00600000

#define		QUASAR_MAP_AREA			0x00600000
#define		QUASAR_PM_START			0x00600000
#define		QUASAR_PM_SIZE			0x00002000
#define		QUASAR_DM_START			0x00604000
#define		QUASAR_DM_SIZE		   	0x00002000

//------------------------------------------------------
// QUASAR DRAM CONTROLLER
//------------------------------------------------------

#define		QUASAR_DRAM_CFG			0x00007000
#define		QUASAR_DRAM_FIFOSIZE0		0x00007004
#define		QUASAR_DRAM_FIFOSIZE1		0x00007008
#define		QUASAR_DRAM_CASDELAY		0x0000700C
#define		QUASAR_DRAM_PLLCONTROL		0x00007010
#define		QUASAR_DRAM_TK0			0x00007014
#define		QUASAR_DRAM_TK1			0x00007018
#define		QUASAR_DRAM_TK2			0x0000701C
#define		QUASAR_DRAM_STARTUP0		0x00007020
#define		QUASAR_DRAM_STARTUP1		0x00007024
#define		QUASAR_DRAM_AC3_BASE		0x00007028
#define		QUASAR_DRAM_FIFOSIZE2		0x0000702C
#define		QUASAR_DRAM_PORTMUX		0x00007030

//------------------------------------------------------
// QUASAR <-> HOST INTERFACE ( DMA ENGINES )
//------------------------------------------------------

#define		QUASAR_H2Q_READ_ADDRESS_LO	0x00007F80	//0xFE0
#define		QUASAR_H2Q_READ_ADDRESS_HI	0x00007F84	//0xFE1
#define		QUASAR_H2Q_READ_CONTER		0x00007F88	//0xFE2
#define		QUASAR_H2Q_READ_MASTER_ENABLE	0x00007F8C	//0xFE3
#define		QUASAR_H2Q_INT_MASK		0x00007F90	//0xFE4
#define		QUASAR_H2Q_INT			0x00007F94	//0xFE5
#define		QUASAR_H2Q_INT_STATUS		0x00007F98	//0xFE6

#define		QUASAR_Q2H_WRITE_ADDRESS_LO	0x00007FA0	//0xFE8
#define		QUASAR_Q2H_WRITE_ADDRESS_HI	0x00007FA4	//0xFE9
#define		QUASAR_Q2H_WRITE_CONTER		0x00007FA8	//0xFEA
#define		QUASAR_Q2H_WRITE_MASTER_ENABLE	0x00007FAC	//0xFEB
#define		QUASAR_Q2H_INT_MASK		0x00007FB0	//0xFEC
#define		QUASAR_Q2H_INT			0x00007FB4	//0xFED
#define		QUASAR_Q2H_INT_STATUS		0x00007FB8	//0xFEE

#define		QUASAR_OSD_SOURCE_ADDRESS_LO	0x00007980	//0xE60
#define		QUASAR_OSD_SOURCE_ADDRESS_HI	0x00007984	//0xE61
#define		QUASAR_OSD_SOURCE_COUNTER	0x00007988	//0xE62
#define		QUASAR_OSD_SOURCE_MUX_ENABLE	0x0000798C	//0xE63
#define		QUASAR_OSD_INT_MASK		0x00007990	//0xE64
#define		QUASAR_OSD_INT			0x00007994	//0xE65
#define		QUASAR_OSD_INT_STATUS		0x00007998	//0xE66

//------------------------------------------------------
// QUASAR Local Bus Controller (LBC)
//------------------------------------------------------

#define QUASAR_LBC_CONFIG0			0x00007900 //0x1E40
#define QUASAR_LBC_CONFIG1			0x00007904 //0x1E41
#define QUASAR_LBC_WRITE_FIFO0_ACCESS		0x00007908 //0x1E42
#define QUASAR_LBC_WRITE_FIFO0_CNT		0x0000790C //0x1E43
#define QUASAR_LBC_READ_FIFO0_ACCESS		0x00007910 //0x1E44
#define QUASAR_LBC_READ_FIFO0_CNT		0x00007914 //0x1E45
#define QUASAR_LBC_READ_FIFO1_ACCESS		0x00007918 //0x1E46
#define QUASAR_LBC_READ_FIFO1_CNT		0x0000791C //0x1E47
#define QUASAR_LBC_WRITE_ADDR			0x00007920 //0x1E48
#define QUASAR_LBC_WRITE_DATA			0x00007924 //0x1E49
#define QUASAR_LBC_READ_ADDR		        0x00007928 //0x1E4a
#define QUASAR_LBC_READ_DATA		        0x0000792C //0x1E4b
#define QUASAR_LBC_BURST_XFER_CTRL		0x00007930 //0x1E4c
#define QUASAR_LBC_STATUS			0x00007934 //0x1E4d
#define QUASAR_LBC_INTERRUPT			0x00007938 //0x1E4e
#define QUASAR_LBC_PGIO				0x0000793C //0x1E4f

//------------------------------------------------------
// PIO COMMAND DEFINITIONS
//------------------------------------------------------
// IN/OUTPUT PIO direction
#define		PIO_OUTPUT_BIT0			0x00010001
#define		PIO_OUTPUT_BIT1			0x00020002
#define		PIO_OUTPUT_BIT2			0x00040004
#define		PIO_OUTPUT_BIT3			0x00080008
#define		PIO_OUTPUT_BIT4			0x00100010
#define		PIO_OUTPUT_BIT5			0x00200020
#define		PIO_OUTPUT_BIT6			0x00400040
#define		PIO_OUTPUT_BIT7			0x00800080
#define		PIO_OUTOUT_ALL16BITS		0xFFFFFFFF
// Set DATA to the PIO OUTPUT pin
#define		PIO_DATA_BIT0			0x00010001
#define		PIO_DATA_BIT1			0x00020002
#define		PIO_DATA_BIT2			0x00040004
#define		PIO_DATA_BIT3			0x00080008
#define		PIO_DATA_BIT4			0x00100010
#define		PIO_DATA_BIT5			0x00200020
#define		PIO_DATA_BIT6			0x00400040
#define		PIO_DATA_BIT7			0x00800080

#define		PIO_EN_SET_BIT7			0x00800000
#define		PIO_EN_SET_BIT6			0x00400000
#define		PIO_EN_SET_BIT5			0x00200000
#define		PIO_EN_SET_BIT4			0x00100000
#define		PIO_EN_SET_BIT3			0x00080000
#define		PIO_EN_SET_BIT2			0x00040000
#define		PIO_EN_SET_BIT1			0x00020000
#define		PIO_EN_SET_BIT0			0x00010000
#define		PIO_EN_ALL16_BITS		0xFFFF0000
#define		PIO_EN_BITS15to8		0xFF000000
#define		PIO_EN_BITS7to0			0x00FF0000

//------------------------------------------------------
// INT CTRL COMMAND DEFINITIONS
//------------------------------------------------------
#define     ENABLE_TIMER0_INT			0x00000001
#define     ENABLE_TIMER1_INT			0x00000002
#define     ENABLE_PIO0_INT			0x00000020
#define     ENABLE_PIO1_INT			0x00000040

#define		GLOBAL_INT_ENABLE		0x80000000

#define		MAX_INT				0x00800000
#define		Q2H_LOC_INT			0x00400000
#define		Q2H_RISC_INT			0x00200000
#define		SPI_INT				0x00100000
#define		SPI_I2S_DMA_INT			0x00080000
#define		I2S_INT				0x00040000
#define		RTC_INT				0x00020000
#define		Q2P_INT				0x00010000
#define		P2Q_DMA_INT			0x00008000
#define		OSD_DMA_INT			0x00004000
#define		FIP_INT				0x00002000
#define		IDE_DMA_INT			0x00001000
#define		IDE_INT				0x00000800
#define		DVD_DMA_INT			0x00000400
#define		DVD_INT				0x00000200
#define		I2CS_INT			0x00000100
#define		I2CM_INT			0x00000080
#define		PIO1_INT			0x00000040
#define		PIO0_INT			0x00000020
#define		UART1_INT			0x00000008
#define		UART0_INT			0x00000004
#define		WDTIMER_INT			0x00000002
#define		TIMER1_INT			0x00000002
#define		TIMER0_INT			0x00000001

#define		JASPER_SC_VALID_INT		0x007FFFEF

//------------------------------------------------------
// INT CTRL COMMAND DEFINITIONS II for J_IRQCTRL.c
//------------------------------------------------------
#define ENABLE_INT_GLOBAL		0x80000000		//INT_ENABLE command

#define ENABLE_INT_TIMER0		0x80000001		//INT_ENABLE command
#define ENABLE_INT_WDTIMER		0x80000002		//INT_ENABLE command
#define ENABLE_INT_UART_1		0x80000004		//INT_ENABLE command
#define ENABLE_INT_UART_2		0x80000008		//INT_ENABLE command

#define ENABLE_INT_PIO			0x80000020		//INT_ENABLE command
#define ENABLE_INT_PIO1			0x80000040		//INT_ENABLE command
#define ENABLE_INT_I2C_MASTER		0x80000080		//INT_ENABLE command

#define ENABLE_INT_I2C_SLAVE		0x80000100		//INT_ENABLE command
#define ENABLE_INT_DVD			0x80000200		//INT_ENABLE command
#define ENABLE_INT_DVD_DMA		0x80000400		//INT_ENABLE command
#define ENABLE_INT_IDE			0x80000800		//INT_ENABLE command

#define ENABLE_INT_IDE_DMA		0x80001000		//INT_ENABLE command
#define ENABLE_INT_FIP			0x80002000		//INT_ENABLE command
#define ENABLE_INT_OSD_DMA		0x80004000		//INT_ENABLE command
#define ENABLE_INT_H2Q_DMA		0x80008000		//INT_ENABLE command

#define ENABLE_INT_Q2H_DMA		0x80010000		//INT_ENABLE command
#define ENABLE_INT_RTC			0x80020000		//INT_ENABLE command
#define ENABLE_INT_I2S			0x80040000		//INT_ENABLE command
#define ENABLE_INT_I2S_DMA		0x80080000		//INT_ENABLE command

#define ENABLE_INT_SPI			0x80100000		//INT_ENABLE command
#define ENABLE_INT_Q2H_RISC		0x80200000		//INT_ENABLE command
#define ENABLE_INT_Q2H_LOC		0x80400000		//INT_ENABLE command

#define DISABLE_INT_GLOBAL		0x7FFFFFFF		//INT_ENABLE command
#define DISABLE_INT_UART_1		0xFFFFFFFB		//INT_ENABLE command
#define DISABLE_INT_UART_2		0xFFFFFFF7		//INT_ENABLE command
#define DISABLE_INT_ALL			0x10000000		//INT_ENABLE command

//------------------------------------------------------
//SYSTEM CTRL COMMAND DEFINITIONS
//------------------------------------------------------
#define		SYSCTRL_CMD_RESET_TIMER		0x00000001	// Bit 0
#define		SYSCTRL_CMD_RESET_INTCTRL	0x00000002	// Bit 1
#define		SYSCTRL_CMD_RESET_UART0		0x00000004	// Bit 2
#define		SYSCTRL_CMD_RESET_UART1		0x00000008	// Bit 3
#define		SYSCTRL_CMD_RESET_MAC		0x00000010	// Bit 4
#define		SYSCTRL_CMD_RESET_PIO0		0x00000020	// Bit 5
#define		SYSCTRL_CMD_RESET_PIO1		0x00000040	// Bit 6
#define		SYSCTRL_CMD_RESET_I2CM		0x00000080	// Bit 7

#define		SYSCTRL_CMD_RESET_I2CS		0x00000100	// Bit 8
#define		SYSCTRL_CMD_RESET_DVD		0x00000200	// Bit 9
#define		SYSCTRL_CMD_RESET_DVD_DMA	0x00000400	// Bit10
#define		SYSCTRL_CMD_RESET_IDE		0x00000800	// Bit11
#define		SYSCTRL_CMD_RESET_IDE_DMA	0x00001000	// Bit12
#define		SYSCTRL_CMD_RESET_FIP		0x00002000	// Bit13
#define		SYSCTRL_CMD_RESET_OSD_DMA	0x00004000	// Bit14
#define		SYSCTRL_CMD_RESET_H2Q_DMA	0x00008000	// Bit15

#define		SYSCTRL_CMD_RESET_Q2H_DMA	0x00010000	// Bit16
#define		SYSCTRL_CMD_RESET_RTC		0x00020000	// Bit17
#define		SYSCTRL_CMD_RESET_I2S		0x00040000	// Bit18
#define		SYSCTRL_CMD_RESET_I2S_DMA	0x00080000	// Bit19
#define		SYSCTRL_CMD_RESET_SPI		0x00100000	// Bit20
#define		SYSCTRL_CMD_RESET_SLAVE		0x00200000	// Bit21
#define		SYSCTRL_CMD_RESET_3		0x00400000	// Bit22
#define		SYSCTRL_CMD_RESET_4		0x00800000	// Bit23

#define		SYSCTRL_CMD_RESET_5		0x01000000	// Bit24
#define		SYSCTRL_CMD_RESET_6		0x02000000
#define		SYSCTRL_CMD_RESET_7		0x03000000
#define		SYSCTRL_CMD_RESET_8		0x04000000
#define		SYSCTRL_CMD_RESET_10		0x10000000
#define		SYSCTRL_CMD_RESET_11		0x20000000
#define		SYSCTRL_CMD_RESET_Q4		0x40000000	// Bit30
#define		SYSCTRL_CMD_RESET_ALL		0x80000000	// Bit31

#define		FORCE_REMAP			0x1
#define		CPU_ACCESS_240_CLOCKS		0xF		// 240 closck
#define		CPU_TIMEOUT_15_CLOCK		0xF		// 15 clocks
#define		CPU_TIMESLOT_ENABLE		0x100		// Enable TIMESLOT mechanism


//------------------------------------------------------
// MAC COMMAND DEFINITIONS
//------------------------------------------------------
#define		SDRAMCLK_EN					0x1
#define		SDRAMINI_SET				0x2
//#define		TIME_SLOT_4DW				0x0
//#define		TIME_SLOT_8DW				0x1
//#define		TIME_SLOT_16DW				0x2
//#define		TIME_SLOT_32DW				0x3
#define		TIME_SLOT_HIGH_ARBITRATION	0x10000
#define		TIME_SLOT_SLOW_ARBITRATION	0x000



//------------------------------------------------------
// TIMER COMMAND DEFINITIONS
//------------------------------------------------------
#define	    TIMER02IRQ                 0xFFFFFFFE
#define	    TIMER02FIQ                 0x00000001
#define	    TIMER12IRQ                 0xFFFFFFFD
#define	    TIMER12FIQ                 0x00000002
#define     TIMER0_START               0x00000010
#define     TIMER1_START               0x00000020
#define     TIMER0_INT_CLR             0x00000001
#define     TIMER1_INT_CLR             0x00000002

#define		TIMER_FREE_RUN_MODE	0x00000000
#define		TIMER_PERIODIC_MODE	0x00000010
#define		TIMER_RUN_OUT_MODE	0x00000020
#define		TIMER_COUNT_ENABLE	0x00000080

#define		TIMER_NO_PRESCALE	0x0
#define		TIMER_PRESCALE_4	0x1
#define		TIMER_PRESCALE_8	0x2
#define		TIMER_PRESCALE_16	0x3
#define		TIMER_PRESCALE_32	0x4
#define		TIMER_PRESCALE_64	0x5
#define		TIMER_PRESCALE_128	0x6
#define		TIMER_PRESCALE_256	0x7
#define		TIMER_PRESCALE_512	0x8
#define		TIMER_PRESCALE_1024	0x9
#define		TIMER_PRESCALE_2048	0xA
#define		TIMER_PRESCALE_4096	0xB
#define		TIMER_PRESCALE_8192	0xC
#define		TIMER_PRESCALE_16384	0xD
#define		TIMER_PRESCALE_32768	0xE
#define		TIMER_PRESCALE_65536	0xF


//------------------------------------------------------
// SPI COMMAND DEFINITIONS
//------------------------------------------------------
#define		SPI_ENABLE					0x1
#define		SPI_RESET					0x2
#define		SPI_RESET_ERR_CNTR			0x4
#define		SPI_DISABLE_ERR_CHECK		0x8
#define		SPI_STATUS_MASK				0x000070

#define		SPI_PKT_DONE				0x1
#define		SPI_SIZE_ERR				0x2
#define		SPI_SYNC_ERR				0x4
#define		SPI_HW_ERR					0x8

//------------------------------------------------------
// SPI/I2S-DMA COMMAND DEFINITIONS
//------------------------------------------------------
#define		SPI_I2S_DMA_RESET			0x2
#define		SPI_I2S_DMA_EN				0x1
#define		SPI_I2S_INT_BUF_FULL		0x1
#define		SPI_I2S_INT_BUF_14			0x2
#define		SPI_I2S_INT_BUF_12			0x4
#define		SPI_I2S_INT_BUF_34			0x8
#define		SPI_I2S_CIR_EN				0x8		/* Enable circular buffer */
#define		SPI_I2S_MUX_SPI_SELECT		0x10
#define		SPI_I2S_FLASH_INTER_FIFO	0x4

//------------------------------------------------------
// FIP COMMAND DEFINITIONS    
//------------------------------------------------------
#define		FIP_CMD_DISP_MODE_08DIGITS_20SEGMENTS		0x00
#define		FIP_CMD_DISP_MODE_09DIGITS_19SEGMENTS		0x08
#define		FIP_CMD_DISP_MODE_10DIGITS_18SEGMENTS		0x09
#define		FIP_CMD_DISP_MODE_11DIGITS_17SEGMENTS		0x0a
#define		FIP_CMD_DISP_MODE_12DIGITS_16SEGMENTS		0x0b
#define		FIP_CMD_DISP_MODE_13DIGITS_15SEGMENTS		0x0c
#define		FIP_CMD_DISP_MODE_14DIGITS_14SEGMENTS		0x0d
#define		FIP_CMD_DISP_MODE_15DIGITS_13SEGMENTS		0x0e
#define		FIP_CMD_DISP_MODE_16DIGITS_12SEGMENTS		0x0f


#define		FIP_CMD_DATA_SET_RW_MODE_WRITE_DISPLAY		0x40
#define		FIP_CMD_DATA_SET_RW_MODE_WRITE_LED_PORT		0x41
#define		FIP_CMD_DATA_SET_RW_MODE_READ_KEYS		0x42
#define		FIP_CMD_DATA_SET_RW_MODE_READ_SWITCHES		0x43
#define		FIP_CMD_DATA_SET_ADR_MODE_INCREMENT_ADR		0x40
#define		FIP_CMD_DATA_SET_ADR_MODE_FIXED_ADR		0x44
#define		FIP_CMD_DATA_SET_OP_MODE_NORMAL_OPERATION	0x40
#define		FIP_CMD_DATA_SET_OP_MODE_TEST_MODE		0x48

#define		FIP_CMD_ADR_SETTING				0xC0

#define		FIP_CMD_DISP_CTRL_PULSE_WIDTH_1_16		0x80
#define		FIP_CMD_DISP_CTRL_PULSE_WIDTH_2_16		0x81
#define		FIP_CMD_DISP_CTRL_PULSE_WIDTH_4_16		0x82
#define		FIP_CMD_DISP_CTRL_PULSE_WIDTH_10_16		0x83
#define		FIP_CMD_DISP_CTRL_PULSE_WIDTH_11_16		0x84
#define		FIP_CMD_DISP_CTRL_PULSE_WIDTH_12_16		0x85
#define		FIP_CMD_DISP_CTRL_PULSE_WIDTH_13_16		0x86
#define		FIP_CMD_DISP_CTRL_PULSE_WIDTH_14_16		0x87
#define		FIP_CMD_DISP_CTRL_TURN_DISPLAY_OFF_MASK		0x87
#define		FIP_CMD_DISP_CTRL_TURN_DISPLAY_ON		0x88

//---------------------------------------------------------------------------------------------
// I/O  Macro definitions
//---------------------------------------------------------------------------------------------
#define PRINT_STATUS						*( (volatile unsigned int * )SimStatusAddress)
#define VERILOG_STOP						*( (volatile unsigned int * )SIMULATION_CONTROL) = SIMULATION_CMD_STOP_WITH_ERROR

//---------------------------------------------------------------------------------------------
// QuickTurn Macro definitions
//---------------------------------------------------------------------------------------------
#define		WRITE_LED_DISPLAY				PIO_0_DATA_REG
#define		WRITE_HEX_DISPLAY				PIO_1_DATA_REG

//---------------------------------------------------------------------------------------------
// PIO Macro definitions
//---------------------------------------------------------------------------------------------
#define		PIO_0_INT_STATUS_REG			( (volatile unsigned int * ) (JASPER_PIO0_BASE + PIO_INT_STATUS) )
#define		PIO_0_DATA_REG					( (volatile unsigned int * ) (JASPER_PIO0_BASE + PIO_DATA) )
#define		PIO_0_DIR_REG					( (volatile unsigned int * ) (JASPER_PIO0_BASE + PIO_DIR) )
#define		PIO_0_POL_REG					( (volatile unsigned int * ) (JASPER_PIO0_BASE + PIO_POL) )
#define		PIO_0_INT_ENABLE_REG			( (volatile unsigned int * ) (JASPER_PIO0_BASE + PIO_INT_ENABLE) )

#define		PIO_1_INT_STATUS_REG			( (volatile unsigned int * ) (JASPER_PIO1_BASE + PIO_INT_STATUS) )
#define		PIO_1_DATA_REG					( (volatile unsigned int * ) (JASPER_PIO1_BASE + PIO_DATA) )
#define		PIO_1_DIR_REG					( (volatile unsigned int * ) (JASPER_PIO1_BASE + PIO_DIR) )
#define		PIO_1_POL_REG					( (volatile unsigned int * ) (JASPER_PIO1_BASE + PIO_POL) )
#define		PIO_1_INT_ENABLE_REG			( (volatile unsigned int * ) (JASPER_PIO1_BASE + PIO_INT_ENABLE) )

//---------------------------------------------------------------------------------------------
// SPI_I2S_DMA  Macro definitions
//---------------------------------------------------------------------------------------------
#define		SPI_I2S_DMA_REG_CTRL			( (volatile unsigned int * ) (SPI_I2S_DMA_BASE + SPI_I2S_DMA_CTRL_REG) )
#define		SPI_I2S_DMA_REG_BASE_ADD		( (volatile unsigned int * ) (SPI_I2S_DMA_BASE + SPI_I2S_DMA_BASE_ADD_REG) )
#define		SPI_I2S_DMA_REG_SIZE			( (volatile unsigned int * ) (SPI_I2S_DMA_BASE + SPI_I2S_DMA_SIZE_REG) )
#define		SPI_I2S_DMA_REG_WR_PTR			( (volatile unsigned int * ) (SPI_I2S_DMA_BASE + SPI_I2S_DMA_WR_PTR_REG) )
#define		SPI_I2S_DMA_REG_RD_PTR			( (volatile unsigned int * ) (SPI_I2S_DMA_BASE + SPI_I2S_DMA_RD_PTR_REG) )
#define		SPI_I2S_DMA_REG_TRSH			( (volatile unsigned int * ) (SPI_I2S_DMA_BASE + SPI_I2S_DMA_TRSH_REG) )
#define		SPI_I2S_DMA_REG_INT_EN			( (volatile unsigned int * ) (SPI_I2S_DMA_BASE + SPI_I2S_DMA_INT_EN_REG) )
#define		SPI_I2S_DMA_REG_INT				( (volatile unsigned int * ) (SPI_I2S_DMA_BASE + SPI_I2S_DMA_INT_REG) )
#define		SPI_I2S_DMA_REG_INT_POLL		( (volatile unsigned int * ) (SPI_I2S_DMA_BASE + SPI_I2S_DMA_INT_POLL_REG) )
#define		SPI_I2S_DMA_REG_INTER_FIFO		( (volatile unsigned int * ) (SPI_I2S_DMA_BASE + SPI_I2S_DMA_INTER_FIFO_REG) )
#define		SPI_I2S_DMA_REG_CNT				( (volatile unsigned int * ) (SPI_I2S_DMA_BASE + SPI_I2S_DMA_CNT_REG) )

//---------------------------------------------------------------------------------------------
// Memory Access Controller (MAC) Macro definitions
//---------------------------------------------------------------------------------------------
#define		MAC_REFTIMER_REG				( (volatile unsigned int * ) (MAC_base + MAC_REFTIMER) )
#define		MAC_FLASHCFG_REG				( (volatile unsigned int * ) (MAC_base + MAC_FLASH_CFG) )
#define		MAC_SDRAMCTL_REG				( (volatile unsigned int * ) (MAC_base + MAC_SDRAMCTL) )
#define		MAC_SDRAMCFG_REG				( (volatile unsigned int * ) (MAC_base + MAC_SDRAMCFG) )
#define		MAC_SDRAMDATA_REG				( (volatile unsigned int * ) (MAC_base + MAC_SDRAMDATA) )

//---------------------------------------------------------------------------------------------
// Interrupt Controller  Macro definitions
//---------------------------------------------------------------------------------------------
#define		INT_IRQSTAT_REG				( (volatile unsigned int * ) (JASPER_INT_CONTROLLER_BASE + INT_IRQSTAT) )
#define		INT_FIQSTAT_REG				( (volatile unsigned int * ) (JASPER_INT_CONTROLLER_BASE + INT_FIQSTAT) )
#define		INT_TYPE_REG				( (volatile unsigned int * ) (JASPER_INT_CONTROLLER_BASE + INT_INTTYPE) )
#define		INT_POLL_REG				( (volatile unsigned int * ) (JASPER_INT_CONTROLLER_BASE + INT_INTPOLL) )
#define		INT_ENABLE_REG				( (volatile unsigned int * ) (JASPER_INT_CONTROLLER_BASE + INT_INTEN) )

//---------------------------------------------------------------------------------------------
// SPI Registers  Macro definitions
//---------------------------------------------------------------------------------------------
#define		SPI_CNTR_REG					( (volatile unsigned int * ) (JASPER_SPI_BASE + SPI_CNTR) )
#define		SPI_REC_COUNTER_REG				( (volatile unsigned int * ) (JASPER_SPI_BASE + SPI_REC_COUNTER) )
#define		SPI_INT_EN_REG					( (volatile unsigned int * ) (JASPER_SPI_BASE + SPI_INT_EN) )
#define		SPI_INT_STATUS_REG				( (volatile unsigned int * ) (JASPER_SPI_BASE + SPI_INT_STATUS) )
#define		SPI_ERROR_CNT_REG				( (volatile unsigned int * ) (JASPER_SPI_BASE + SPI_ERROR_CNT) )

//---------------------------------------------------------------------------------------------
// I2S Registers  Macro definitions
//---------------------------------------------------------------------------------------------
#define		I2S_CTRL_REG					( (volatile unsigned int * ) (JASPER_I2S_BASE + I2S_CTRL) )
#define		I2S_PROG_LEN_REG				( (volatile unsigned int * ) (JASPER_I2S_BASE + I2S_PROG_LEN) )
#define		I2S_STATUS_REG					( (volatile unsigned int * ) (JASPER_I2S_BASE + I2S_STATUS) )
#define		I2S_FRAME_CNTR_REG				( (volatile unsigned int * ) (JASPER_I2S_BASE + I2S_FRAME_CNTR) )

//---------------------------------------------------------------------------------------------
// RTC Registers  Macro definitions
//---------------------------------------------------------------------------------------------
#define		RTC_CTRL_REG					( (volatile unsigned int * ) (JASPER_RTC_BASE + RTC_CTRL) )
#define		RTC_LOAD1_REG					( (volatile unsigned int * ) (JASPER_RTC_BASE + RTC_LOAD1) )
#define		RTC_LOAD2_REG					( (volatile unsigned int * ) (JASPER_RTC_BASE + RTC_LOAD2) )
#define		RTC_ALARM_REG					( (volatile unsigned int * ) (JASPER_RTC_BASE + RTC_ALARM) )
#define		RTC_INTEN_REG					( (volatile unsigned int * ) (JASPER_RTC_BASE + RTC_INTEN) )
#define		RTC_INT_REG						( (volatile unsigned int * ) (JASPER_RTC_BASE + RTC_INT0) )
#define		RTC_COUNT1_REG					( (volatile unsigned int * ) (JASPER_RTC_BASE + RTC_COUNT1) )
#define		RTC_COUNT2_REG					( (volatile unsigned int * ) (JASPER_RTC_BASE + RTC_COUNT2) )

//---------------------------------------------------------------------------------------------
// Timer Registers  Macro definitions
//---------------------------------------------------------------------------------------------
#define		TIMER_TMRSTAT_REG				( (volatile unsigned int * ) (JASPER_TIMER_BASE + TIMER_TMRSTAT) )
#define		TIMER0_LOAD_REG					( (volatile unsigned int * ) (JASPER_TIMER_BASE + TIMER_TMR0LOAD) )
#define		TIMER0_VAL_REG					( (volatile unsigned int * ) (JASPER_TIMER_BASE + TIMER_TMR0VAL) )
#define		TIMER0_CNTL_REG					( (volatile unsigned int * ) (JASPER_TIMER_BASE + TIMER_TMR0CTL) )

#define		TIMER1_LOAD_REG					( (volatile unsigned int * ) (JASPER_TIMER_BASE + TIMER_TMR1LOAD) )
#define		TIMER1_VAL_REG					( (volatile unsigned int * ) (JASPER_TIMER_BASE + TIMER_TMR1VAL) )
#define		TIMER1_CNTL_REG					( (volatile unsigned int * ) (JASPER_TIMER_BASE + TIMER_TMR1CTL) )

//---------------------------------------------------------------------------------------------
// Interrupt Registers  Macro definitions
//---------------------------------------------------------------------------------------------
/* XXX - defined twice - see above INT_xxx_REG
#define		INTERRUPT_IRQSTAT_REG			( (volatile unsigned int * ) (INT_CONTROLLER_BASE + INT_IRQSTAT) )
#define		INTERRUPT_FIQSTAT_REG			( (volatile unsigned int * ) (INT_CONTROLLER_BASE + INT_FIQSTAT) )
#define		INTERRUPT_INTTYPE_REG			( (volatile unsigned int * ) (INT_CONTROLLER_BASE + INT_INTTYPE) )
#define		INTERRUPT_POLL_REG				( (volatile unsigned int * ) (INT_CONTROLLER_BASE + INT_INTPOLL) )
#define		INTERRUPT_ENABLE_REG			( (volatile unsigned int * ) (INT_CONTROLLER_BASE + INT_INTEN) )
*/

//---------------------------------------------------------------------------------------------
// System Registers  Macro definitions
//---------------------------------------------------------------------------------------------
#define		SYS_CHIPID_REG				( (volatile unsigned int * ) (JASPER_SYSCTRL_BASE + SYSCTRL_CHIPID) )
#define		SYS_REVID_REG				( (volatile unsigned int * ) (JASPER_SYSCTRL_BASE + SYSCTRL_REVISION_ID) )
#define		SYS_CPU_CFG_REG				( (volatile unsigned int * ) (JASPER_SYSCTRL_BASE + SYSCTRL_CPUCFG) )
#define		SYS_TESTSTAT_REG			( (volatile unsigned int * ) (JASPER_SYSCTRL_BASE + SYSCTRL_TESTSTAT) )
#define		SYS_RESET_REG				( (volatile unsigned int * ) (JASPER_SYSCTRL_BASE + SYSCTRL_RSTCTL) )
#define		SYS_TIMESLOT_REG			( (volatile unsigned int * ) (JASPER_SYSCTRL_BASE + SYSCTRL_CPUTIMESLOT) )

//---------------------------------------------------------------------------------------------
// FIP Registers  Macro definitions
//---------------------------------------------------------------------------------------------
#define		FIP_COMMAND_REG				( (volatile unsigned int * ) (JASPER_FIP_BASE + FIP_COMMAND) )
#define		FIP_DISPLAY_DATA_REG			( (volatile unsigned int * ) (JASPER_FIP_BASE + FIP_DISPLAY_DATA) )
#define		FIP_LED_DATA_REG			( (volatile unsigned int * ) (JASPER_FIP_BASE + FIP_LED_DATA) )
#define		FIP_KEY_DATA1_REG			( (volatile unsigned int * ) (JASPER_FIP_BASE + FIP_KEY_DATA1) )
#define		FIP_KEY_DATA2_REG			( (volatile unsigned int * ) (JASPER_FIP_BASE + FIP_KEY_DATA2) )
#define		FIP_SWITCH_DATA_REG			( (volatile unsigned int * ) (JASPER_FIP_BASE + FIP_SWITCH_DATA) )
#define		FIP_CLK_DIV_REG				( (volatile unsigned int * ) (JASPER_FIP_BASE + FIP_CLK_DIV) )
#define		FIP_TRISTATE_MODE_REG			( (volatile unsigned int * ) (JASPER_FIP_BASE + FIP_TRISTATE_MODE) )

#ifndef __ASSEMBLY__
#include <asm/io.h>

#define HARD_RESET_NOW() outl(JASPER_SYSCTRL_BASE + SYSCTRL_RSTCTL,1)

static inline unsigned long __get_clock(unsigned int unit)
{
	unsigned int clock;
#ifndef CONFIG_QUICKTURN_HACKS
	unsigned int pll_reg;
	unsigned int pll_mult, pll_div, pll_D;
		
	pll_reg = inl(JASPER_QUASAR_BASE + QUASAR_DRAM_PLLCONTROL);
	pll_mult = (pll_reg & 0xFF) >> 2;
	pll_div = (pll_reg >> 8 ) & 0x3;
	pll_D = (pll_reg & 0x2);
		
	clock = ((JASPER_EXT_CLOCK / unit) * (pll_mult+2))/(pll_div + 2);
	if(pll_D)
		clock = clock / 2;
#else
#warning ***** QUICKTURN HACK ***** Kernel think CPU clock is 4 Mhz
	clock = 4000000 / unit;
#endif			
	return clock;
}
#endif

#endif  /* _ASM_ARCH_HARDWARE_H */


