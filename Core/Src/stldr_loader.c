#include "main.h"
#include "gpio.h"
#include "w25qxx.h"
#include "stldr_loader.h"
#include "DevInf.h"

#define LOADER_OK 0x1
#define LOADER_FAIL 0x0

struct StorageInfo const StorageInfo __attribute__((section(".dev_info"))) = {
    "W25Q16_STM32L4xx-QSPI",             // Device Name + version number
    NOR_FLASH,                           // Device Type
    0x90000000,                          // Device Start Address
    MEMORY_FLASH_SIZE,                   // Device Size in Bytes
    MEMORY_PAGE_SIZE,                    // Programming Page Size
    0xFF,                                // Initial Content of Erased Memory

    // Specify Size and Address of Sectors (view example below)
    {   {
            (MEMORY_FLASH_SIZE / MEMORY_SECTOR_SIZE),  // Sector Numbers,
            MEMORY_SECTOR_SIZE                         // Sector Size
        },
        { 0x00000000, 0x00000000 }
    }
};

extern void SystemClock_Config(void);

/**
 * @brief  System initialization.
 * @param  None
 * @retval  LOADER_OK = 1   : Operation succeeded
 * @retval  LOADER_FAIL = 0 : Operation failed
 */
int Init(void)
{
    hqspi.Instance = QUADSPI;
	HAL_QSPI_DeInit(&hqspi);
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_QUADSPI_Init();
    w25qxx_init();

    return LOADER_OK;
}

/**
 * @brief   Program memory.
 * @param   Address: page address
 * @param   Size   : size of data
 * @param   buffer : pointer to data buffer
 * @retval  LOADER_OK = 1       : Operation succeeded
 * @retval  LOADER_FAIL = 0 : Operation failed
 */
int Write(uint32_t Address, uint32_t Size, uint8_t *buffer)
{
    w25qxx_exit_memory_mapped_mode();
    if (w25qxx_write(buffer, Address - MEMORY_BASE_ADDR, Size) != HAL_OK)
    {
        return LOADER_FAIL;
    }

    return LOADER_OK;
}

int Read(uint32_t Address, uint32_t Size, uint8_t *Buffer)
{
    unsigned int i = 0;

    w25qxx_enter_memory_mapped_mode();
    for (i = 0; i < Size; i++)
    {
        *(uint8_t *)Buffer++ = *(uint8_t *)Address;
        Address++;
    }

    return 1;
}

/**
 * @brief   Sector erase.
 * @param   EraseStartAddress :  erase start address
 * @param   EraseEndAddress   :  erase end address
 * @retval  LOADER_OK = 1       : Operation succeeded
 * @retval  LOADER_FAIL = 0 : Operation failed
 */
int SectorErase(uint32_t EraseStartAddress, uint32_t EraseEndAddress)
{
    w25qxx_exit_memory_mapped_mode();
    while (EraseEndAddress >= EraseStartAddress)
    {
        if (w25qxx_erase_sector(EraseStartAddress - MEMORY_BASE_ADDR) != HAL_OK)
            return LOADER_FAIL;
        EraseStartAddress += MEMORY_SECTOR_SIZE;
    }

    return LOADER_OK;
}

/**
 * Description :
 * Mass erase of external flash area
 * Optional command - delete in case usage of mass erase is not planed
 * Inputs    :
 *      none
 * outputs   :
 *     none
 * Note: Optional for all types of device
 */
int MassErase(void)
{
    w25qxx_exit_memory_mapped_mode();
    if (HAL_OK != w25qxx_erase_chip())
    {
        return LOADER_FAIL;
    }

    return LOADER_OK;
}

/**
 * Description :
 * Calculates checksum value of the memory zone
 * Inputs    :
 *      StartAddress  : Flash start address
 *      Size          : Size (in WORD)
 *      InitVal       : Initial CRC value
 * outputs   :
 *     R0             : Checksum value
 * Note: Optional for all types of device
 */
uint32_t CheckSum(uint32_t StartAddress, uint32_t Size, uint32_t InitVal)
{
    uint8_t missalignementAddress = StartAddress % 4;
    uint8_t missalignementSize = Size;
    unsigned int cnt;
    uint32_t Val;

    StartAddress -= StartAddress % 4;
    Size += (Size % 4 == 0) ? 0 : 4 - (Size % 4);

    for (cnt = 0; cnt < Size; cnt += 4)
    {
        Val = *(uint32_t *)StartAddress;
        if (missalignementAddress)
        {
            switch (missalignementAddress)
            {
            case 1:
                InitVal += (uint8_t)(Val >> 8 & 0xff);
                InitVal += (uint8_t)(Val >> 16 & 0xff);
                InitVal += (uint8_t)(Val >> 24 & 0xff);
                missalignementAddress -= 1;
                break;
            case 2:
                InitVal += (uint8_t)(Val >> 16 & 0xff);
                InitVal += (uint8_t)(Val >> 24 & 0xff);
                missalignementAddress -= 2;
                break;
            case 3:
                InitVal += (uint8_t)(Val >> 24 & 0xff);
                missalignementAddress -= 3;
                break;
            }
        }
        else if ((Size - missalignementSize) % 4 && (Size - cnt) <= 4)
        {
            switch (Size - missalignementSize)
            {
            case 1:
                InitVal += (uint8_t)Val;
                InitVal += (uint8_t)(Val >> 8 & 0xff);
                InitVal += (uint8_t)(Val >> 16 & 0xff);
                missalignementSize -= 1;
                break;
            case 2:
                InitVal += (uint8_t)Val;
                InitVal += (uint8_t)(Val >> 8 & 0xff);
                missalignementSize -= 2;
                break;
            case 3:
                InitVal += (uint8_t)Val;
                missalignementSize -= 3;
                break;
            }
        }
        else
        {
            InitVal += (uint8_t)Val;
            InitVal += (uint8_t)(Val >> 8 & 0xff);
            InitVal += (uint8_t)(Val >> 16 & 0xff);
            InitVal += (uint8_t)(Val >> 24 & 0xff);
        }
        StartAddress += 4;
    }

    return (InitVal);
}

/**
 * Description :
 * Verify flash memory with RAM buffer and calculates checksum value of
 * the programmed memory
 * Inputs    :
 *      FlashAddr     : Flash address
 *      RAMBufferAddr : RAM buffer address
 *      Size          : Size (in WORD)
 *      InitVal       : Initial CRC value
 * outputs   :
 *     R0             : Operation failed (address of failure)
 *     R1             : Checksum value
 * Note: Optional for all types of device
 */
uint64_t Verify(uint32_t MemoryAddr, uint32_t RAMBufferAddr, uint32_t Size, uint32_t missalignement)
{
    uint32_t VerifiedData = 0;
    uint32_t InitVal = 0;
    uint64_t checksum = 0;

    Size *= 4;
    w25qxx_enter_memory_mapped_mode();
    checksum = CheckSum((uint32_t)MemoryAddr + (missalignement & 0xf), Size - ((missalignement >> 16) & 0xF), InitVal);
    while (Size > VerifiedData)
    {
        if (*(uint8_t *)MemoryAddr++ != *((uint8_t *)RAMBufferAddr + VerifiedData))
            return ((checksum << 32) + (MemoryAddr + VerifiedData));

        VerifiedData++;
    }

    return (checksum << 32);
}
