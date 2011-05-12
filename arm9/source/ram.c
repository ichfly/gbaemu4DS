#include "ram.h"


//===================================//
//                                   //
//       Device Ram Drivers !        //
//                                   //
//===================================//

//========================
vu16 *_sc_unlock ()
//========================
{
    *(vu16*)0x9FFFFFE = 0xA55A;
    *(vu16*)0x9FFFFFE = 0xA55A;
    *(vu16*)0x9FFFFFE = 0x5; // RAM_RW
    *(vu16*)0x9FFFFFE = 0x5;

    return (vu16*)0x8000000;
}

//========================
void _sc_lock ()
//========================
{
    *(vu16*)0x9FFFFFE = 0xA55A;
    *(vu16*)0x9FFFFFE = 0xA55A;
    *(vu16*)0x9FFFFFE = 0x3; // MEDIA
    *(vu16*)0x9FFFFFE = 0x3;
}

//========================
vu16 *_m3_unlock ()
//========================
{
    u32 mode = 0x00400006; // RAM_RW
    vu16 tmp;
    tmp = *(vu16*)0x08E00002;
    tmp = *(vu16*)0x0800000E;
    tmp = *(vu16*)0x08801FFC;
    tmp = *(vu16*)0x0800104A;
    tmp = *(vu16*)0x08800612;
    tmp = *(vu16*)0x08000000;
    tmp = *(vu16*)0x08801B66;
    tmp = *(vu16*)(0x08000000 + (mode << 1));
    tmp = *(vu16*)0x0800080E;
    tmp = *(vu16*)0x08000000;
    tmp = *(vu16*)0x080001E4;
    tmp = *(vu16*)0x080001E4;
    tmp = *(vu16*)0x08000188;
    tmp = *(vu16*)0x08000188;

    return (vu16*)0x8000000;
}

//========================
void _m3_lock ()
//========================
{
    u32 mode = 0x00400003; // MEDIA
    vu16 tmp;
    tmp = *(vu16*)0x08E00002;
    tmp = *(vu16*)0x0800000E;
    tmp = *(vu16*)0x08801FFC;
    tmp = *(vu16*)0x0800104A;
    tmp = *(vu16*)0x08800612;
    tmp = *(vu16*)0x08000000;
    tmp = *(vu16*)0x08801B66;
    tmp = *(vu16*)(0x08000000 + (mode << 1));
    tmp = *(vu16*)0x0800080E;
    tmp = *(vu16*)0x08000000;
    tmp = *(vu16*)0x080001E4;
    tmp = *(vu16*)0x080001E4;
    tmp = *(vu16*)0x08000188;
    tmp = *(vu16*)0x08000188;
}

//========================
vu16 *_opera_unlock ()
//========================
{
    *(vu16*)0x8240000 = 1;

    return (vu16*)0x9000000;
}

//========================
void _opera_lock ()
//========================
{
    *(vu16*)0x8240000 = 0;
}


//========================
vu16 *_g6_unlock ()
//========================
{
    u32 mode = 6; // RAM_RW
    vu16 tmp;
	tmp = *(vu16*)0x09000000;
	tmp = *(vu16*)0x09FFFFE0;
	tmp = *(vu16*)0x09FFFFEC;
	tmp = *(vu16*)0x09FFFFEC;
	tmp = *(vu16*)0x09FFFFEC;
	tmp = *(vu16*)0x09FFFFFC;
	tmp = *(vu16*)0x09FFFFFC;
	tmp = *(vu16*)0x09FFFFFC;
	tmp = *(vu16*)0x09FFFF4A;
	tmp = *(vu16*)0x09FFFF4A;
	tmp = *(vu16*)0x09FFFF4A;
	tmp = *(vu16*)(0x09200000 + (mode << 1));
	tmp = *(vu16*)0x09FFFFF0;
	tmp = *(vu16*)0x09FFFFE8;

    return (vu16*)0x8000000;
}

//========================
void _g6_lock ()
//========================
{
    u32 mode = 3; // MEDIA
    vu16 tmp;
	tmp = *(vu16*)0x09000000;
	tmp = *(vu16*)0x09FFFFE0;
	tmp = *(vu16*)0x09FFFFEC;
	tmp = *(vu16*)0x09FFFFEC;
	tmp = *(vu16*)0x09FFFFEC;
	tmp = *(vu16*)0x09FFFFFC;
	tmp = *(vu16*)0x09FFFFFC;
	tmp = *(vu16*)0x09FFFFFC;
	tmp = *(vu16*)0x09FFFF4A;
	tmp = *(vu16*)0x09FFFF4A;
	tmp = *(vu16*)0x09FFFF4A;
	tmp = *(vu16*)(0x09200000 + (mode << 1));
	tmp = *(vu16*)0x09FFFFF0;
	tmp = *(vu16*)0x09FFFFE8;
}

//========================
vu16 *_ez_unlock ()
//========================
{
    return (vu16*)0x8400000;
}

//========================
void _ez_lock ()
//========================
{
}


//===================================//
//                                   //
//              Ram API !            //
//                                   //
//===================================//

static vu16* (*_unlock) () = 0;
static void  (*_lock) () = 0;
static u32  _size = 0;
static RAM_TYPE _type = DETECT_RAM;
const char *_types[] = {"Unknown", "Supercard", "M3", "Opera", "G6", "EZ"};


//==========================================================
static bool  _ram_test ()
//==========================================================
{
    vu16 *ram = _unlock();

    ram[0] = 0x1234;
    if(ram[0] == 0x1234)        // test writability
    {
        _lock();

        ram[0] = 0x4321;
        if(ram[0] != 0x4321)    // test non-writability
        {
            return true;
        }
    }

    return false;
}

