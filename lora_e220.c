// =====================================================
// lora_e220.c
// ACP E220 DRIVER
// E220-900T22D
// STM32 HAL LIBRARY
// =====================================================

#include "lora_e220.h"

// =====================================================
// INIT
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
)
{
    lora->huart = huart;

    lora->M0_PORT = M0_PORT;
    lora->M0_PIN  = M0_PIN;

    lora->M1_PORT = M1_PORT;
    lora->M1_PIN  = M1_PIN;

    lora->AUX_PORT = AUX_PORT;
    lora->AUX_PIN  = AUX_PIN;

    // ============================================
    // NORMAL MODE
    // ============================================

    LORA_SetMode(
        lora,
        LORA_MODE_NORMAL
    );
}

// =====================================================
// MODE
// =====================================================

void LORA_SetMode(
    LORA_E220* lora,
    uint8_t mode
)
{
    switch(mode)
    {
        case LORA_MODE_NORMAL:

            HAL_GPIO_WritePin(
                lora->M0_PORT,
                lora->M0_PIN,
                GPIO_PIN_RESET
            );

            HAL_GPIO_WritePin(
                lora->M1_PORT,
                lora->M1_PIN,
                GPIO_PIN_RESET
            );

        break;

        case LORA_MODE_WAKEUP:

            HAL_GPIO_WritePin(
                lora->M0_PORT,
                lora->M0_PIN,
                GPIO_PIN_SET
            );

            HAL_GPIO_WritePin(
                lora->M1_PORT,
                lora->M1_PIN,
                GPIO_PIN_RESET
            );

        break;

        case LORA_MODE_POWERSAVE:

            HAL_GPIO_WritePin(
                lora->M0_PORT,
                lora->M0_PIN,
                GPIO_PIN_RESET
            );

            HAL_GPIO_WritePin(
                lora->M1_PORT,
                lora->M1_PIN,
                GPIO_PIN_SET
            );

        break;

        case LORA_MODE_SLEEP:

            HAL_GPIO_WritePin(
                lora->M0_PORT,
                lora->M0_PIN,
                GPIO_PIN_SET
            );

            HAL_GPIO_WritePin(
                lora->M1_PORT,
                lora->M1_PIN,
                GPIO_PIN_SET
            );

        break;
    }

    HAL_Delay(5);

    LORA_WaitAux(lora);
}

// =====================================================
// AUX
// =====================================================

uint8_t LORA_IsBusy(
    LORA_E220* lora
)
{
    return !HAL_GPIO_ReadPin(
        lora->AUX_PORT,
        lora->AUX_PIN
    );
}

void LORA_WaitAux(
    LORA_E220* lora
)
{
    uint32_t timeout = HAL_GetTick();

    while(
        LORA_IsBusy(lora)
    )
    {
        if(
            HAL_GetTick() - timeout > 1000
        )
        {
            break;
        }
    }
}

// =====================================================
// READ REGISTER
// =====================================================

int8_t LORA_ReadRegister(
    LORA_E220* lora,
    uint8_t reg
)
{
    uint8_t tx[3] =
    {
        CMD_READ_CFG,
        reg,
        0x01
    };

    uint8_t rx[4] = {0};

    // ============================================
    // CONFIG MODE
    // ============================================

    LORA_SetMode(
        lora,
        LORA_MODE_SLEEP
    );

    HAL_UART_Transmit(
        lora->huart,
        tx,
        3,
        100
    );

    LORA_WaitAux(lora);

    HAL_UART_Receive(
        lora->huart,
        rx,
        4,
        100
    );

    // ============================================
    // BACK NORMAL
    // ============================================

    LORA_SetMode(
        lora,
        LORA_MODE_NORMAL
    );

    if(
        rx[0] == CMD_READ_CFG &&
        rx[1] == reg &&
        rx[2] == 0x01
    )
    {
        return rx[3];
    }

    return -1;
}

// =====================================================
// WRITE REGISTER
// =====================================================

int8_t LORA_WriteRegister(
    LORA_E220* lora,
    uint8_t reg,
    uint8_t value
)
{
    uint8_t tx[4] =
    {
        CMD_WRITE_CFG_PWR_DWN_SAVE,
        reg,
        0x01,
        value
    };

    uint8_t rx[4] = {0};

    // ============================================
    // CONFIG MODE
    // ============================================

    LORA_SetMode(
        lora,
        LORA_MODE_SLEEP
    );

    HAL_UART_Transmit(
        lora->huart,
        tx,
        4,
        100
    );

    LORA_WaitAux(lora);

    HAL_UART_Receive(
        lora->huart,
        rx,
        4,
        100
    );

    // ============================================
    // BACK NORMAL
    // ============================================

    LORA_SetMode(
        lora,
        LORA_MODE_NORMAL
    );

    if(
        rx[0] == CMD_WRITE_CFG_PWR_DWN_SAVE &&
        rx[1] == reg &&
        rx[2] == 0x01 &&
        rx[3] == value
    )
    {
        return value;
    }

    return -1;
}

// =====================================================
// SEND RAW
// =====================================================

LORA_Status LORA_Send(
    LORA_E220* lora,
    uint8_t* data,
    uint16_t size
)
{
    LORA_WaitAux(lora);

    if(
        HAL_UART_Transmit(
            lora->huart,
            data,
            size,
            1000
        ) != HAL_OK
    )
    {
        return LORA_ERROR;
    }

    LORA_WaitAux(lora);

    return LORA_OK;
}

// =====================================================
// SEND STRING
// =====================================================

LORA_Status LORA_SendString(
    LORA_E220* lora,
    char* str
)
{
    return LORA_Send(
        lora,
        (uint8_t*)str,
        strlen(str)
    );
}

// =====================================================
// RECEIVE
// =====================================================

LORA_Status LORA_Receive(
    LORA_E220* lora,
    uint8_t* buffer,
    uint16_t size,
    uint32_t timeout
)
{
    if(
        HAL_UART_Receive(
            lora->huart,
            buffer,
            size,
            timeout
        ) != HAL_OK
    )
    {
        return LORA_TIMEOUT;
    }

    return LORA_OK;
}
