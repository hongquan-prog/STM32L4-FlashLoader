#include "quadspi.h"
#include "w25qxx.h"

/* =============== CMD ================ */
#define W25X_WriteEnable 0x06
#define W25X_WriteDisable 0x04
#define W25X_ReadStatusReg1 0x05
#define W25X_ReadStatusReg2 0x35
#define W25X_ReadStatusReg3 0x15
#define W25X_WriteStatusReg1 0x01
#define W25X_WriteStatusReg2 0x31
#define W25X_WriteStatusReg3 0x11
#define W25X_ReadData 0x03
#define W25X_FastReadData 0x0B
#define W25X_FastReadDual 0x3B
#define W25X_PageProgram 0x02
#define W25X_BlockErase 0xD8
#define W25X_SectorErase 0x20
#define W25X_ChipErase 0xC7
#define W25X_PowerDown 0xB9
#define W25X_ReleasePowerDown 0xAB
#define W25X_DeviceID 0xAB
#define W25X_ManufactDeviceID 0x90
#define W25X_JedecDeviceID 0x9F
#define W25X_Enable4ByteAddr 0xB7
#define W25X_Exit4ByteAddr 0xE9
#define W25X_SetReadParam 0xC0
#define W25X_EnterQSPIMode 0x38
#define W25X_ExitQSPIMode 0xFF

#define W25X_EnableReset 0x66
#define W25X_ResetDevice 0x99

#define W25X_QUAD_FAST_READ_DTR_CMD 0x0D
#define W25X_QUAD_INOUT_FAST_READ_CMD 0xEB
#define W25X_QUAD_INOUT_FAST_READ_DTR_CMD 0xED
#define W25X_QUAD_INOUT_FAST_READ_4_BYTE_ADDR_CMD 0xEC
#define W25X_QUAD_INOUT_FAST_READ_4_BYTE_DTR_CMD 0xEE
#define W25X_QUAD_ManufactDeviceID 0x94
#define W25X_QUAD_INPUT_PAGE_PROG_CMD 0x32 /*!< Page Program 3 Byte Address */

/* 4-byte Address Mode Operations */
#define W25X_ENTER_4_BYTE_ADDR_MODE_CMD 0xB7
#define W25X_EXIT_4_BYTE_ADDR_MODE_CMD 0xE9

/* Dummy cycles for DTR read mode */
#define W25X_DUMMY_CYCLES_READ_QUAD_DTR 4U
#define W25X_DUMMY_CYCLES_READ_QUAD 6U

/* Dummy cycles for Fast read mode */
#define W25X_DUMMY_CYCLES_FAST_READ 8U

/**
 * @brief  W25Qxx Registers
 */
/* Status Register */
#define W25X_SR_WIP (0x01)  /*!< Write in progress */
#define W25X_SR_WREN (0x02) /*!< Write enable latch */

static volatile int qspi_mode = 0;

