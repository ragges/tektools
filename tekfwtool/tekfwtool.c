
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <errno.h>

#if defined(__MSDOS__)
	#include <malloc.h> /* can this be removed? */
	#include "dosdefs.h"
	#include "tfdos.h"	//simply tekfwtool.h renamed to 8.3-safe name
	#include "tgtdummy.h"
#else
	#include "target-procs.h"
	#include "tekfwtool.h"
#endif

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	#include <malloc.h> /* can this be removed? */
	#include <windows.h>
	#include "ni488.h"
#elif defined(__linux__)
	/* linux with linux-gpib */
	#include <gpib/ib.h>
#elif defined(__APPLE__)
	/* MacOS with NI GPIB drivers */
	#include <ni4882.h>
#else
        #error "Unknown compiler environment!"
#endif


#define DEFAULT_GPIBADDR 29


#if !defined (PROG_NAME)
  #define PROG_NAME __FILE__
#endif
#if !defined (GIT_VERSION)
  #define GIT_VERSION "unknown"
#endif
#if !defined (BUILD_TIME)
  #define BUILD_TIME __DATE__ " " __TIME__
#endif



int  Dev;
const char *ErrorMnemonic[] = {"EDVR", "ECIC", "ENOL", "EADR", "EARG",
			       "ESAC", "EABO", "ENEB", "EDMA", "",
			       "EOIP", "ECAP", "EFSO", "", "EBUS",
			       "ESTB", "ESRQ", "", "", "", "ETAB"};

static int abort_requested = 0;
static int debug;

#define UNUSED(x) (void)(x)

static void sigint_handler(int arg)
{
	UNUSED(arg);
	abort_requested = 1;
}

static void GPIBCleanup(int Dev, char* ErrorMsg)
{
	printf("Error : %s\nibsta = 0x%x iberr = %d (%s)\n",
	       ErrorMsg, ibsta, iberr, ErrorMnemonic[iberr]);
	if (Dev != -1) {
		printf("Cleanup: Taking device offline\n");
		ibonl (Dev, 0);
	}
}

#if 0
/* currently not used */
static int write_command(char *cmd)
{
	ibwrt (Dev, cmd, strlen(cmd));
	if (ibsta & ERR)
		return -1;
	return 0;
}
#endif

#if 0
/* currently not used */
static int query(char *query, char *buf, int maxsize)
{

	ibwrt (Dev, query, strlen(query));
	if (ibsta & ERR)
		return -1;

	ibrd (Dev, buf, maxsize);
	if (ibsta & ERR)
		return -1;

	buf[ibcntl - 1] = '\0';
	return ibcntl;
}
#endif

static void hexdump(void *_buf, int len)
{
	int i;
	uint8_t *buf = (uint8_t *)_buf;
	for(i = 0; i < len; i++)
		fprintf(stderr, "%02X ", buf[i]);
	fprintf(stderr, "\n");
}

static void build_csum(struct cmd_hdr *hdr)
{
	uint8_t csum = 0;
	uint32_t i;
	for(i = 0; i < be16_to_cpu(hdr->len) + sizeof(struct cmd_hdr); i++)
			csum += ((uint8_t *)hdr)[i];
	hdr->csum = csum;
}

static int write_memory(uint32_t addr, uint8_t *buf, int len)
{
	struct memory_write_cmd cmd;
	struct cmd_hdr hdr;
	char c;

	memset(&cmd, 0, sizeof(cmd));
	cmd.hdr.cmd = 'M';
	cmd.hdr.len = cpu_to_be16(len + 8);
	cmd.addr = cpu_to_be32(addr);
	cmd.len = cpu_to_be32(len);

	memcpy(cmd.buf, buf, len);

	build_csum((struct cmd_hdr *)&cmd);
	if (debug > 1)
		hexdump(&cmd, len + 12);

	ibwrt (Dev, &cmd, len + 12);
	if (ibsta & ERR) {
		fprintf(stderr, "%s: writing command failed\n", __func__);
		return -1;
	}

	ibrd(Dev, &c, 1);
	if (ibcntl != 1 || c != '+') {
		fprintf(stderr, "%s: response reading failed\n", __func__);
		return -1;
	}

	ibrd(Dev, &hdr, sizeof(struct cmd_hdr));
	if (ibsta & ERR) {
		fprintf(stderr, "%s: response reading failed\n", __func__);
		return -1;
	}

	if (ibcntl < (signed)sizeof(hdr)) {
		fprintf(stderr, "%s: short header\n", __func__);
		return -1;
	}

	if (hdr.cmd != '=') {
		fprintf(stderr, "%s: invalid response: %c\n", __func__, hdr.cmd);
		return -1;
	}
	c = '+';
	ibwrt(Dev, &c, 1);
	return 0;
}

