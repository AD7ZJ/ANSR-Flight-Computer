//Ryans Mods:
//	Added fat16_set_dir(...)-Allows user to set the next entry of the directory


/* This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef FAT16_H
#define FAT16_H

#include "sd_raw_config.h"

/**
 * \addtogroup fat16
 *
 * @{
 */
/**
 * \file
 * FAT16 header.
 *
 * \author Roland Riegel
 */



class FAT16
{
public:
	/**
	 * \addtogroup fat16_file
	 * @{
	 */
	static FAT16 * GetInstance();

	/****************************** SD Card Stuff *****************************/
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

	typedef uint8_t (FAT16::*sd_raw_interval_handler)(uint8_t* buffer, uint32_t offset, void* p);

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
	    unsigned char raw_block[512];
	    /* offset where the data within raw_block lies on the card */
	    unsigned int raw_block_address;
	#if SD_RAW_WRITE_BUFFERING
	    /* flag to remember if raw_block was written to the card */
	    unsigned char raw_block_written;
	#endif

	#endif

	/* private helper functions */
	static void sd_raw_send_byte(unsigned char b);
	static unsigned char sd_raw_rec_byte(void);
	static unsigned char sd_raw_send_command_r1(unsigned char command, unsigned int arg);
	//static unsigned short sd_raw_send_command_r2(unsigned char command, unsigned int arg);


	/****************************** FAT16 Stuff *******************************/
	/** The file is read-only. */
	#define FAT16_ATTRIB_READONLY (1 << 0)
	/** The file is hidden. */
	#define FAT16_ATTRIB_HIDDEN (1 << 1)
	/** The file is a system file. */
	#define FAT16_ATTRIB_SYSTEM (1 << 2)
	/** The file is empty and has the volume label as its name. */
	#define FAT16_ATTRIB_VOLUME (1 << 3)
	/** The file is a directory. */
	#define FAT16_ATTRIB_DIR (1 << 4)
	/** The file has to be archived. */
	#define FAT16_ATTRIB_ARCHIVE (1 << 5)

	/** The given offset is relative to the beginning of the file. */
	#define FAT16_SEEK_SET 0
	/** The given offset is relative to the current read/write position. */
	#define FAT16_SEEK_CUR 1
	/** The given offset is relative to the end of the file. */
	#define FAT16_SEEK_END 2

	/**
	 * @}
	 */

	struct partition_struct;
	struct fat16_fs_struct;
	struct fat16_file_struct;
	struct fat16_dir_struct;

	/**
	 * \ingroup fat16_file
	 * Describes a directory entry.
	 */
	struct fat16_dir_entry_struct
	{
		/** The file's name, truncated to 31 characters. */
		char long_name[32];
		/** The file's attributes. Mask of the FAT16_ATTRIB_* constants. */
		uint8_t attributes;

		/** The time the file was last modified. */
		//uint16_t modified_time;
		/** The date the file was modified. */
		//uint16_t modified_date;

		/** The cluster in which the file's first byte resides. */
		uint16_t cluster;
		/** The file's size. */
		uint32_t file_size;
		/** The total disk offset of this directory entry. */
		uint32_t entry_offset;
	};

	struct fat16_fs_struct*
	fat16_open(struct partition_struct* partition);

	void
	fat16_close(struct fat16_fs_struct* fs);

	struct
	fat16_file_struct* fat16_open_file(struct fat16_fs_struct* fs, const struct fat16_dir_entry_struct* dir_entry);

	void
	fat16_close_file(struct fat16_file_struct* fd);

	int16_t
	fat16_read_file(struct fat16_file_struct* fd, uint8_t* buffer, uint16_t buffer_len);

	int16_t
	fat16_write_file(struct fat16_file_struct* fd, const uint8_t* buffer, uint16_t buffer_len);

	uint8_t
	fat16_seek_file(struct fat16_file_struct* fd, int32_t* offset, uint8_t whence);

