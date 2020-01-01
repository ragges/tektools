#ifndef __TARGET_PROCS_H
#define __TARGET_PROCS_H

#define TARGET__memset 0x05010000
#define TARGET_csum_hdr 0x05010340
#define TARGET_find_flash 0x0501077c
#define TARGET_flash_erase 0x050104c8
#define TARGET_flash_erase_intel_s5 0x050102a4
#define TARGET_flash_erase_intel_sa 0x050102f2
#define TARGET_flash_load_to_pagebuffer_intel_sa 0x05010396
#define TARGET_flash_program 0x050105da
#define TARGET_flash_program_page_intel_sa 0x0501058e
#define TARGET_flash_program_single_cmd40 0x05010544
#define TARGET_flash_wait_bsr 0x05010200
#define TARGET_flash_wait_gsr 0x0501015c
#define TARGET_flash_wait_sr 0x050100ba
#define TARGET_flash_write_pagebuffer_intel_sa 0x0501042c
#define TARGET_identify_flash 0x05010732
#define TARGET_init 0x05010864
#define TARGET_udelay 0x05010092
#endif