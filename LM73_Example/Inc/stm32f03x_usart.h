#ifndef USART_H
#define USART_H
// ---------------------------------------------------------------------------
/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif
// ---------------------------------------------------------------------------
#include "stm32f0xx.h"
#include "stm32f03x_defs.h"
// ---------------------------------------------------------------------------
#define USART_BUFFER_SIZE      64

/* NVIC Global Priority */
#ifndef USART_NVIC_PRIORITY
#define USART_NVIC_PRIORITY    0x06
#endif
// ---------------------------------------------------------------------------
//Internal USART struct
typedef struct {
  uint16_t Num;
  uint16_t In;
  uint16_t Out;
  uint16_t Size;
  uint8_t* Buffer;
  uint8_t  Initialized;
} USART_t;
// ---------------------------------------------------------------------------
/**
 * @brief  Initializes USARTx peripheral and corresponding pins
 * @param  *USARTx: Pointer to USARTx peripheral you will use
 * @param  pinspack: This parameter can be a value of @ref USART_PinsPack_t enumeration
 * @param  baudrate: Baudrate number for USART communication
 * @retval None
 */
void USART_Init(uint32_t baudrate);

/**
 * @brief  Sends character to USART port
 * @param  c: character to be send over USART
 * @retval None
 */
void USART_SendChar(char c);

/**
 * @brief  Sends string to USART port
 * @param  *str: Pointer to string to send over USART
 * @retval None
 */
void USART_SendString(char* str);

/**
 * @brief  Sends data array to USART port
 * @param  *USARTx: Pointer to USARTx peripheral you will use
 * @param  *DataArray: Pointer to data array to be sent over USART
 * @param  count: Number of elements in data array to be send over USART
 * @retval None
 */
void USART_SendBytes(uint8_t* DataArray, uint16_t count);

/**
 * @brief  Sends data array as hex string bytes to USART port
 * @param  *USARTx: Pointer to USARTx peripheral you will use
 * @param  *DataArray: Pointer to data array to be sent over USART
 * @param  count: Number of elements in data array to be send over USART
 * @param  separator: Separator for hex bytes. If set zero then bytes is not separated
 * @retval None
 */
void USART_SendBytesAsHex(uint8_t* DataArray, uint16_t count, char separator);

/**
 * @brief  Gets character from internal USART buffer
 * @param  *USARTx: Pointer to USARTx peripheral you will use
 * @retval Character from buffer, or 0 if nothing in buffer
 */
uint8_t USART_GetChar(void);

/**
 * @brief  Gets string from USART
 * @param  *buffer: Pointer to buffer where data will be stored from buffer
 * @param  bufsize: maximal number of characters we can add to your buffer, including leading zero
 * @retval Number of characters in buffer
 */
uint16_t USART_GetString(char* buffer, uint16_t bufsize);

/**
 * @brief  Checks if character c is available in internal buffer
 * @param  *USARTx: Pointer to USARTx peripheral you will use
 * @param  c: character to check if it is in USARTx's buffer
 * @retval Character status:
 *            - 0: Character was not found
 *            - > 0: Character has been found in buffer
 */
uint8_t USART_FindCharacter(uint8_t c);

/**
 * @brief  Checks if internal USARTx buffer is empty
 * @param  *USARTx: Pointer to USARTx peripheral you will use
 * @retval Buffer empty status:
 *            - 0: Buffer is not empty
 *            - > 0: Buffer is empty
 */
uint8_t USART_BufferEmpty(void);

/**
 * @brief  Checks if internal USARTx buffer is full
 * @param  *USARTx: Pointer to USARTx peripheral you will use
 * @retval Buffer full status:
 *            - 0: Buffer is not full
 *            - > 0: Buffer is full
 */
uint8_t USART_BufferFull(void);

/**
 * @brief  Clears internal USART buffer
 * @param  *USARTx: Pointer to USARTx peripheral you will use
 * @retval None
 */
void USART_ClearBuffer(void);
// ---------------------------------------------------------------------------
/* C++ detection */
#ifdef __cplusplus
}
#endif
// ---------------------------------------------------------------------------
#endif
