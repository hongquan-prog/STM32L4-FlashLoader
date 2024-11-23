#pragma once

#include "quadspi.h"

#define SECTION(x) __attribute__((section(x)))

int Init(void) SECTION(".loader");
int Read(uint32_t Address, uint32_t Size, uint8_t *Buffer) SECTION(".loader");
int Write(uint32_t Address, uint32_t Size, uint8_t *buffer) SECTION(".loader");
int SectorErase(uint32_t EraseStartAddress, uint32_t EraseEndAddress) SECTION(".loader");
int MassErase(void) SECTION(".loader");
uint32_t CheckSum(uint32_t StartAddress, uint32_t Size, uint32_t InitVal) SECTION(".loader");
uint64_t Verify(uint32_t MemoryAddr, uint32_t RAMBufferAddr, uint32_t Size, uint32_t missalignement) SECTION(".loader");