static int branch_cmd(uint32_t addr, uint32_t arg0, uint8_t *data, int *datalen)
{
	struct branch_cmd cmd;
	uint8_t buf[1024];
	struct cmd_hdr hdr;
	char c;

	memset(&cmd, 0, sizeof(cmd));
	cmd.hdr.cmd = 'B';
	cmd.hdr.len = cpu_to_be16(16 + *datalen);
	cmd.function = cpu_to_be32(addr);
//	cmd.argc = cpu_to_be32(2);
//	cmd.unknown = cpu_to_be32(2);
	cmd.arg0 = cpu_to_be32(arg0);
	memcpy(&cmd.buffer, data, *datalen);

	build_csum((struct cmd_hdr *)&cmd);
	if (debug > 1)
		hexdump(&cmd, be16_to_cpu(cmd.hdr.len) + sizeof(struct cmd_hdr));

	ibwrt (Dev, &cmd, 20 + *datalen);
	if (ibsta & ERR) {
		fprintf(stderr, "%s: writing command failed\n", __func__);
		return -1;
	}

	ibrd(Dev, &c, 1);
	if (ibcntl != 1 || c != '+') {
	  fprintf(stderr, "%s: response reading failed: ibcntl: %u, %02x\n", __func__, (unsigned int) ibcntl, c);
		return -1;
	}

	ibrd(Dev, &hdr, sizeof(struct cmd_hdr));
	if (ibsta & ERR) {
		fprintf(stderr, "%s: response reading failed\n", __func__);
		return -1;
	}

	if (ibcntl < (signed)sizeof(hdr)) {
		fprintf(stderr, "%s: short header\n", __func__);
		return -1;
	}

	if (debug > 1)
		hexdump(&hdr, 4);

	if (hdr.len) {
		printf("reading %u bytes\n", be16_to_cpu(hdr.len));
		ibrd(Dev, buf, be16_to_cpu(hdr.len));
		if (ibsta & ERR) {
			fprintf(stderr, "%s: reading of additional data failed\n", __func__);
			return -1;
		}
	}

	if (hdr.len)
		hexdump(buf, be16_to_cpu(hdr.len));

	if (hdr.cmd != 'P') {
		fprintf(stderr, "%s: invalid response: %c\n", __func__, hdr.cmd);
		return -1;
	}
	c = '+';
	ibwrt(Dev, &c, 1);
	return 0;
}

static int read_memory(uint32_t addr, uint8_t *buf, int len)
{
	struct memory_read_cmd cmd;
	struct cmd_hdr hdr;
	int responselen;
	char c;

	memset(&cmd, 0, sizeof(cmd));
	cmd.hdr.cmd = 'm';
	cmd.hdr.len = cpu_to_be16(sizeof(cmd) - 4);
	cmd.addr = cpu_to_be32(addr);
	cmd.len = cpu_to_be32(len);

	build_csum((struct cmd_hdr *)&cmd);
	if (debug > 1)
		hexdump(&cmd, sizeof(cmd));

	ibwrt (Dev, &cmd, sizeof(struct memory_read_cmd));
	if (ibsta & ERR) {
		fprintf(stderr, "%s: writing command failed\n", __func__);
		return -1;
	}

	ibrd(Dev, &c, 1);
	if (ibcntl != 1 || c != '+') {
		fprintf(stderr, "%s: response reading failed\n", __func__);
		return -1;
	}
	ibrd(Dev, &hdr, sizeof(struct cmd_hdr));
	if (ibsta & ERR) {
		fprintf(stderr, "%s: response reading failed\n", __func__);
		return -1;
	}

	if (debug > 1) {
		hexdump(&hdr, sizeof(hdr));
	}

	if (ibcntl < (signed)sizeof(hdr)) {
	  fprintf(stderr, "%s: short header (ibcntl=%u)\n", __func__, (unsigned int) ibcntl);
		return -1;
	}

	if (hdr.cmd != '=') {
		fprintf(stderr, "%s: invalid response: %c\n", __func__, hdr.cmd);
		return -1;
	}

	responselen = be16_to_cpu(hdr.len);

	if (responselen != len) {
		fprintf(stderr, "%s: short response (%d < %u)\n",
				__func__, responselen, len);
		return -1;
	}
	ibrd(Dev, buf, responselen);
	if (ibsta & ERR || ibcntl < len) {
		fprintf(stderr, "%s: response reading failed\n", __func__);
		return -1;
	}

	c = '+';
	ibwrt(Dev, &c, 1);
	if (ibsta & ERR) {
		fprintf(stderr, "%s: unable to send ACK\n", __func__);
		return -1;
	}

	return 0;
}

