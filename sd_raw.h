
/* This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef SD_RAW_H
#define SD_RAW_H

#include "sd_raw_config.h"


class SdRaw {
public:

	/**
	 * \addtogroup sd_raw
	 *
	 * @{
	 */
	/**
	 * \file
	 * MMC/SD raw access header.
	 *
	 * \author Roland Riegel
	 */

	static SdRaw * GetInstance();

	/**
	 * The card's layout is harddisk-like, which means it contains
	 * a master boot record with a partition table.
	 */
	#define SD_RAW_FORMAT_HARDDISK 0
	/**
	 * The card contains a single filesystem and no partition table.
	 */
	#define SD_RAW_FORMAT_SUPERFLOPPY 1
	/**
	 * The card's layout follows the Universal File Format.
	 */
	#define SD_RAW_FORMAT_UNIVERSAL 2
	/**
	 * The card's layout is unknown.
	 */
	#define SD_RAW_FORMAT_UNKNOWN 3

	/**
	 * This struct is used by sd_raw_get_info() to return
	 * manufacturing and status information of the card.
	 */
	struct sd_raw_info
	{
		/**
		 * A manufacturer code globally assigned by the SD card organization.
		 */
		unsigned char manufacturer;
		/**
		 * A string describing the card's OEM or content, globally assigned by the SD card organization.
		 */
		unsigned char oem[3];
		/**
		 * A product name.
		 */
		unsigned char product[6];
		/**
		 * The card's revision, coded in packed BCD.
		 *
		 * For example, the revision value \c 0x32 means "3.2".
		 */
		unsigned char revision;
		/**
		 * A serial number assigned by the manufacturer.
		 */
		unsigned int serial;
		/**
		 * The year of manufacturing.
		 *
		 * A value of zero means year 2000.
		 */
		unsigned char manufacturing_year;
		/**
		 * The month of manufacturing.
		 */
		unsigned char manufacturing_month;
		/**
		 * The card's total capacity in bytes.
		 */
		unsigned int capacity;
		/**
		 * Defines wether the card's content is original or copied.
		 *
		 * A value of \c 0 means original, \c 1 means copied.
		 */
		unsigned char flag_copy;
		/**
		 * Defines wether the card's content is write-protected.
		 *
		 * \note This is an internal flag and does not represent the
		 *       state of the card's mechanical write-protect switch.
		 */
		unsigned char flag_write_protect;
		/**
		 * Defines wether the card's content is temporarily write-protected.
		 *
		 * \note This is an internal flag and does not represent the
		 *       state of the card's mechanical write-protect switch.
		 */
		unsigned char flag_write_protect_temp;
		/**
		 * The card's data layout.
		 *
		 * See the \c SD_RAW_FORMAT_* constants for details.
		 *
		 * \note This value is not guaranteed to match reality.
		 */
		unsigned char format;
	};

	typedef unsigned char (*sd_raw_interval_handler)(unsigned char* buffer, unsigned int offset, void* p);

	unsigned char sd_raw_init(void);
	unsigned char sd_raw_available(void);
	unsigned char sd_raw_locked(void);

	unsigned char sd_raw_read(unsigned int offset, unsigned char* buffer, unsigned short length);
	unsigned char sd_raw_read_interval(unsigned int offset, unsigned char* buffer, unsigned short interval, unsigned short length, sd_raw_interval_handler callback, void* p);
	unsigned char sd_raw_write(unsigned int offset, const unsigned char* buffer, unsigned short length);
	unsigned char sd_raw_sync(void);

	unsigned char sd_raw_get_info(struct sd_raw_info* info);
	void SDoff(void);

	char format_card(char make_sure);

	/**
	 * @}
	 */

	/**
	 * \addtogroup sd_raw MMC/SD card raw access
	 *
	 * This module implements read and write access to MMC and
	 * SD cards. It serves as a low-level driver for the higher
	 * level modules such as partition and file system access.
	 *
	 * @{
	 */
	/**
	 * \file
	 * MMC/SD raw access implementation.
	 *
	 * \author Roland Riegel
	 */

	/**
	 * \addtogroup sd_raw_config MMC/SD configuration
	 * Preprocessor defines to configure the MMC/SD support.
	 */

	/**
	 * @}
	 */

	/* commands available in SPI mode */

	/* CMD0: response R1 */
	#define CMD_GO_IDLE_STATE 0x00
	/* CMD1: response R1 */
	#define CMD_SEND_OP_COND 0x01
	/* CMD9: response R1 */
	#define CMD_SEND_CSD 0x09
	/* CMD10: response R1 */
	#define CMD_SEND_CID 0x0a
	/* CMD12: response R1 */
	#define CMD_STOP_TRANSMISSION 0x0c
	/* CMD13: response R2 */
	#define CMD_SEND_STATUS 0x0d
	/* CMD16: arg0[31:0]: block length, response R1 */
	#define CMD_SET_BLOCKLEN 0x10
	/* CMD17: arg0[31:0]: data address, response R1 */
	#define CMD_READ_SINGLE_BLOCK 0x11
	/* CMD18: arg0[31:0]: data address, response R1 */
	#define CMD_READ_MULTIPLE_BLOCK 0x12
	/* CMD24: arg0[31:0]: data address, response R1 */
	#define CMD_WRITE_SINGLE_BLOCK 0x18
	/* CMD25: arg0[31:0]: data address, response R1 */
	#define CMD_WRITE_MULTIPLE_BLOCK 0x19
	/* CMD27: response R1 */
	#define CMD_PROGRAM_CSD 0x1b
	/* CMD28: arg0[31:0]: data address, response R1b */
	#define CMD_SET_WRITE_PROT 0x1c
	/* CMD29: arg0[31:0]: data address, response R1b */
	#define CMD_CLR_WRITE_PROT 0x1d
	/* CMD30: arg0[31:0]: write protect data address, response R1 */
	#define CMD_SEND_WRITE_PROT 0x1e
	/* CMD32: arg0[31:0]: data address, response R1 */
	#define CMD_TAG_SECTOR_START 0x20
	/* CMD33: arg0[31:0]: data address, response R1 */
	#define CMD_TAG_SECTOR_END 0x21
	/* CMD34: arg0[31:0]: data address, response R1 */
	#define CMD_UNTAG_SECTOR 0x22
	/* CMD35: arg0[31:0]: data address, response R1 */
	#define CMD_TAG_ERASE_GROUP_START 0x23
	/* CMD36: arg0[31:0]: data address, response R1 */
	#define CMD_TAG_ERASE_GROUP_END 0x24
	/* CMD37: arg0[31:0]: data address, response R1 */
	#define CMD_UNTAG_ERASE_GROUP 0x25
	/* CMD38: arg0[31:0]: stuff bits, response R1b */
	#define CMD_ERASE 0x26
	/* CMD42: arg0[31:0]: stuff bits, response R1b */
	#define CMD_LOCK_UNLOCK 0x2a
	/* CMD58: response R3 */
	#define CMD_READ_OCR 0x3a
	/* CMD59: arg0[31:1]: stuff bits, arg0[0:0]: crc option, response R1 */
	#define CMD_CRC_ON_OFF 0x3b

	/* command responses */
	/* R1: size 1 byte */
	#define R1_IDLE_STATE 0
	#define R1_ERASE_RESET 1
	#define R1_ILL_COMMAND 2
	#define R1_COM_CRC_ERR 3
	#define R1_ERASE_SEQ_ERR 4
	#define R1_ADDR_ERR 5
	#define R1_PARAM_ERR 6
	/* R1b: equals R1, additional busy bytes */
	/* R2: size 2 bytes */
	#define R2_CARD_LOCKED 0
	#define R2_WP_ERASE_SKIP 1
	#define R2_ERR 2
	#define R2_CARD_ERR 3
	#define R2_CARD_ECC_FAIL 4
	#define R2_WP_VIOLATION 5
	#define R2_INVAL_ERASE 6
	#define R2_OUT_OF_RANGE 7
	#define R2_CSD_OVERWRITE 7
	#define R2_IDLE_STATE (R1_IDLE_STATE + 8)
	#define R2_ERASE_RESET (R1_ERASE_RESET + 8)
	#define R2_ILL_COMMAND (R1_ILL_COMMAND + 8)
	#define R2_COM_CRC_ERR (R1_COM_CRC_ERR + 8)
	#define R2_ERASE_SEQ_ERR (R1_ERASE_SEQ_ERR + 8)
	#define R2_ADDR_ERR (R1_ADDR_ERR + 8)
	#define R2_PARAM_ERR (R1_PARAM_ERR + 8)
	/* R3: size 5 bytes */
	#define R3_OCR_MASK (0xffffffffUL)
	#define R3_IDLE_STATE (R1_IDLE_STATE + 32)
	#define R3_ERASE_RESET (R1_ERASE_RESET + 32)
	#define R3_ILL_COMMAND (R1_ILL_COMMAND + 32)
	#define R3_COM_CRC_ERR (R1_COM_CRC_ERR + 32)
	#define R3_ERASE_SEQ_ERR (R1_ERASE_SEQ_ERR + 32)
	#define R3_ADDR_ERR (R1_ADDR_ERR + 32)
	#define R3_PARAM_ERR (R1_PARAM_ERR + 32)
	/* Data Response: size 1 byte */
	#define DR_STATUS_MASK 0x0e
	#define DR_STATUS_ACCEPTED 0x05
	#define DR_STATUS_CRC_ERR 0x0a
	#define DR_STATUS_WRITE_ERR 0x0c

	#if !SD_RAW_SAVE_RAM

	    /* static data buffer for acceleration */
	    static unsigned char raw_block[512];
	    /* offset where the data within raw_block lies on the card */
	    static unsigned int raw_block_address;
	    #if SD_RAW_WRITE_BUFFERING
	    /* flag to remember if raw_block was written to the card */
	    static unsigned char raw_block_written;
	#endif

	#endif

	/* private helper functions */
	static void sd_raw_send_byte(unsigned char b);
	static unsigned char sd_raw_rec_byte(void);
	static unsigned char sd_raw_send_command_r1(unsigned char command, unsigned int arg);
	//static unsigned short sd_raw_send_command_r2(unsigned char command, unsigned int arg);
};

#endif

