#pragma once


#define MEMORY_BASE_ADDR 0x90000000
#define MEMORY_FLASH_SIZE 0x200000
#define MEMORY_PAGE_SIZE  0x100
#define MEMORY_SECTOR_SIZE 0x1000

void w25qxx_init(void);
HAL_StatusTypeDef w25qxx_erase_sector(uint32_t SectorAddress);
HAL_StatusTypeDef w25qxx_erase_block(uint32_t BlockAddress);
HAL_StatusTypeDef w25qxx_erase_chip(void);
HAL_StatusTypeDef w25qxx_program_page(uint8_t *pData, uint32_t WriteAddr, uint32_t Size);
HAL_StatusTypeDef w25qxx_read(uint8_t *pData, uint32_t ReadAddr, uint32_t Size);
HAL_StatusTypeDef w25qxx_write(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite);
HAL_StatusTypeDef w25qxx_enter_memory_mapped_mode(void);
HAL_StatusTypeDef w25qxx_exit_memory_mapped_mode(void);