static struct option long_options[] = {
	{ "addr", required_argument, 0, 'a' },
	{ "read", required_argument, 0, 'r' },
	{ "write", required_argument, 0, 'w' },
	{ "base", required_argument, 0, 'b' },
	{ "length", required_argument, 0, 'l' },
	{ "debug", no_argument, 0, 'd' },
	{ "flash-id", no_argument, 0, 'i' },
	{ "flash-erase", no_argument, 0, 'e' },
	{ "flash-program", required_argument, 0, 'p' },
	{ "help", no_argument, 0, 'h' },
	{ NULL, 0, 0, 0 }
};

static char* ident = PROG_NAME "   Version: " GIT_VERSION "   Build time: " BUILD_TIME;
static void print_version(void)
{
	fprintf(stderr, "# %s\n", ident);
}

static void usage(void)
{
	print_version(); /* XXX should have it's own flag */
	fprintf(stderr, "\nusage:\n"
		"--read          -r <filename>  read from memory to file\n"
		"--write         -w <filename>  read from file to memory\n"
		"--base          -b <base>      base address for read/write/program\n"
		"--length        -l <length>    length of data to be read or written\n"
		"--addr          -a <addr>      device's GPIB address (optional). Default 29\n"
		"--debug         -d             enable debug logging\n"
		"--flash-id      -i             print ID of flash chips\n"
		"--flash-erase   -e             erase flash at base address\n"
		"--flash-program -p             program flash at base address\n"
		"");
}

static uint32_t to_number(char *s)
{
	uint32_t val;
	char *endp;

	if (*s == '0' && *(s+1) == 'x') {
		val = strtoul(s+2, &endp, 16);
		if (*endp != '\0') {
			fprintf(stderr, "failed to parse: [%s]\n", s);
			return 0;
		}
	} else {
		val = strtoul(s, &endp, 10);
		if (*endp != '\0') {
			fprintf(stderr, "failed to parse: [%s]\n", s);
			return 0;
		}
	}
	return val;
}

static int flash_program(uint32_t base, uint8_t *buf, int size)
{
	int len = size;
	return branch_cmd(TARGET_flash_program, base, buf, &len);
}

static int flash_erase(uint32_t base)
{
	int len = 0;
	printf("Erasing flash @ 0x%08lx\n", (unsigned long) base);
	return branch_cmd(TARGET_flash_erase, base, NULL, &len);
}

static int init_firmware(void)
{
	int len = 0;
	return branch_cmd(TARGET_init, 0, NULL, &len);
}

static int download_firmware(void)
{
	FILE *file = fopen("target.bin", "rb");
	char buf[512];
	size_t len, offset = 0;
	int ret = -1;

	if (!file) {
		fprintf(stderr, "failed to open target.bin: %s", strerror(errno));
		return -1;
	}

	printf("Downloading firmware to scope\n");
	while((len = fread(buf, 1, sizeof(buf), file)) > 0) {
		if (write_memory(TARGET_FIRMWARE_BASE + offset, (uint8_t *)&buf, len) == -1) {
			fprintf(stderr, "error downloading firmware\n");
			goto out;
		}
		offset += len;
	}

	if (ferror(file)) {
		fprintf(stderr, "error reading firmware from file\n");
		goto out;
	}

	printf("Pinging firmware\n");
	if (init_firmware() == -1) {
		fprintf(stderr, "failed to ping firmware\n");
		goto out;
	}
	printf("Firmware downloaded\n");
	ret = 0;
out:
	fclose(file);
	return ret;
}

