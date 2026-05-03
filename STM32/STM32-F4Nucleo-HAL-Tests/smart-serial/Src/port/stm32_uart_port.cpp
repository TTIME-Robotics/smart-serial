/**
 * @file stm32_uart_port.cpp
 * @brief Contains implementations of the STM32 uart port
 * @author Daniel Dew
 * @version 0.1
 * @date 30.04.26
 */

#include "smart-serial/port/stm32_uart_port.hpp"
#include "main.h"

using namespace Smart_serial;

// Initialise object's attributes
STM32_uart_port::STM32_uart_port(UART_HandleTypeDef* huart,
								GPIO_TypeDef* de_port,
								uint16_t de_pin)
	: huart_(huart), de_port_(de_port), de_pin_(de_pin) {}

STM32_uart_port::~STM32_uart_port() {} // Trivial destructor

int32_t STM32_uart_port::write(const uint8_t* const buf, const uint16_t len) {
	// Assert dominance on RS485 line
	assert_de();
	// Write using HAL
	const HAL_StatusTypeDef status =
			HAL_UART_Transmit(huart_, const_cast<uint8_t*>(buf), len, HAL_MAX_DELAY);
	// Release dominance
	release_de();
	return (status == HAL_OK) ? static_cast<int32_t>(len) : S_SERIAL_ERR;
}

int32_t STM32_uart_port::read_byte()
{
    // Check RXNE flag — receive data register not empty
    if ((huart_->Instance->SR & USART_SR_RXNE) == 0U)
    {
        return S_SERIAL_ERR; // nothing available
    }
    // Read directly from data register — clears RXNE flag automatically
    return static_cast<int32_t>(huart_->Instance->DR & 0xFFU);
}

void STM32_uart_port::flush_rx() {
	for (uint16_t i=0; i<MAX_FLUSH_ITERS; ++i) {
		if (read_byte() < 0) {
			break; // No more bytes to read
		}
	}
}

void STM32_uart_port::assert_de() {
	HAL_GPIO_WritePin(de_port_, de_pin_, GPIO_PIN_SET);
}
void STM32_uart_port::release_de() {
	HAL_GPIO_WritePin(de_port_, de_pin_, GPIO_PIN_RESET);
}
