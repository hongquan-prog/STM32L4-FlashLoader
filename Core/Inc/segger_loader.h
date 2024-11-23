#pragma once

#include "FlashOS.h"

int SEGGER_FL_Prepare(unsigned long PreparePara0, unsigned long PreparePara1, unsigned long PreparePara2);
int SEGGER_FL_Restore(unsigned long RestorePara0, unsigned long RestorePara1, unsigned long RestorePara2);
int SEGGER_FL_Program(unsigned long DestAddr, unsigned long NumBytes, unsigned char *pSrcBuff);
int SEGGER_FL_Erase(unsigned long SectorAddr, unsigned long SectorIndex, unsigned long NumSectors);
int SEGGER_FL_EraseChip(void);
int SEGGER_FL_Read(unsigned long Addr, unsigned long NumBytes, unsigned char *pDestBuff);