static HAL_StatusTypeDef w25qxx_reset(QSPI_HandleTypeDef *hqspi)
{
    QSPI_CommandTypeDef cmd = {0};

    /* Initialize the reset enable command */
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = W25X_EnableReset;
    cmd.AddressMode = QSPI_ADDRESS_NONE;
    cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    cmd.DataMode = QSPI_DATA_NONE;
    cmd.DummyCycles = 0;
    cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
    cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    cmd.Instruction = W25X_EnableReset;
    if (HAL_QSPI_Command(hqspi, &cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return HAL_ERROR;
    }

    cmd.Instruction = W25X_ResetDevice;
    if (HAL_QSPI_Command(hqspi, &cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return HAL_ERROR;
    }

    cmd.InstructionMode = QSPI_INSTRUCTION_4_LINES;
    cmd.Instruction = W25X_EnableReset;
    if (HAL_QSPI_Command(hqspi, &cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return HAL_ERROR;
    }

    cmd.Instruction = W25X_ResetDevice;
    if (HAL_QSPI_Command(hqspi, &cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}

static HAL_StatusTypeDef w25qxx_send_cmd(QSPI_HandleTypeDef *hqspi, uint32_t instruction, uint32_t address, uint32_t addressSize,
                                         uint32_t dummyCycles, uint32_t instructionMode, uint32_t addressMode, uint32_t dataMode,
                                         uint32_t dataSize)
{
    QSPI_CommandTypeDef cmd = {0};

    cmd.Instruction = instruction;
    cmd.InstructionMode = instructionMode;
    cmd.Address = address;
    cmd.AddressSize = addressSize;
    cmd.AddressMode = addressMode;
    cmd.AlternateBytes = 0x00;
    cmd.AlternateBytesSize = QSPI_ALTERNATE_BYTES_8_BITS;
    cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    cmd.DummyCycles = dummyCycles;
    cmd.DataMode = dataMode;
    cmd.NbData = dataSize;
    cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
    cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    return HAL_QSPI_Command(hqspi, &cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
}

uint16_t w25qxx_get_id(void)
{
    uint8_t id[6];

    w25qxx_send_cmd(&hqspi, W25X_QUAD_ManufactDeviceID, 0x00, QSPI_ADDRESS_24_BITS, 6, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_4_LINES, QSPI_DATA_4_LINES, sizeof(id));
    HAL_QSPI_Receive(&hqspi, id, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);

    return (id[0] << 8) | id[1];
}

uint8_t w25qxx_read_sr(uint8_t addr)
{
    uint8_t byte = 0;

    w25qxx_send_cmd(&hqspi, addr, 0x00, QSPI_ADDRESS_8_BITS, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE, QSPI_DATA_1_LINE, 1);
    HAL_QSPI_Receive(&hqspi, &byte, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);

    return byte;
}

uint8_t w25qxx_write_sr(uint8_t addr, uint8_t data)
{
    w25qxx_send_cmd(&hqspi, addr, 0x00, QSPI_ADDRESS_8_BITS, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE, QSPI_DATA_1_LINE, 1);

    return HAL_QSPI_Transmit(&hqspi, &data, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
}

static uint32_t w25qxx_write_enable(void)
{
    QSPI_CommandTypeDef cmd = {0};
    QSPI_AutoPollingTypeDef cfg = {0};
    HAL_StatusTypeDef ret = HAL_OK;

    /* Enable write operations ------------------------------------------ */
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = W25X_WriteEnable;
    cmd.AddressMode = QSPI_ADDRESS_NONE;
    cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    cmd.DataMode = QSPI_DATA_NONE;
    cmd.DummyCycles = 0;
    cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
    cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    ret = HAL_QSPI_Command(&hqspi, &cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    if (ret != HAL_OK)
    {
        return ret;
    }

    /* Configure automatic polling mode to wait for write enabling ---- */
    cfg.Match = 0x02;
    cfg.Mask = 0x02;
    cfg.MatchMode = QSPI_MATCH_MODE_AND;
    cfg.StatusBytesSize = 1;
    cfg.Interval = 0x10;
    cfg.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;
    cmd.Instruction = W25X_ReadStatusReg1;
    cmd.DataMode = QSPI_DATA_1_LINE;

    return HAL_QSPI_AutoPolling(&hqspi, &cmd, &cfg, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
}

void w25qxx_wait_free(void)
{
    while ((w25qxx_read_sr(W25X_ReadStatusReg1) & 0x01) == 0x01)
    {
    }
}

static uint32_t w25qxx_auto_polling_memory_ready(QSPI_HandleTypeDef *hqspi, uint32_t timeout)
{
    QSPI_CommandTypeDef cmd;
    QSPI_AutoPollingTypeDef cfg;

    /* Configure automatic polling mode to wait for memory ready */
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = W25X_ReadStatusReg1;
    cmd.AddressMode = QSPI_ADDRESS_NONE;
    cmd.Address = 0x00;
    cmd.AddressSize = QSPI_ADDRESS_8_BITS;
    cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    cmd.DataMode = QSPI_DATA_1_LINE;
    cmd.DummyCycles = 0;
    cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
    cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    cfg.Match = 0;
    cfg.Mask = W25X_SR_WIP;
    cfg.MatchMode = QSPI_MATCH_MODE_AND;
    cfg.Interval = 0x10;
    cfg.AutomaticStop = QSPI_AUTOMATIC_STOP_ENABLE;
    cfg.StatusBytesSize = 1;

    return HAL_QSPI_AutoPolling(hqspi, &cmd, &cfg, timeout);
}

static void w25qxx_enter_qspi(void)
{
    uint8_t data = (8 / 2 - 1) << 4 | ((8 / 8 - 1) & 0x03);
    uint8_t ret = w25qxx_read_sr(W25X_ReadStatusReg2);

    w25qxx_write_enable();
    ret |= 0x2;
    w25qxx_write_sr(W25X_WriteStatusReg2, ret);
    w25qxx_send_cmd(&hqspi, W25X_EnterQSPIMode, 0x00, QSPI_ADDRESS_8_BITS, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE, QSPI_DATA_NONE, 0);
    /* Configure automatic polling mode to wait the memory is ready */
    w25qxx_auto_polling_memory_ready(&hqspi, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    /* Set read parameters */
    w25qxx_write_enable();
    w25qxx_send_cmd(&hqspi, W25X_SetReadParam, 0x00, QSPI_ADDRESS_8_BITS, 0, QSPI_INSTRUCTION_4_LINES, QSPI_ADDRESS_NONE, QSPI_DATA_4_LINES, 1);
    HAL_QSPI_Transmit(&hqspi, &data, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
}

HAL_StatusTypeDef w25qxx_enter_memory_mapped_mode(void)
{
    QSPI_CommandTypeDef cmd = {0};
    QSPI_MemoryMappedTypeDef cfg = {0};

    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = W25X_QUAD_INOUT_FAST_READ_CMD;
    cmd.AddressSize = QSPI_ADDRESS_24_BITS;
    cmd.AddressMode = QSPI_ADDRESS_4_LINES;
    cmd.Address = 0;
    cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_4_LINES;
    cmd.AlternateBytes = 0xFF;
    cmd.AlternateBytesSize = 1;
    cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
    cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;
    cmd.DataMode = QSPI_DATA_4_LINES;
    cmd.NbData = 0;
    cmd.DummyCycles = 4;
    cfg.TimeOutActivation = QSPI_TIMEOUT_COUNTER_DISABLE;
    cfg.TimeOutPeriod = 0;

    return HAL_QSPI_MemoryMapped(&hqspi, &cmd, &cfg);
}

HAL_StatusTypeDef w25qxx_exit_memory_mapped_mode(void)
{
    return HAL_QSPI_Abort(&hqspi);
}

void w25qxx_init(void)
{
    w25qxx_reset(&hqspi);
    w25qxx_get_id();
    w25qxx_enter_qspi();
}

HAL_StatusTypeDef w25qxx_erase_sector(uint32_t SectorAddress)
{
    HAL_StatusTypeDef ret = HAL_OK;

    w25qxx_write_enable();
    ret = w25qxx_send_cmd(&hqspi, W25X_SectorErase, SectorAddress, QSPI_ADDRESS_24_BITS, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_1_LINE, QSPI_DATA_NONE, 0);
    w25qxx_wait_free();

    return ret;
}

HAL_StatusTypeDef w25qxx_erase_block(uint32_t BlockAddress)
{
    HAL_StatusTypeDef ret = HAL_OK;

    w25qxx_write_enable();
    ret = w25qxx_send_cmd(&hqspi, W25X_BlockErase, BlockAddress, QSPI_ADDRESS_24_BITS, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_1_LINE, QSPI_DATA_NONE, 0);
    w25qxx_wait_free();

    return ret;
}

HAL_StatusTypeDef w25qxx_erase_chip(void)
{
    HAL_StatusTypeDef ret = HAL_OK;

    w25qxx_write_enable();
    ret = w25qxx_send_cmd(&hqspi, W25X_ChipErase, 0x00, QSPI_ADDRESS_8_BITS, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_NONE, QSPI_DATA_NONE, 0);
    w25qxx_wait_free();

    return ret;
}

HAL_StatusTypeDef w25qxx_program_page(uint8_t *pData, uint32_t WriteAddr, uint32_t Size)
{
    HAL_StatusTypeDef ret = HAL_OK;

    w25qxx_write_enable();
    ret = w25qxx_send_cmd(&hqspi, W25X_QUAD_INPUT_PAGE_PROG_CMD, WriteAddr, QSPI_ADDRESS_24_BITS, 0, QSPI_INSTRUCTION_1_LINE, QSPI_ADDRESS_1_LINE, QSPI_DATA_4_LINES, Size);
    HAL_QSPI_Transmit(&hqspi, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE);
    w25qxx_wait_free();

    return ret;
}

HAL_StatusTypeDef w25qxx_read(uint8_t *pData, uint32_t ReadAddr, uint32_t Size)
{
    HAL_StatusTypeDef ret = HAL_OK;
    QSPI_CommandTypeDef cmd = {0};

    /* Initialize the read command */
    cmd.InstructionMode = QSPI_INSTRUCTION_1_LINE;
    cmd.Instruction = W25X_QUAD_INOUT_FAST_READ_CMD;
    cmd.AddressMode = QSPI_ADDRESS_4_LINES;
    cmd.AddressSize = QSPI_ADDRESS_24_BITS;
    cmd.Address = ReadAddr;
    cmd.AlternateByteMode = QSPI_ALTERNATE_BYTES_NONE;
    cmd.DataMode = QSPI_DATA_4_LINES;
    cmd.DummyCycles = 6U;
    cmd.NbData = Size;
    cmd.DdrMode = QSPI_DDR_MODE_DISABLE;
    cmd.DdrHoldHalfCycle = QSPI_DDR_HHC_ANALOG_DELAY;
    cmd.SIOOMode = QSPI_SIOO_INST_EVERY_CMD;

    /* Configure the command */
    if (HAL_QSPI_Command(&hqspi, &cmd, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return HAL_ERROR;
    }

    /* Set S# timing for Read command */
    MODIFY_REG(hqspi.Instance->DCR, QUADSPI_DCR_CSHT,
               QSPI_CS_HIGH_TIME_5_CYCLE);

    /* Reception of the data */
    if (HAL_QSPI_Receive(&hqspi, pData, HAL_QPSI_TIMEOUT_DEFAULT_VALUE) != HAL_OK)
    {
        return HAL_ERROR;
    }

    /* Restore S# timing for nonRead commands */
    MODIFY_REG(hqspi.Instance->DCR, QUADSPI_DCR_CSHT,
               QSPI_CS_HIGH_TIME_6_CYCLE);

    return ret;
}

HAL_StatusTypeDef w25qxx_write(uint8_t *pBuffer, uint32_t WriteAddr, uint32_t NumByteToWrite)
{
    uint16_t pageremain = MEMORY_PAGE_SIZE - WriteAddr % MEMORY_PAGE_SIZE;

    if (NumByteToWrite <= pageremain)
    {
        pageremain = NumByteToWrite;
    }
    while (1)
    {
        if (HAL_OK != w25qxx_program_page(pBuffer, WriteAddr, pageremain))
        {
            return HAL_ERROR;
        }

        if (NumByteToWrite == pageremain)
        {
            break;
        }
        // NumByteToWrite>pageremain
        else
        {
            pBuffer += pageremain;
            WriteAddr += pageremain;
            NumByteToWrite -= pageremain;
            if (NumByteToWrite > MEMORY_PAGE_SIZE)
                pageremain = MEMORY_PAGE_SIZE;
            else
                pageremain = NumByteToWrite;
        }
    }

    return HAL_OK;
}
