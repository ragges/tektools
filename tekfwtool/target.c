typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#define NULL (void *)0
#define ARRAY_SIZE(_x) (sizeof(_x)/sizeof(_x[0]))

uint32_t bss_begin, bss_end;

void (*_console_log)(char *fmt, ...) = (void *)0x16ba;

#define console_log
//_console_log
struct gpib_hdr {
	uint8_t cmd;
	uint8_t csum;
	uint16_t len;
};

struct gpib_flash_program_cmd {
	uint32_t arg0;
	uint32_t arg1;
	uint32_t function;
	uint32_t *base;
	uint32_t data[128];
};

struct gpib_flash_erase_cmd {
	struct gpib_hdr *hdr;
	uint32_t *base;
};

struct flash_descriptor {
	uint8_t manufacturer;
	uint8_t device;
	uint32_t size;
	uint32_t blocksize;
	int (*erase_chip)(uint32_t *base);
	int (*program_single)(uint32_t *base, uint32_t data);
	int (*program_page)(uint32_t *base, uint32_t *data, uint16_t len);
};

static struct flash_descriptor *current_flash;

static void _memset(void *dst, char c, int len)
{
	int unaligned = len % 4;
	char *dst8 = (char *)dst;
	uint32_t *dst32 = (uint32_t *)dst;
	char c32;

	while(unaligned--) {
		*dst8++ = c;
		len--;
	}
	
	c32 = (c << 24) | (c << 16) | (c << 8) | c;
	len /= 4;

	while(len--)
		*dst32++ = c32;
}

static void udelay(int delay)
{
	volatile int i;
        for(i = 0; i < delay; i++);
}

static int flash_wait_sr(uint32_t *base, uint16_t mask, uint16_t result, int tries)
{
	uint32_t buf;
	uint32_t _mask = (mask << 16) | mask;
	uint32_t _result = (result << 16) | result;
	int ret = -1;

	while(tries--) {
		//console_log("flash_wait_sr: %08x: %08x\n", base, *(uint32_t *)base);
		if (*base & _mask == _result)
			break;

		udelay(10000);
	}

	if (!tries) {
		console_log("flash_wait_gsr timeout\n");
		return -1;
	}
	ret = 0;
out:
	return ret;
}

static int flash_wait_gsr(uint32_t *base, uint16_t mask, uint16_t result, int tries)
{
	uint32_t buf;
	uint32_t _mask = (mask << 16) | mask;
	uint32_t _result = (result << 16) | result;
	int ret = -1;

	base = (uint32_t *)(((uint32_t)base) & ~0x1fffff);
	base += 2;

	while(tries--) {
		console_log("flash_wait_gsr: %08x: %08x\n", base, *(uint32_t *)base);
		if ((*base & _mask) == _result)
			break;

		udelay(10);
	}

	if (!tries) {
		console_log("flash_wait_gsr timeout\n");
		return -1;
	}
	ret = 0;
out:
	return ret;
}

static int flash_wait_bsr(uint32_t *base, uint16_t mask, uint16_t result, int tries)
{
	uint32_t buf;
	uint32_t _mask = (mask << 16) | mask;
	uint32_t _result = (result << 16) | result;
	int ret = -1;

	base = (uint32_t *)(((uint32_t)base) & ~0x1ffff);
	base += 1;

	while(tries--) {
		console_log("flash_wait_bsr: %08x: %08x (mask %08x/%08x\n", base, *base, _mask, _result);
		if ((*base & _mask) == _result)
			break;
		udelay(10);
	}

	if (!tries)
		return -1;
	ret = 0;
out:
	return ret;
}


static int flash_erase_intel_s5(uint32_t *base)
{
	*base = 0x30303030;
	*base = 0xd0d0d0d0;

	if (flash_wait_sr(base, 0x0080, 0x0080, 0x100000) == -1)
		return -1;

	*base = 0xffffffff;
	return 0;
}

static int flash_erase_intel_sa(uint32_t *base)
{
	*base = 0xa7a7a7a7;
	*base = 0xd0d0d0d0;

	if (flash_wait_gsr(base, 0x0080, 0x0080, 0x100000) == -1)
		return -1;

	*base = 0xffffffff;
	return 0;
}

static uint8_t csum_hdr(struct gpib_hdr *hdr)
{
	int i, csum;
	uint8_t *buf = (uint8_t *)&hdr;

	csum = buf[0];

	for(i = 2; i < hdr->len + sizeof(struct gpib_hdr) - 2; i++)
			csum += buf[i];
	return csum;
}

static int flash_load_to_pagebuffer_intel_sa(uint32_t *base, uint32_t *data)
{
	uint32_t *buf;
	int i,ret = -1;

	*base = 0x71717171;

	if (flash_wait_gsr(base, 0x0004, 0x0004, 0x100000) == -1)
		goto out;

	*base = 0xe0e0e0e0;

	*base = 0x007f007f;
	*base = 0x00000000;

	buf = (uint32_t *)data;

	for(i = 0; i < 128; i++)
		*base++ = *buf++;
	ret = 0;
out:
	*base = 0xffffffff;
	return ret;
}

