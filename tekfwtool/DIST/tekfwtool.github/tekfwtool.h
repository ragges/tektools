#ifndef __TEKFW_TOOL_H
#define __TEKFW_TOOL_H

#include <stdint.h>

#define TARGET_FIRMWARE_BASE 0x05010000

#ifndef BYTE_ORDER
#error Unknown endianness !
#endif

#if BYTE_ORDER == LITTLE_ENDIAN
#define cpu_to_be16(_x) ((((uint16_t) (_x) & 0xff) << 8) | (((_x) >> 8) & 0xff))
#define be16_to_cpu cpu_to_be16
#define cpu_to_be32(_x) (cpu_to_be16(((_x) >> 16) & 0xffff) | \
			 (cpu_to_be16(((uint32_t) (_x) & 0xffff)) << 16))
#define be32_to_cpu cpu_to_be32
#else
#define cpu_to_be16
#define cpu_to_be32
#define be16_to_cpu
#define be32_to_cpu
#endif

#define MIN(_a, _b) ((_a) > (_b) ? (_b) : (_a))

struct cmd_hdr {
	uint8_t cmd;
	uint8_t csum;
	uint16_t len;
};

struct memory_read_cmd {
	struct cmd_hdr hdr;
	uint32_t addr;
	uint32_t len;
};

struct memory_write_cmd {
	struct cmd_hdr hdr;
	uint32_t addr;
	uint32_t len;
	uint8_t buf[1024];
};

struct branch_cmd {
	struct cmd_hdr hdr;
	uint32_t argc;
  	uint32_t unknown;
	uint32_t function;
	uint32_t arg0;
	uint8_t buffer[512];
};

#define TARGET_FW_INIT 0
#define TARGET_FW_FLASH_ERASE 1
#define TARGET_FW_FLASH_PROGRAM 2

#endif