	uint8_t
	fat16_resize_file(struct fat16_file_struct* fd, uint32_t size);

	struct fat16_dir_struct*
	fat16_open_dir(struct fat16_fs_struct* fs, const struct fat16_dir_entry_struct* dir_entry);

	void
	fat16_close_dir(struct fat16_dir_struct* dd);

	uint8_t
	fat16_read_dir(struct fat16_dir_struct* dd, struct fat16_dir_entry_struct* dir_entry);

	uint8_t
	fat16_reset_dir(struct fat16_dir_struct* dd);

	uint8_t
	fat16_set_dir(struct fat16_dir_struct* dd, uint16_t offset);

	uint8_t
	fat16_create_file(struct fat16_dir_struct* parent, const char* file, struct fat16_dir_entry_struct* dir_entry);

	uint8_t
	fat16_delete_file(struct fat16_fs_struct* fs, struct fat16_dir_entry_struct* dir_entry);

	uint8_t
	fat16_get_dir_entry_of_path(struct fat16_fs_struct* fs, const char* path, struct fat16_dir_entry_struct* dir_entry);

	uint32_t
	fat16_get_fs_size(const struct fat16_fs_struct* fs);

	uint32_t
	fat16_get_fs_free(const struct fat16_fs_struct* fs);

	uint8_t
	find_file_in_dir(struct fat16_fs_struct* fs, struct fat16_dir_struct* dd, const char* name, struct fat16_dir_entry_struct* dir_entry);

	struct fat16_file_struct*
	open_file_in_dir(struct fat16_fs_struct* fs, struct fat16_dir_struct* dd, const char* name);

	int fat16_file_size(struct fat16_file_struct * file);

	/**
	 * @}
	 */

	/* Each entry within the directory table has a size of 32 bytes
	 * and either contains a 8.3 DOS-style file name or a part of a
	 * long file name, which may consist of several directory table
	 * entries at once.
	 *
	 * multi-byte integer values are stored little-endian!
	 *
	 * 8.3 file name entry:
	 * ====================
	 * offset  length  description
	 *      0       8  name (space padded)
	 *      8       3  extension (space padded)
	 *     11       1  attributes (FAT16_ATTRIB_*)

	 *     0x0E     2  Creation Time
	 *     0x10     2  Creation Date

	 *
	 * long file name (lfn) entry ordering for a single file name:
	 * ===========================================================
	 * LFN entry n
	 *     ...
	 * LFN entry 2
	 * LFN entry 1
	 * 8.3 entry (see above)
	 *
	 * lfn entry:
	 * ==========
	 * offset  length  description
	 *      0       1  ordinal field
	 *      1       2  unicode character 1
	 *      3       3  unicode character 2
	 *      5       3  unicoagainstde character 3
	 *      7       3  unicode character 4
	 *      9       3  unicode character 5
	 *     11       1  attribute (always 0x0f)
	 *     12       1  type (reserved, always 0)
	 *     13       1  checksum
	 *     14       2  unicode character 6
	 *     16       2  unicode character 7
	 *     18       2  unicode character 8
	 *     20       2  unicode character 9
	 *     22       2  unicode character 10
	 *     24       2  unicode character 11
	 *     26       2  cluster (unused, always 0)
	 *     28       2  unicode character 12
	 *     30       2  unicode character 13
	 *
	 * The ordinal field contains a descending number, from n to 1.
	 * For the n'th lfn entry the ordinal field is or'ed with 0x40.
	 * For deleted lfn entries, the ordinal field is set to 0xe5.
	 */

	struct fat16_header_struct
	{
	    uint32_t size;

	    uint32_t fat_offset;
	    uint32_t fat_size;

	    uint16_t sector_size;
	    uint16_t cluster_size;

	    uint32_t root_dir_offset;

	    uint32_t cluster_zero_offset;
	};

	struct fat16_fs_struct
	{
	    struct partition_struct* partition;
	    struct fat16_header_struct header;
	};