static int flash_write_pagebuffer_intel_sa(uint32_t *base)
{
	int ret = -1;
	uint32_t buf;

	*base = 0x71717171;

	if (flash_wait_bsr(base, 0x0008, 0x0000, 0x100000) == -1)
		goto out;

	*base = 0x0c0c0c0c;

	*base = 0x007f007f;
	*base = 0x00000000;

        *base = 0x71717171;

	if (flash_wait_bsr(base, 0x0080, 0x0080, 0x100000) == -1)
		goto out;

	*base = 0xffffffff;
	*base = 0xffffffff;
	ret = 0;
out:
	return ret;

}

static void flash_erase(struct gpib_hdr *hdr)
{
	struct gpib_flash_erase_cmd *cmd = (struct gpib_flash_erase_cmd *)hdr;
	uint32_t *base = (uint32_t *)0x01000000;

	console_log("flash_erase\n");
	_memset(hdr, 0, sizeof(hdr));
	hdr->cmd = '-';

	if (!current_flash)
		goto out;

	if (current_flash->erase_chip(base) == -1) {
		console_log("flash_erase failed\n");
		goto out;
	}

	hdr->cmd = 'P';
out:
	*base = 0xffffffff;
	hdr->csum = csum_hdr(hdr);

}

static int flash_program_single_cmd40(uint32_t *base, uint32_t data)
{
	int i, ret = -1;

	*base = 0x40404040;
	*base = data;

	if (flash_wait_gsr(base, 0x0080, 0x0080, 1000) == -1)
		goto out;

out:
	*base = 0xffffffff;
}

static int flash_program_page_intel_sa(uint32_t *base, uint32_t *data)
{
	if (flash_load_to_pagebuffer_intel_sa(base, data) == -1)
		goto out;

	if (flash_write_pagebuffer_intel_sa(base) == -1)
		goto out;

	*base = 0x50505050;
	*base = 0xffffffff;
	return 0;
out:
	return -1;
}

struct cmd_params {
	struct gpib_hdr hdr;
	struct gpib_flash_program_cmd *cmd;
};

static void flash_program(struct cmd_params *params)
{
	int i, ret = -1;
	uint32_t *base = params->cmd->base;
	uint32_t len = params->hdr.len - 16;
	uint32_t offset = 0;

	//	console_log("flash_program: %d bytes @ %08x\n", len, base);

	_memset(&params->hdr, 0, sizeof(struct gpib_hdr));
	params->hdr.cmd = '-';

	if (!current_flash)
		goto out;
		
	if (len < 0)
		goto out;


	if (current_flash->program_page) {
		while (len >= 512) {
			current_flash->program_page(base + offset, params->cmd->data + offset, 512);
			len -= 512;
			offset += 512;
		}
	}

	len /= 4;
	for(i = 0; i < len; i++) {
		current_flash->program_single(base + offset, params->cmd->data[i]);
		offset += 4;
	}
	
	params->hdr.cmd = 'P';
out:
	*base = 0xffffffff;
	params->hdr.csum = csum_hdr(&params->hdr);
}


struct flash_descriptor flash_types[] = {
	/* Intel TE28F160S5 */
    {	.manufacturer = 0xb0,
	.device = 0xd0,
	.size = 0x200000,
	.blocksize = 0x200,
	.erase_chip = flash_erase_intel_s5,
	.program_single = flash_program_single_cmd40,
    },{
	/* Intel E28F016SA */
	.manufacturer = 0x89,
	.device = 0xa0,
	.size = 0x200000,
	.blocksize = 0x200,
	.erase_chip = flash_erase_intel_sa,
	.program_single = flash_program_single_cmd40,
	.program_page = flash_program_page_intel_sa,
    }
};

static uint32_t identify_flash(uint32_t *base)
{
	uint32_t id;
	*base = 0x90909090;
	id = (*base & 0x00ff00ff) << 8;
	id |= (*(base+1) & 0x00ff00ff);
	*base = 0xffffffff;
	return id;
}

static struct flash_descriptor *find_flash(void)
{
	uint32_t id;
	int i;
	
	id = identify_flash((uint32_t *)0x01000000);
	if (id & 0xffff != (id >> 16) & 0xffff)
		return NULL;

	id &= 0xffff;

	for(i = 0; i < ARRAY_SIZE(flash_types); i++) {
		if ((flash_types[i].device == (id & 0xff)) &&
		   ((flash_types[i].manufacturer == (id >> 8))))
			return flash_types + i;
	}
	console_log("Unknown flash with Vendor ID 0x%02X, Device ID 0x%02X\n", id >> 8, id & 0xff);
	return NULL;
}

static void init(struct gpib_hdr *hdr)
{
	console_log("target init\n");
	_memset(&bss_begin, 0, bss_end - bss_begin);

	current_flash = find_flash();
	_memset(hdr, 0, sizeof(hdr));
	if (current_flash)
		hdr->cmd = 'P';
	else
		hdr->cmd = 'X';
	hdr->csum = csum_hdr(hdr);
}