int main(int argc, char **argv)
{
	uint32_t len, addr, base = 0, length = 0;
	int devaddr = DEFAULT_GPIBADDR;
	char c;
	uint8_t buf[1024];
	int optidx;
	FILE *file = NULL;
	int read_op = 0, write_op = 0, erase_flash_op = 0, flash_write_op = 0;
	int readlen;
	time_t start, now;

	while((c = getopt_long(argc, argv, "a:r:w:b:l:p:hied",
			       long_options, &optidx)) != -1) {
		switch(c) {
		case 'h':
			usage();
			return 0;
		case 'a':
			devaddr = (int) to_number(optarg);
			if ((devaddr < 0) || (devaddr > 30)) {
				printf("invalid GPIB address\n");
				return 1;
			}
			break;
		case 'l':
			if (length) {
				fprintf(stderr, "length given twice");
				return 1;
			}
			length = to_number(optarg);
			break;
		case 'b':
			if (base) {
				fprintf(stderr, "base given twice");
				return 1;
			}
			base = to_number(optarg);
			break;
		case 'r':
			if (file) {
				fprintf(stderr, "read given twice");
				return 1;
			}
			file = fopen(optarg, "wb");
			if (!file) {
				fprintf(stderr, "failed to open output file: %s\n", strerror(errno));
				return 1;

			}
			read_op = 1;
			break;
		case 'w':
			if (file) {
				fprintf(stderr, "read given twice");
				return 1;
			}
			file = fopen(optarg, "rb");
			if (!file) {
				fprintf(stderr, "failed to open input file: %s\n", strerror(errno));
				return 1;

			}

			write_op = 1;
			break;
		case 'p':
			if (file) {
				fprintf(stderr, "read given twice");
				return 1;
			}
			file = fopen(optarg, "rb");
			if (!file) {
				fprintf(stderr, "failed to open input file: %s\n", strerror(errno));
				return 1;

			}
			flash_write_op = 1;
			break;

		case 'e':
			erase_flash_op = 1;
			break;
		case 'd':
			debug++;
			break;
		default:
			usage();
			goto bad_exit;
		}
	}
	if (optind <= 1) {
		usage();
		return 1;
	}

	if (!read_op && !write_op && !erase_flash_op && !flash_write_op) {
		printf("No operation specified !\n");
		usage();
		goto bad_exit;
	}

	if (!length) {
		fprintf(stderr, "%s: length required\n", __func__);
		goto bad_exit;
	}

	if ((erase_flash_op || flash_write_op) && (TARGET_init == 0)) {
		printf("Cannot flash: compiled without flash write support.\n");
		goto bad_exit;
	}

	signal(SIGINT, sigint_handler);

	Dev = ibdev(0, devaddr, 0, T100s, 1, 0);
	if (ibsta & ERR) {
		printf("Unable to open device\nibsta = 0x%x iberr = %d\n",
		       ibsta, iberr);
		goto bad_exit;
	}

	ibclr (Dev);
	if (ibsta & ERR) {
		GPIBCleanup(Dev, "Unable to clear device");
		goto bad_exit;
	}

	if (erase_flash_op || flash_write_op) {
		if (download_firmware() == -1)
			goto bad_exit;
	}
	if (erase_flash_op) {
		flash_erase(base);
		return 0;
	}

	time(&start);
	for(addr = base; addr < base + length && !abort_requested;) {
		len = MIN(512, base + length - addr);
		if (read_op) {
			if (read_memory(addr, buf, len) == -1)
				goto bad_exit;

			if (fwrite(buf, 1, len, file) != len) {
				fprintf(stderr, "short fwrite\n");
				goto bad_exit;
			}
			if ((addr % 0x1000) == 0) {
				time(&now);
				fprintf(stderr, "READ %08lx/%08lx, %3u%% %4ds\r",
							(unsigned long) (addr - base), (unsigned long) length,
							(unsigned) ((addr - base) * 100 / length), (int)(now - start));
			}

			addr += len;
		} else if (write_op) {
			readlen = fread(buf, 1, len, file);
			if (readlen == 0)
				break;

			if (readlen == -1) {
				fprintf(stderr, "fread: %s\n", strerror(errno));
				goto bad_exit;
			}
			if (write_memory(addr, buf, readlen) == -1){
				goto bad_exit;
			}
			if ((addr % 0x1000) == 0) {
				time(&now);
				fprintf(stderr, "WRITE %08lx/%08lx, %3u%% %4ds\r",
							(unsigned long) (addr - base), (unsigned long) length,
							(unsigned) ((addr - base) * 100 / length), (int)(now - start));
			}
			addr += readlen;
		} else if (flash_write_op) {
			len = MIN(512, base + length - addr);
			readlen = fread(buf, 1, len, file);
			if (readlen == 0)
				break;

			if (readlen == -1) {
				fprintf(stderr, "fread: %s\n", strerror(errno));
				goto bad_exit;
			}

			if (flash_program(addr, buf, readlen) == -1) {
				fprintf(stderr, "flash programming failed\n");
				goto bad_exit;
			}
			addr += readlen;
			if ((addr % 0x1000) == 0) {
				time(&now);
				fprintf(stderr, "FLASH WRITE %08lx/%08lx, %3u%% %4ds\r",
							(unsigned long) (addr - base), (unsigned long) length,
							(unsigned) ((addr - base) * 100 / length), (int)(now - start));
			}
		} else {
			fprintf(stderr, "either read or write required\n");
			goto bad_exit;
		}
	}
	fprintf(stderr, "\n");
	fclose(file);
	ibonl(Dev, 0);
	return 0;

bad_exit:
	if (file) {
		fclose(file);
	}
	return 1;

}