	struct fat16_file_struct
	{
	    struct fat16_fs_struct* fs;
	    struct fat16_dir_entry_struct dir_entry;
	    uint32_t pos;
	    uint16_t pos_cluster;
	};

	struct fat16_dir_struct
	{
	    struct fat16_fs_struct* fs;
	    struct fat16_dir_entry_struct dir_entry;
	    uint16_t entry_next;
	};

	struct fat16_read_callback_arg
	{
	    uint16_t entry_cur;
	    uint16_t entry_num;
	    uint32_t entry_offset;
	    uint8_t byte_count;
	};

	struct fat16_usage_count_callback_arg
	{
	    uint16_t cluster_count;
	    uint8_t buffer_size;
	};


	uint8_t fat16_read_header(struct fat16_fs_struct* fs);
	uint8_t fat16_read_root_dir_entry(const struct fat16_fs_struct* fs, uint16_t entry_num, struct fat16_dir_entry_struct* dir_entry);
	uint8_t fat16_read_sub_dir_entry(const struct fat16_fs_struct* fs, uint16_t entry_num, const struct fat16_dir_entry_struct* parent, struct fat16_dir_entry_struct* dir_entry);
	uint8_t fat16_dir_entry_seek_callback(uint8_t* buffer, uint32_t offset, void* p);
	uint8_t fat16_dir_entry_read_callback(uint8_t* buffer, uint32_t offset, void* p);
	uint8_t fat16_interpret_dir_entry(struct fat16_dir_entry_struct* dir_entry, const uint8_t* raw_entry);
	uint16_t fat16_get_next_cluster(const struct fat16_fs_struct* fs, uint16_t cluster_num);
	uint16_t fat16_append_clusters(const struct fat16_fs_struct* fs, uint16_t cluster_num, uint16_t count);
	uint8_t fat16_free_clusters(const struct fat16_fs_struct* fs, uint16_t cluster_num);
	uint8_t fat16_terminate_clusters(const struct fat16_fs_struct* fs, uint16_t cluster_num);
	uint8_t fat16_write_dir_entry(const struct fat16_fs_struct* fs, const struct fat16_dir_entry_struct* dir_entry);
	uint8_t fat16_get_fs_free_callback(uint8_t* buffer, uint32_t offset, void* p);

	/**************************** Rootdir stuff ***************************/
	struct fat16_fs_struct;

	uint8_t print_disk_info(const struct fat16_fs_struct* fs);

	/* returns NULL if error, pointer if file opened */
	struct fat16_file_struct * root_open_new(char* name);

	/* returns 1 if file exists, 0 else */
	int root_file_exists(char* name);

	int openroot(void);

	struct fat16_file_struct * root_open(char* name);

	void root_disk_info(void);
	int rootDirectory_files(char* buf, int len);
	void root_format(void);
	char rootDirectory_files_stream(int reset);
	int root_delete(char* filename);

	struct fat16_dir_entry_struct dir_entry;
	struct fat16_fs_struct* fs;
	struct partition_struct* partition;
	struct fat16_dir_struct* dd;
	struct fat16_file_struct * fd;

	/****************** Partition Stuff *********************/
	/**
	 * The partition table entry is not used.
	 */
	#define PARTITION_TYPE_FREE 0x00
	/**
	 * The partition contains a FAT12 filesystem.
	 */
	#define PARTITION_TYPE_FAT12 0x01
	/**
	 * The partition contains a FAT16 filesystem with 32MB maximum.
	 */
	#define PARTITION_TYPE_FAT16_32MB 0x04
	/**
	 * The partition is an extended partition with its own partition table.
	 */
	#define PARTITION_TYPE_EXTENDED 0x05
	/**
	 * The partition contains a FAT16 filesystem.
	 */
	#define PARTITION_TYPE_FAT16 0x06
	/**
	 * The partition contains a FAT32 filesystem.
	 */
	#define PARTITION_TYPE_FAT32 0x0b
	/**
	 * The partition contains a FAT32 filesystem with LBA.
	 */
	#define PARTITION_TYPE_FAT32_LBA 0x0c
	/**
	 * The partition contains a FAT16 filesystem with LBA.
	 */
	#define PARTITION_TYPE_FAT16_LBA 0x0e
	/**
	 * The partition is an extended partition with LBA.
	 */
	#define PARTITION_TYPE_EXTENDED_LBA 0x0f
	/**
	 * The partition has an unknown type.
	 */
	#define PARTITION_TYPE_UNKNOWN 0xff