// Based on DSLinux Amadeus' detection
//==========================================================
static bool  _ram_test_ez ()
//==========================================================
{
	vu16 *ram;

    *(vu16*)0x9FE0000 = 0xD200; // SetRompage (OS mode)
    *(vu16*)0x8000000 = 0x1500;
    *(vu16*)0x8020000 = 0xD200;
    *(vu16*)0x8040000 = 0x1500;
    *(vu16*)0x9880000 = 0x8000;
    *(vu16*)0x9FC0000 = 0x1500;

    *(vu16*)0x9FE0000 = 0xD200; // OpenNorWrite
    *(vu16*)0x8000000 = 0x1500;
    *(vu16*)0x8020000 = 0xD200;
    *(vu16*)0x8040000 = 0x1500;
    *(vu16*)0x9C40000 = 0x1500;
    *(vu16*)0x9FC0000 = 0x1500;


    ram = (vu16*)0x08400000;

    ram[0] = 0x1234;
    if(ram[0] == 0x1234)        // test writability
    {
        ram = (vu16*)0x08000000;

        ram[0] = 0x4321;
        if(ram[0] != 0x4321)    // test non-writability
        {
            return true;
        }
    }

    *(vu16*)0x9FE0000 = 0xD200; // CloseNorWrite
    *(vu16*)0x8000000 = 0x1500;
    *(vu16*)0x8020000 = 0xD200;
    *(vu16*)0x8040000 = 0x1500;
    *(vu16*)0x9C40000 = 0xD200;
    *(vu16*)0x9FC0000 = 0x1500;

    *(vu16*)0x9FE0000 = 0xD200; // SetRompage (352)
    *(vu16*)0x8000000 = 0x1500;
    *(vu16*)0x8020000 = 0xD200;
    *(vu16*)0x8040000 = 0x1500;
    *(vu16*)0x9880000 = 352;
    *(vu16*)0x9FC0000 = 0x1500;

    *(vu16*)0x9FE0000 = 0xD200; // OpenNorWrite
    *(vu16*)0x8000000 = 0x1500;
    *(vu16*)0x8020000 = 0xD200;
    *(vu16*)0x8040000 = 0x1500;
    *(vu16*)0x9C40000 = 0x1500;
    *(vu16*)0x9FC0000 = 0x1500;

    ram = (vu16*)0x08400000;
    
    ram[0] = 0x1234;
    if(ram[0] == 0x1234)        // test writability
    {
        return true;
    }
	
	return false;
}


//==========================================================
static void  _ram_precalc_size ()
//==========================================================
{
    if(_unlock == 0 || _lock == 0)
        return;
        
    vu16 *ram = _unlock();
    _size = 0;

    ram[0] = 0x2468;
    while(ram[_size] == 0x2468)
    {
        ram[_size] = 0;
        _size += 512;
        ram[_size] = 0x2468;
    }
    _size<<=1;

    _lock();
}


//==========================================================
bool  ram_init (RAM_TYPE type)
//==========================================================
{
    sysSetBusOwners(BUS_OWNER_ARM9, BUS_OWNER_ARM9);

    switch(type)
    {
        case SC_RAM:
        {
            _unlock = _sc_unlock;
            _lock   = _sc_lock;
            _type   = SC_RAM;
        }
        break;

        case M3_RAM:
        {
            _unlock = _m3_unlock;
            _lock   = _m3_lock;
            _type   = M3_RAM;
        }
        break;

        case OPERA_RAM:
        {
            _unlock = _opera_unlock;
            _lock   = _opera_lock;
            _type   = OPERA_RAM;
        }
        break;

        case G6_RAM:
        {
            _unlock = _g6_unlock;
            _lock   = _g6_lock;
            _type   = G6_RAM;
        }
        break;

        case EZ_RAM:
        {
            _unlock = _ez_unlock;
            _lock   = _ez_lock;
            _type   = EZ_RAM;
        }
        break;

        case DETECT_RAM:
        default:
        {
            // try ez
            _unlock = _ez_unlock;
            _lock   = _ez_lock;
            _type   = EZ_RAM;
            
            if(_ram_test_ez())
            {
                break;
            }

            // try supercard
            _unlock = _sc_unlock;
            _lock   = _sc_lock;
            _type   = SC_RAM;

            if(_ram_test())
            {
                break;
            }

            // try m3
            _unlock = _m3_unlock;
            _lock   = _m3_lock;
            _type   = M3_RAM;

            if(_ram_test())
            {
                break;
            }

            // try opera
            _unlock = _opera_unlock;
            _lock   = _opera_lock;
            _type   = OPERA_RAM;

            if(_ram_test())
            {
                break;
            }

            // try g6
            _unlock = _g6_unlock;
            _lock   = _g6_lock;
            _type   = G6_RAM;
            
            if(_ram_test())
            {
                break;
            }

            // fail
            _unlock = 0;
            _lock   = 0;
            _type   = DETECT_RAM;

            return false;
        }
        break;
    }
    
    _ram_precalc_size();
    
    return true;
}


//==========================================================
RAM_TYPE   ram_type ()
//==========================================================
{
    return _type;
}


//==========================================================
const char*   ram_type_string ()
//==========================================================
{
    return _types[(int)_type];
}


//==========================================================
u32   ram_size ()
//==========================================================
{
    return _size;
}


//==========================================================
vu16* ram_unlock ()
//==========================================================
{
    sysSetBusOwners(BUS_OWNER_ARM9, BUS_OWNER_ARM9);

    if(_unlock)
        return _unlock();
    return 0;
}


//==========================================================
void  ram_lock ()
//==========================================================
{
    sysSetBusOwners(BUS_OWNER_ARM9, BUS_OWNER_ARM9);

    if(_lock)
        _lock();
}


