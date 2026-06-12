// =====================================================
// lora_e220.h
// ACP E220 DRIVER
// E220-900T22D
// STM32 HAL LIBRARY
// =====================================================

#ifndef LORA_E220_H
#define LORA_E220_H

#include "main.h"

#include <stdint.h>
#include <string.h>

// =====================================================
// COMMANDS
// =====================================================

#define CMD_WRITE_CFG_PWR_DWN_SAVE   0xC0
#define CMD_READ_CFG                 0xC1
#define CMD_WRITE_CFG_PWR_DWN_LOSE   0xC2

// =====================================================
// REGISTERS
// =====================================================

#define REG_ADDH     0x00
#define REG_ADDL     0x01
#define REG_NETID    0x02
#define REG_REG0     0x03
#define REG_REG1     0x04
#define REG_REG2     0x05
#define REG_REG3     0x06
#define REG_CRYPT_H  0x07
#define REG_CRYPT_L  0x08

// =====================================================
// MODES
// =====================================================

#define LORA_MODE_NORMAL      0
#define LORA_MODE_WAKEUP      1
#define LORA_MODE_POWERSAVE   2
#define LORA_MODE_SLEEP       3

// =====================================================
// STATUS
// =====================================================

typedef enum
{
    LORA_OK = 0,
    LORA_ERROR,
    LORA_TIMEOUT

}LORA_Status;

// =====================================================
// HANDLE
// =====================================================

typedef struct
{
    UART_HandleTypeDef* huart;

    GPIO_TypeDef* M0_PORT;
    uint16_t M0_PIN;

    GPIO_TypeDef* M1_PORT;
    uint16_t M1_PIN;

    GPIO_TypeDef* AUX_PORT;
    uint16_t AUX_PIN;

}LORA_E220;

// =====================================================
// API
// =====================================================

void LORA_Init(
    LORA_E220* lora,
    UART_HandleTypeDef* huart,

    GPIO_TypeDef* M0_PORT,
    uint16_t M0_PIN,

    GPIO_TypeDef* M1_PORT,
    uint16_t M1_PIN,

    GPIO_TypeDef* AUX_PORT,
    uint16_t AUX_PIN
);

void LORA_SetMode(
    LORA_E220* lora,
    uint8_t mode
);

uint8_t LORA_IsBusy(
    LORA_E220* lora
);

void LORA_WaitAux(
    LORA_E220* lora
);

// =====================================================
// REGISTER ACCESS
// =====================================================

int8_t LORA_ReadRegister(
    LORA_E220* lora,
    uint8_t reg
);

int8_t LORA_WriteRegister(
    LORA_E220* lora,
    uint8_t reg,
    uint8_t value
);

// =====================================================
// DATA
// =====================================================

LORA_Status LORA_Send(
    LORA_E220* lora,
    uint8_t* data,
    uint16_t size
);

LORA_Status LORA_SendString(
    LORA_E220* lora,
    char* str
);

LORA_Status LORA_Receive(
    LORA_E220* lora,
    uint8_t* buffer,
    uint16_t size,
    uint32_t timeout
);

#endif