	/**
	 * A function pointer used to read from the partition.
	 *
	 * \param[in] offset The offset on the device where to start reading.
	 * \param[out] buffer The buffer into which to place the data.
	 * \param[in] length The count of bytes to read.
	 */
	typedef uint8_t (FAT16::*device_read_t)(uint32_t offset, uint8_t* buffer, uint16_t length);
	/**
	 * A function pointer passed to a \c device_read_interval_t.
	 *
	 * \param[in] buffer The buffer which contains the data just read.
	 * \param[in] offset The offset from which the data in \c buffer was read.
	 * \param[in] p An opaque pointer.
	 * \see device_read_interval_t
	 */
	typedef uint8_t (FAT16::*device_read_callback_t)(uint8_t* buffer, uint32_t offset, void* p);
	/**
	 * A function pointer used to continuously read units of \c interval bytes
	 * and call a callback function.
	 *
	 * This function starts reading at the specified offset. Every \c interval bytes,
	 * it calls the callback function with the associated data buffer.
	 *
	 * By returning zero, the callback may stop reading.
	 *
	 * \param[in] offset Offset from which to start reading.
	 * \param[in] buffer Pointer to a buffer which is at least interval bytes in size.
	 * \param[in] interval Number of bytes to read before calling the callback function.
	 * \param[in] length Number of bytes to read altogether.
	 * \param[in] callback The function to call every interval bytes.
	 * \param[in] p An opaque pointer directly passed to the callback function.
	 * \returns 0 on failure, 1 on success
	 * \see device_read_t
	 */
	typedef uint8_t (FAT16::*device_read_interval_t)(uint32_t offset, uint8_t* buffer, uint16_t interval, uint16_t length, device_read_callback_t callback, void* p);
	/**
	 * A function pointer used to write from the partition.
	 *
	 * \param[in] offset The offset on the device where to start writing.
	 * \param[in] buffer The buffer which to write.
	 * \param[in] length The count of bytes to write.
	 */
	typedef uint8_t (FAT16::*device_write_t)(uint32_t offset, const uint8_t* buffer, uint16_t length);

	/**
	 * Describes a partition.
	 */
	struct partition_struct
	{
		/**
		 * The function which reads data from the partition.
		 *
		 * \note The offset given to this function is relative to the whole disk,
		 *       not to the start of the partition.
		 */
		device_read_t device_read;
		/**
		 * The function which repeatedly reads a constant amount of data from the partition.
		 *
		 * \note The offset given to this function is relative to the whole disk,
		 *       not to the start of the partition.
		 */
		device_read_interval_t device_read_interval;
		/**
		 * The function which writes data to the partition.
		 *
		 * \note The offset given to this function is relative to the whole disk,
		 *       not to the start of the partition.
		 */
		device_write_t device_write;

		/**
		 * The type of the partition.
		 *
		 * Compare this value to the PARTITION_TYPE_* constants.
		 */
		uint8_t type;
		/**
		 * The offset in bytes on the disk where this partition starts.
		 */
		uint32_t offset;
		/**
		 * The length in bytes of this partition.
		 */
		uint32_t length;
	};

	struct partition_struct* partition_open(device_read_t device_read, device_read_interval_t device_read_interval, device_write_t device_write, int8_t index);
	uint8_t partition_close(struct partition_struct* partition);

	/**
	 * @}
	 */
};

#endif

