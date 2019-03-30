// VisualBoyAdvance - Nintendo Gameboy/GameboyAdvance (TM) emulator.
// Copyright (C) 1999-2003 Forgotten
// Copyright (C) 2005 Forgotten and the VBA development team

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or(at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

/************************************************************************/
/* Arm/Thumb command set disassembler                                   */
/************************************************************************/

#include "ichflysettings.h"
#ifndef noichflydebugger
#include <stdio.h>

#include "System.h"
#include "GBA.h"
#include "armdis.h"
#include "elf.h"
#include "Util.h"

struct Opcodes {
  u32 mask;
  u32 cval;
  char *mnemonic;
};

#define debuggerReadMemory(addr) \
  READ32LE(((u32*)&map[(addr)>>24].address[(addr) & map[(addr)>>24].mask]))

#define debuggerReadHalfWord(addr) \
  READ16LE(((u16*)&map[(addr)>>24].address[(addr) & map[(addr)>>24].mask]))

#define debuggerReadByte(addr) \
  map[(addr)>>24].address[(addr) & map[(addr)>>24].mask]

const char hdig[] = "0123456789abcdef";

const char *decVals[16] = {
  "0","1","2","3","4","5","6","7","8",
  "9","10","11","12","13","14","15"
};

const char *regs[16] = {
  "r0","r1","r2","r3","r4","r5","r6","r7",
  "r8","r9","r10","r11","r12","sp","lr","pc"
};

const char *conditions[16] = {
  "eq","ne","cs","cc","mi","pl","vs","vc",
  "hi","ls","ge","lt","gt","le","","nv"
};

const char *shifts[5] = {
  "lsl","lsr","asr","ror","rrx"
};

const char *armMultLoadStore[12] = {
  // non-stack
  "da","ia","db","ib",
  // stack store
  "ed","ea","fd","fa",
  // stack load
  "fa","fd","ea","ed"
};

const Opcodes thumbOpcodes[] = {
  // Format 1
  {0xf800, 0x0000, (char*)"lsl %r0, %r3, %o"},
  {0xf800, 0x0800, (char*)"lsr %r0, %r3, %o"},
  {0xf800, 0x1000, (char*)"asr %r0, %r3, %o"},
  // Format 2
  {0xfe00, 0x1800, (char*)"add %r0, %r3, %r6"},
  {0xfe00, 0x1a00, (char*)"sub %r0, %r3, %r6"},
  {0xfe00, 0x1c00, (char*)"add %r0, %r3, %i"},
  {0xfe00, 0x1e00, (char*)"sub %r0, %r3, %i"},
  // Format 3
  {0xf800, 0x2000, (char*)"mov %r8, %O"},
  {0xf800, 0x2800, (char*)"cmp %r8, %O"},
  {0xf800, 0x3000, (char*)"add %r8, %O"},
  {0xf800, 0x3800, (char*)"sub %r8, %O"},
  // Format 4
  {0xffc0, 0x4000, (char*)"and %r0, %r3"},
  {0xffc0, 0x4040, (char*)"eor %r0, %r3"},
  {0xffc0, 0x4080, (char*)"lsl %r0, %r3"},
  {0xffc0, 0x40c0, (char*)"lsr %r0, %r3"},
  {0xffc0, 0x4100, (char*)"asr %r0, %r3"},
  {0xffc0, 0x4140, (char*)"adc %r0, %r3"},
  {0xffc0, 0x4180, (char*)"sbc %r0, %r3"},
  {0xffc0, 0x41c0, (char*)"ror %r0, %r3"},
  {0xffc0, 0x4200, (char*)"tst %r0, %r3"},
  {0xffc0, 0x4240, (char*)"neg %r0, %r3"},
  {0xffc0, 0x4280, (char*)"cmp %r0, %r3"},
  {0xffc0, 0x42c0, (char*)"cmn %r0, %r3"},
  {0xffc0, 0x4300, (char*)"orr %r0, %r3"},
  {0xffc0, 0x4340, (char*)"mul %r0, %r3"},
  {0xffc0, 0x4380, (char*)"bic %r0, %r3"},
  {0xffc0, 0x43c0, (char*)"mvn %r0, %r3"},
  // Format 5
  {0xff80, 0x4700, (char*)"bx %h36"},
  {0xfcc0, 0x4400, (char*)"[ ??? ]"},
  {0xff00, 0x4400, (char*)"add %h07, %h36"},
  {0xff00, 0x4500, (char*)"cmp %h07, %h36"},
  {0xff00, 0x4600, (char*)"mov %h07, %h36"},
  // Format 6
  {0xf800, 0x4800, (char*)"ldr %r8, [%I] (=%J)"},
  // Format 7
  {0xfa00, 0x5000, (char*)"str%b %r0, [%r3, %r6]"},
  {0xfa00, 0x5800, (char*)"ldr%b %r0, [%r3, %r6]"},
  // Format 8
  {0xfe00, 0x5200, (char*)"strh %r0, [%r3, %r6]"},
  {0xfe00, 0x5600, (char*)"ldsb %r0, [%r3, %r6]"},
  {0xfe00, 0x5a00, (char*)"ldrh %r0, [%r3, %r6]"},
  {0xfe00, 0x5e00, (char*)"ldsh %r0, [%r3, %r6]"},
  // Format 9
  {0xe800, 0x6000, (char*)"str%B %r0, [%r3, %p]"},
  {0xe800, 0x6800, (char*)"ldr%B %r0, [%r3, %p]"},
  // Format 10
  {0xf800, 0x8000, (char*)"strh %r0, [%r3, %e]"},
  {0xf800, 0x8800, (char*)"ldrh %r0, [%r3, %e]"},
  // Format 11
  {0xf800, 0x9000, (char*)"str %r8, [sp, %w]"},
  {0xf800, 0x9800, (char*)"ldr %r8, [sp, %w]"},
  // Format 12
  {0xf800, 0xa000, (char*)"add %r8, pc, %w (=%K)"},
  {0xf800, 0xa800, (char*)"add %r8, sp, %w"},
  // Format 13
  {0xff00, 0xb000, (char*)"add sp, %s"},
  // Format 14
  {0xffff, 0xb500, (char*)"push {lr}"},  
  {0xff00, 0xb400, (char*)"push {%l}"},
  {0xff00, 0xb500, (char*)"push {%l,lr}"},
  {0xffff, 0xbd00, (char*)"pop {pc}"},
  {0xff00, 0xbd00, (char*)"pop {%l,pc}"},  
  {0xff00, 0xbc00, (char*)"pop {%l}"},
  // Format 15
  {0xf800, 0xc000, (char*)"stmia %r8!, {%l}"},
  {0xf800, 0xc800, (char*)"ldmia %r8!, {%l}"},
  // Format 17
  {0xff00, 0xdf00, (char*)"swi %m"},
  // Format 16
  {0xf000, 0xd000, (char*)"b%c %W"},
  // Format 18
  {0xf800, 0xe000, (char*)"b %a"},
  // Format 19
  {0xf800, 0xf000, (char*)"bl %A"},
  {0xf800, 0xf800, (char*)"blh %Z"},
  {0xff00, 0xbe00, (char*)"bkpt %O"},
  // Unknown
  {0x0000, 0x0000, (char*)"[ ??? ]"}
};

const Opcodes armOpcodes[] = {
  // Undefined
  {0x0e000010, 0x06000010, (char*)"[ undefined ]"},
  // Branch instructions
  {0x0ff000f0, 0x01200010, (char*)"bx%c %r0"},
  {0x0f000000, 0x0a000000, (char*)"b%c %o"},
  {0x0f000000, 0x0b000000, (char*)"bl%c %o"},
  {0x0f000000, 0x0f000000, (char*)"swi%c %q"},
  // PSR transfer
  {0x0fbf0fff, 0x010f0000, (char*)"mrs%c %r3, %p"},
  {0x0db0f000, 0x0120f000, (char*)"msr%c %p, %i"},
  // Multiply instructions
  {0x0fe000f0, 0x00000090, (char*)"mul%c%s %r4, %r0, %r2"},
  {0x0fe000f0, 0x00200090, (char*)"mla%c%s %r4, %r0, %r2, %r3"},
  {0x0fa000f0, 0x00800090, (char*)"%umull%c%s %r3, %r4, %r0, %r2"},
  {0x0fa000f0, 0x00a00090, (char*)"%umlal%c%s %r3, %r4, %r0, %r2"},
  // Load/Store instructions
  {0x0fb00ff0, 0x01000090, (char*)"swp%c%b %r3, %r0, [%r4]"},
  {0x0fb000f0, 0x01000090, (char*)"[ ??? ]"},
  {0x0c100000, 0x04000000, (char*)"str%c%b%t %r3, %a"},
  {0x0c100000, 0x04100000, (char*)"ldr%c%b%t %r3, %a"},
  {0x0e100090, 0x00000090, (char*)"str%c%h %r3, %a"},
  {0x0e100090, 0x00100090, (char*)"ldr%c%h %r3, %a"},
  {0x0e100000, 0x08000000, (char*)"stm%c%m %r4%l"},
  {0x0e100000, 0x08100000, (char*)"ldm%c%m %r4%l"},
  // Data processing
  {0x0de00000, 0x00000000, (char*)"and%c%s %r3, %r4, %i"},
  {0x0de00000, 0x00200000, (char*)"eor%c%s %r3, %r4, %i"},
  {0x0de00000, 0x00400000, (char*)"sub%c%s %r3, %r4, %i"},
  {0x0de00000, 0x00600000, (char*)"rsb%c%s %r3, %r4, %i"},
  {0x0de00000, 0x00800000, (char*)"add%c%s %r3, %r4, %i"},
  {0x0de00000, 0x00a00000, (char*)"adc%c%s %r3, %r4, %i"},
  {0x0de00000, 0x00c00000, (char*)"sbc%c%s %r3, %r4, %i"},
  {0x0de00000, 0x00e00000, (char*)"rsc%c%s %r3, %r4, %i"},
  {0x0de00000, 0x01000000, (char*)"tst%c%s %r4, %i"},
  {0x0de00000, 0x01200000, (char*)"teq%c%s %r4, %i"},
  {0x0de00000, 0x01400000, (char*)"cmp%c%s %r4, %i"},
  {0x0de00000, 0x01600000, (char*)"cmn%c%s %r4, %i"},
  {0x0de00000, 0x01800000, (char*)"orr%c%s %r3, %r4, %i"},
  {0x0de00000, 0x01a00000, (char*)"mov%c%s %r3, %i"},
  {0x0de00000, 0x01c00000, (char*)"bic%c%s %r3, %r4, %i"},
  {0x0de00000, 0x01e00000, (char*)"mvn%c%s %r3, %i"},
  // Coprocessor operations
  {0x0f000010, 0x0e000000, (char*)"cdp%c %P, %N, %r3, %R4, %R0%V"},
  {0x0e100000, 0x0c000000, (char*)"stc%c%L %P, %r3, %A"},
  {0x0f100010, 0x0e000010, (char*)"mcr%c %P, %N, %r3, %R4, %R0%V"},
  {0x0f100010, 0x0e100010, (char*)"mrc%c %P, %N, %r3, %R4, %R0%V"},
  // Unknown
  {0x00000000, 0x00000000, (char*)"[ ??? ]"}
};

char* addStr(char *dest, const char *src){
  while (*src){
    *dest++ = *src++;
  }
  return dest;
}

char* addHex(char *dest, int siz, u32 val){
  if (siz==0){
    siz = 28;
    while ( (((val>>siz)&15)==0) && (siz>=4) )
      siz -= 4;
    siz += 4;
  }
  while (siz>0){
    siz -= 4;
    *dest++ = hdig[(val>>siz)&15];
  }
  return dest;
}

int disArm(u32 offset, char *dest, int flags){
  u32 opcode = debuggerReadMemory(offset);
        
  const Opcodes *sp = armOpcodes;
  while( sp->cval != (opcode & sp->mask) )
    sp++;

  if (flags&DIS_VIEW_ADDRESS){
    dest = addHex(dest, 32, offset);
    *dest++ = ' ';
  }
  if (flags&DIS_VIEW_CODE){
    dest = addHex(dest, 32, opcode);
    *dest++ = ' ';
  }

  char *src = sp->mnemonic;
  while (*src){
    if (*src!='%')
      *dest++ = *src++;
    else{
      src++;
      switch (*src){
      case 'c':
        dest = addStr(dest, conditions[opcode>>28]);
        break;
      case 'r':
        dest = addStr(dest, regs[(opcode>>((*(++src)-'0')*4))&15]);
        break;
      case 'o':
        {
          *dest++ = '$';
          int off = opcode&0xffffff;
          if (off&0x800000)
            off |= 0xff000000;
          off <<= 2;
          dest = addHex(dest, 32, offset+8+off);
        }
        break;
      case 'i':
        if (opcode&(1<<25)){
          dest = addStr(dest, "#0x");
          int imm = opcode&0xff;
          int rot = (opcode&0xf00)>>7;
          int val = (imm<<(32-rot))|(imm>>rot);
          dest = addHex(dest, 0, val);
        } else{
          dest = addStr(dest, regs[opcode&0x0f]);
          int shi = (opcode>>5)&3;
          int sdw = (opcode>>7)&0x1f;
          if ((sdw==0)&&(shi==3))
            shi = 4;
          if ( (sdw) || (opcode&0x10) || (shi)) {
            dest = addStr(dest, ", ");
            dest = addStr(dest, shifts[shi]);
            if (opcode&0x10){
              *dest++ = ' ';
              dest = addStr(dest, regs[(opcode>>8)&15]);
            } else {
              if (sdw==0 && ( (shi==1) || (shi==2) ))
                sdw = 32;
              if(shi != 4) {
                dest = addStr(dest, " #0x");
                dest = addHex(dest, 8, sdw);
              }
            }
          }
        }
        break;
      case 'p':
        if (opcode&(1<<22))
          dest = addStr(dest, "spsr");
        else
          dest = addStr(dest, "cpsr");
        if(opcode & 0x00F00000) {
          *dest++ = '_';
          if(opcode & 0x00080000)
            *dest++ = 'f';
          if(opcode & 0x00040000)
            *dest++ = 's';
          if(opcode & 0x00020000)
            *dest++ = 'x';
          if(opcode & 0x00010000)
            *dest++ = 'c';
        }
        break;
      case 's':
        if (opcode&(1<<20))
          *dest++ = 's';
        break;
      case 'S':
        if (opcode&(1<<22))
          *dest++ = 's';
        break;
      case 'u':
        if (opcode&(1<<22))
          *dest++ = 's';
        else
          *dest++ = 'u';
        break;
      case 'b':
        if (opcode&(1<<22))
          *dest++ = 'b';
        break;
      case 'a':
        if ((opcode&0x076f0000)==0x004f0000){
          *dest++ = '[';
          *dest++ = '$';
          int adr = offset+8;
          int add = (opcode&15)|((opcode>>8)&0xf0);
          if (opcode&(1<<23))
            adr += add;
          else
            adr -= add;
          dest = addHex(dest, 32, adr);
          *dest++ = ']';
          dest = addStr(dest, " (=");
          *dest++ = '$';
          dest = addHex(dest ,32, debuggerReadMemory(adr));
          *dest++=')';
        }
        if ((opcode&0x072f0000)==0x050f0000){
          *dest++ = '[';
          *dest++ = '$';
          int adr = offset+8;
          if (opcode&(1<<23))
            adr += opcode&0xfff;
          else
            adr -= opcode&0xfff;
          dest = addHex(dest, 32, adr);
          *dest++ = ']';
          dest = addStr(dest, " (=");
          *dest++ = '$';
          dest = addHex(dest ,32, debuggerReadMemory(adr));
          *dest++=')';
        } else {
          int reg = (opcode>>16)&15;
          *dest++ = '[';
          dest = addStr(dest, regs[reg]);
          if (!(opcode&(1<<24)))
            *dest++ = ']';
          if ( ((opcode&(1<<25))&&(opcode&(1<<26))) || (!(opcode&(1<<22))&&!(opcode&(1<<26))) ){
            dest = addStr(dest, ", ");
            if (!(opcode&(1<<23)))
              *dest++ = '-';
            dest = addStr(dest, regs[opcode&0x0f]);
            int shi = (opcode>>5)&3;
            if (opcode&(1<<26)){
              if ( ((opcode>>7)&0x1f) || (opcode&0x10) || (shi==1) || (shi==2)){
                dest = addStr(dest, ", ");
                dest = addStr(dest, shifts[shi]);
                if (opcode&0x10){
                  *dest++ = ' ';
                  dest = addStr(dest, regs[(opcode>>8)&15]);
                } else {
                  int sdw = (opcode>>7)&0x1f;
                  if (sdw==0 && ( (shi==1) || (shi==2) ))
                    sdw = 32;
                  dest = addStr(dest, " #0x");
                  dest = addHex(dest, 8, sdw);
                }
              }
            }
          } else {
            int off;
            if (opcode&(1<<26))
              off = opcode&0xfff;
            else
              off = (opcode&15)|((opcode>>4)&0xf0);
            if (off){
              dest = addStr(dest, ", ");
              if (!(opcode&(1<<23)))
                *dest++ = '-';
              dest = addStr(dest, "#0x");
              dest = addHex(dest, 0, off);
            }
          }
          if (opcode&(1<<24)){
            *dest++ = ']';
            if (opcode&(1<<21))
              *dest++ = '!';
          }
        }
        break;
      case 't':
        if ((opcode&0x01200000)==0x01200000)
          *dest++ = 't';
        break;
      case 'h':
        if (opcode&(1<<6))
          *dest++ = 's';
        if (opcode&(1<<5))
          *dest++ = 'h';
        else
          *dest++ = 'b';
        break;
      case 'm':
        if (((opcode>>16)&15)==13) {
          if(opcode & 0x00100000)
            dest = addStr(dest, armMultLoadStore[8+((opcode>>23)&3)]);
          else
            dest = addStr(dest, armMultLoadStore[4+((opcode>>23)&3)]);      
        } else
          dest = addStr(dest, armMultLoadStore[(opcode>>23)&3]);
        break;
      case 'l':
        if (opcode&(1<<21))
          *dest++ = '!';
        dest = addStr(dest, ", {");
        {
          int rlst = opcode&0xffff;
          int msk = 0;
          int not_first = 0;
          while (msk<16){
            if (rlst&(1<<msk)){
              int fr = msk;
              while (rlst&(1<<msk))
                msk++;
              int to = msk-1;
              if (not_first)
                //dest = addStr(dest, ", ");
                *dest++ = ',';
              dest = addStr(dest, regs[fr]);
              if (fr!=to){
                if (fr==to-1)
                  //dest = addStr(", ");
                  *dest++ = ',';
                else
                  *dest++ = '-';
                dest = addStr(dest, regs[to]);
              }
              not_first = 1;
            } else
              msk++;
          }
          *dest++ = '}';
          if (opcode&(1<<22))
            *dest++ = '^';
        }
        break;
      case 'q':
        *dest++ = '$';
        dest = addHex(dest, 24, opcode&0xffffff);
        break;
      case 'P':
        *dest++ = 'p';
        dest = addStr(dest, decVals[(opcode>>8)&15]);
        break;
      case 'N':
        if (opcode&0x10)
          dest = addStr(dest, decVals[(opcode>>21)&7]);
        else
          dest = addStr(dest, decVals[(opcode>>20)&15]);
        break;
      case 'R':
        {
          src++;
          int reg = 4*(*src-'0');
          *dest++ = 'c';
          dest = addStr(dest, decVals[(opcode>>reg)&15]);
        }
        break;
      case 'V':
        {
          int val = (opcode>>5)&7;
          if (val){
            dest = addStr(dest, ", ");
            dest = addStr(dest, decVals[val]);
          }
        }
        break;
      case 'L':
        if (opcode&(1<<22))
          *dest++ = 'l';
        break;
      case 'A':
        if ((opcode&0x012f0000)==0x010f0000){
          int adr = offset+8;
          int add = (opcode&0xff)<<2;
          if (opcode&(1<<23))
            adr += add;
          else
            adr -= add;
          *dest++ = '$';
          addHex(dest, 32, adr);
        } else {
          *dest++ = '[';
          dest = addStr(dest, regs[(opcode>>16)&15]);
          if (!(opcode&(1<<24)))
            *dest++ = ']';
          int off = (opcode&0xff)<<2;
          if (off){
            dest = addStr(dest, ", ");
            if (!(opcode&(1<<23)))
              *dest++ = '-';
            dest = addStr(dest, "#0x");
            dest = addHex(dest, 0, off);
          }
          if (opcode&(1<<24)){
            *dest++ = ']';
            if (opcode&(1<<21))
              *dest++ = '!';
          }
        }
        break;
      }
      src++;
    }
  }
  *dest++ = 0;
        
  return 4;
}

int disThumb(u32 offset, char *dest, int flags){
  u32 opcode = debuggerReadHalfWord(offset);
        
  const Opcodes *sp = thumbOpcodes;
  int ret = 2;
  while( sp->cval != (opcode & sp->mask) )
    sp++;
        
  if (flags&DIS_VIEW_ADDRESS){
    dest = addHex(dest, 32, offset);
    *dest++ = ' ';
  }
  if (flags&DIS_VIEW_CODE){
    dest = addHex(dest, 16, opcode);
    *dest++ = ' ';
  }
        
  char *src = sp->mnemonic;
  while (*src){
    if (*src!='%')
      *dest++ = *src++;
    else {
      src++;
      switch (*src){
      case 'r':
        src++;
        dest = addStr(dest, regs[(opcode>>(*src-'0'))&7]);
        break;
      case 'o':
        dest = addStr(dest, "#0x");
        {
          int val = (opcode>>6)&0x1f;
          dest = addHex(dest, 8, val);
        }
        break;
      case 'p':
        dest = addStr(dest, "#0x");
        {
          int val = (opcode>>6)&0x1f;
          if (!(opcode&(1<<12)))
            val <<= 2;
          dest = addHex(dest, 0, val);
        }
        break;
      case 'e':
        dest = addStr(dest, "#0x");
        dest = addHex(dest, 0, ((opcode>>6)&0x1f)<<1);
        break;
      case 'i':
        dest = addStr(dest, "#0x");
        dest = addHex(dest, 0, (opcode>>6)&7);
        break;
      case 'h':
        {
          src++;
          int reg = (opcode>>(*src-'0'))&7;
          src++;
          if (opcode&(1<<(*src-'0')))
            reg += 8;
          dest = addStr(dest, regs[reg]);
        }
        break;
      case 'O':
        dest = addStr(dest, "#0x");
        dest = addHex(dest, 0, (opcode&0xff));
        break;
      case 'I':
        *dest++ = '$';
        dest = addHex(dest, 32, (offset&0xfffffffc)+4+((opcode&0xff)<<2));
        break;
      case 'b':
        if (opcode&(1<<10))
          *dest++ = 'b';
        break;
      case 'B':
        if (opcode&(1<<12))
          *dest++ = 'b';
        break;
      case 'w':
        dest = addStr(dest, "#0x");
        dest = addHex(dest, 0, (opcode&0xff)<<2);
        break;
      case 'W':
        *dest++ = '$';
        {
          int add = opcode&0xff;
          if (add&0x80)
            add |= 0xffffff00;
          dest = addHex(dest, 32, (offset&0xfffffffe)+4+(add<<1));
        }
        break;
      case 'c':
        dest = addStr(dest, conditions[(opcode>>8)&15]);
        break;
      case 's':
        if (opcode&(1<<7))
          *dest++ = '-';
        dest = addStr(dest, "#0x");
        dest = addHex(dest, 0, (opcode&0x7f)<<2);
        break;
      case 'l':
        {
          int rlst = opcode&0xff;
          int msk = 0;
          int not_first = 0;
          while (msk<8){
            if (rlst&(1<<msk)){
              int fr = msk;
              while (rlst&(1<<msk))
                msk++;
              int to = msk-1;
              if (not_first)
                *dest++ = ',';
              dest = addStr(dest, regs[fr]);
              if (fr!=to){
                if (fr==to-1)
                  *dest++ = ',';
                else
                  *dest++ = '-';
                dest = addStr(dest, regs[to]);
              }
              not_first = 1;
            } else
              msk++;
          }
        }
        break;
      case 'm':
        *dest++ = '$';
        dest = addHex(dest, 8, opcode&0xff);
        break;
      case 'Z':
        *dest++ = '$';
        dest = addHex(dest, 16, (opcode&0x7ff)<<1);
        break;
      case 'a':
        *dest++ = '$';
        {
          int add = opcode&0x07ff;
          if (add&0x400)
            add |= 0xfffff800;
          add <<= 1;
          dest = addHex(dest, 32, offset+4+add);
        }
        break;
      }
      src++;
    }
  }
  *dest++ = 0;
  return ret;
}
#endif