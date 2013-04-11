/* unofficial gameplaySP kai
 *
 * Copyright (C) 2006 Exophase <exophase@gmail.com>
 * Copyright (C) 2007 takka <takka@tfact.net>
 * Copyright (C) 2007 ????? <?????>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef MEMORY_H
#define MEMORY_H

#define MEM_STATE_NUM (10)

#define PSP2K_MEM_TOP (0x0a000000)

#define SAVESTATE_SIZE 506952
#define SAVESTATE_SIZE_OLD 506947
#define SVS_HEADER_SIZE 11
extern const u8 SVS_HEADER[SVS_HEADER_SIZE];
#define SVS_FILE_SIZE (SAVESTATE_SIZE+SVS_HEADER_SIZE)



// グローバル変数宣言

extern u32 mem_save_flag;
extern char gamepak_title[13];
extern char gamepak_code[5];
extern char gamepak_maker[3];
extern char gamepak_filename[MAX_FILE];
extern char gamepak_filename_full_path[MAX_PATH];
extern u32 gamepak_crc32;

extern u8 *gamepak_rom;
extern u8 *gamepak_rom_resume;
extern u32 gamepak_ram_buffer_size;
extern u32 oam_update;
extern u32 gbc_sound_update;

extern u8 savestate_write_buffer[];
extern u8 *g_state_buffer_ptr;

//#define USE_VRAM

#ifndef USE_VRAM
extern u16 palette_ram[512];
extern u16 oam_ram[512];
extern u16 io_registers[1024 * 16];
extern u8 ewram[1024 * 256 * 2];
extern u8 iwram[1024 * 32 * 2];
extern u8 vram[1024 * 96 * 2];
extern u8 bios_rom[0x8000];
#else
extern u16 *palette_ram;
extern u16 *oam_ram;
extern u16 *io_registers;
extern u8 *ewram;
extern u8 *iwram;
extern u8 *vram;
extern u8 *bios_rom;
#endif

extern u32 bios_read_protect;

extern u8 *memory_map_read[8 * 1024];
extern u32 reg[64];
extern u8 *memory_map_write[8 * 1024];

extern FILE_TAG_TYPE gamepak_file_large;

extern u32 gbc_sound_wave_update;

#ifdef OLD_COUNT
extern u32 waitstate_cycles_sequential[16][3];
#else
extern u8 waitstate_cycles_seq[2][16];
extern u8 waitstate_cycles_non_seq[2][16];
extern u8 cpu_waitstate_cycles_seq[2][16];
#endif

// SIO
extern u32 g_multi_mode;
extern u32 g_adhoc_transfer_flag;

// 関数宣言

extern u8 read_memory8(u32 address);
extern u32 read_memory16(u32 address);
extern u16 read_memory16_signed(u32 address);
extern u32 read_memory32(u32 address);

extern u8 *memory_region(u32 address, u32 *memory_limit);
extern s32 load_bios(char *name);
extern s32 load_gamepak(char *file_path);
extern u8 *load_gamepak_page(u32 physical_index);
extern u32 load_backup(char *name);
extern void init_memory();
extern void init_gamepak_buffer();
extern void update_backup();
extern void update_backup_force();
extern void bios_region_read_allow();
extern void bios_region_read_protect();
//extern u32 load_state(char *savestate_filename, u32 slot_num);
extern u32 load_state(char *savestate_filename, FILE *fp);
extern u32 save_state(char *savestate_filename, u16 *screen_capture);
extern void savefast_int(void);
extern void savestate_fast(void);
extern void loadstate_fast(void);

extern unsigned int savefast_queue_len;

#endif
