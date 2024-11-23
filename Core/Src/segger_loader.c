/***********************************************************************/
/*  This file is part of the ARM Toolchain package                     */
/*  Copyright (c) 2020 Keil - An ARM Company. All rights reserved.     */
/***********************************************************************/
/*                                                                     */
/*  FlashDev.c:  Device Description for ST STM32L476G-DISCO Flash      */
/*                                                                     */
/***********************************************************************/

#include "FlashOS.h" // FlashOS Structures
#include "quadspi.h"
#include "w25qxx.h"
#include "segger_loader.h"
#include "stldr_loader.h"

#define PrgCode __attribute__((section("PrgCode"), __used__))
#define DevDescr __attribute__((section("DevDscr")))

struct FlashDevice const FlashDevice DevDescr = {
    FLASH_DRV_VERS,          // Driver Version, do not modify!
    "W25Q16_STM32L4xx-QSPI", // Device Name
    EXTSPI,                  // Device Type
    0x90000000,              // Device Start Address
    MEMORY_FLASH_SIZE,       // Device Size in Bytes (2MB)
    MEMORY_PAGE_SIZE,        // Programming Page Size 4096 Bytes
    0x00,                    // Reserved, must be 0
    0xFF,                    // Initial Content of Erased Memory
    10000,                   // Program Page Timeout 100 mSec
    6000,                    // Erase Sector Timeout 6000 mSec

    // Specify Size and Address of Sectors
    {{MEMORY_SECTOR_SIZE, 0x00000000}, // Sector Size  64kB, Sector Num : 256
     {SECTOR_END}}};

int PrgCode SEGGER_FL_Prepare(unsigned long PreparePara0, unsigned long PreparePara1, unsigned long PreparePara2)
{
    (void)PreparePara0;
    (void)PreparePara1;
    (void)PreparePara2;
    return (Init() == 1) ? (0) : (-1);
}

int PrgCode SEGGER_FL_Restore(unsigned long RestorePara0, unsigned long RestorePara1, unsigned long RestorePara2)
{
    (void)RestorePara0;
    (void)RestorePara1;
    (void)RestorePara2;
    return 0;
}

int PrgCode SEGGER_FL_Program(unsigned long DestAddr, unsigned long NumBytes, unsigned char *pSrcBuff)
{
    return (Write(DestAddr, NumBytes, pSrcBuff) == 1) ? (0) : (-1);
}

int PrgCode SEGGER_FL_Erase(unsigned long SectorAddr, unsigned long SectorIndex, unsigned long NumSectors)
{
    uint32_t start = (SectorAddr - MEMORY_BASE_ADDR) / MEMORY_SECTOR_SIZE;

    w25qxx_exit_memory_mapped_mode();
    for (uint32_t i = start + SectorIndex; i < (SectorIndex + NumSectors); i++)
    {
        if (HAL_OK != w25qxx_erase_sector(i))
        {
            return -1;
        }
    }

    return 0;
}

int PrgCode SEGGER_FL_EraseChip(void)
{
    return (MassErase() == 1) ? (0) : (-1);
}

int PrgCode SEGGER_FL_Read(unsigned long Addr, unsigned long NumBytes, unsigned char *pDestBuff)
{
    return (Read(Addr, NumBytes, pDestBuff) == 1) ? (0) : (-1);
}