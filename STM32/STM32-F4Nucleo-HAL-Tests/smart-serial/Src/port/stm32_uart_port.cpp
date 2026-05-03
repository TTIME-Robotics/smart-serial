/**
 * @file stm32_uart_port.cpp
 * @brief Contains implementations of the STM32 uart port
 * @author Daniel Dew
 * @version 0.1
 * @date 30.04.26
 */

#include "smart-serial/port/stm32_uart_port.hpp"
#include "main.h"
#include <cstring>
using namespace Smart_serial;

// Registry definition
STM32_uart_port* Smart_serial::g_port_registry[MAX_UART_PORTS] = {};

// Helper to get registry index from UART instance
static int8_t get_uart_index(const USART_TypeDef* const instance)
{
    int8_t index = -1;
    if      (instance == USART1) { index = 0; }
    else if (instance == USART2) { index = 1; }
	#if defined(USART3)
		else if (instance == USART3) { index = 2; }
	#endif
	#if defined(UART4)
		else if (instance == UART4)  { index = 3; }
	#endif
	#if defined(UART5)
		else if (instance == UART5)  { index = 4; }
	#endif
	#if defined(USART6)
		else if (instance == USART6) { index = 5; }
	#endif
	#if defined(UART7)
		else if (instance == UART7)  { index = 6; }
	#endif
	#if defined(UART8)
		else if (instance == UART8)  { index = 7; }
	#endif
    return index;
}

// Initialise object's attributes
STM32_uart_port::STM32_uart_port(UART_HandleTypeDef* huart,
								GPIO_TypeDef* de_port,
								uint16_t de_pin)
	: huart_(huart), de_port_(de_port), de_pin_(de_pin), rx_head_(0U) , rx_tail_(0U)
{

	// Initialsie ring buffer
	memset(const_cast<uint8_t*>(rx_ring_), 0U, sizeof(rx_ring_));

	// Register this instance by UART index
	const int8_t index =
		get_uart_index(huart_->Instance);

	if (index >= 0)
	{
		g_port_registry[
			static_cast<uint8_t>(index)] = this;
	}

	// Enable UART interrupt on peripheral level (instead of MX)
	__HAL_UART_ENABLE_IT(huart_, UART_IT_RXNE);

	IRQn_Type irqn = USART1_IRQn;
	if      (huart_->Instance == USART1) { irqn = USART1_IRQn; }
	else if (huart_->Instance == USART2) { irqn = USART2_IRQn; }
	#if defined(USART3)
	else if (huart_->Instance == USART3) { irqn = USART3_IRQn; }
	#endif
	#if defined(UART4)
	else if (huart_->Instance == UART4)  { irqn = UART4_IRQn;  }
	#endif
	#if defined(UART5)
	else if (huart_->Instance == UART5)  { irqn = UART5_IRQn;  }
	#endif
	#if defined(USART6)
	else if (huart_->Instance == USART6) { irqn = USART6_IRQn; }
	#endif
	#if defined(UART7)
	else if (huart_->Instance == UART7)  { irqn = UART7_IRQn;  }
	#endif
	#if defined(UART8)
	else if (huart_->Instance == UART8)  { irqn = UART8_IRQn;  }
	#endif

	// Configure and enable NVIC:
	HAL_NVIC_SetPriority(irqn, 5U, 0U);
	HAL_NVIC_EnableIRQ(irqn);

	// Port is open

}

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
    if (rx_tail_ == rx_head_)
    {
        return S_SERIAL_ERR; // buffer empty
    }

    const uint8_t  byte     = rx_ring_[rx_tail_];
    const uint16_t next_tail =
        static_cast<uint16_t>((rx_tail_ + 1U) % RING_BUF_SIZE);

    rx_tail_ = next_tail;

    return static_cast<int32_t>(byte);
}

void STM32_uart_port::flush_rx() {
	for (uint16_t i=0; i<MAX_FLUSH_ITERS; ++i) {
		if (read_byte() < 0) {
			break; // No more bytes to read
		}
	}
}

void STM32_uart_port::rx_isr()
{
    // Clear overrun if set — must be done before reading DR
    if ((huart_->Instance->SR & USART_SR_ORE) != 0U)
    {
        static_cast<void>(huart_->Instance->DR);
        return;
    }

    if ((huart_->Instance->SR & USART_SR_RXNE) != 0U)
    {
        const uint8_t byte =
            static_cast<uint8_t>(huart_->Instance->DR & 0xFFU);

        const uint16_t next_head =
            static_cast<uint16_t>((rx_head_ + 1U) % RING_BUF_SIZE);

        if (next_head != rx_tail_) // not full — drop byte if full
        {
            rx_ring_[rx_head_] = byte;
            rx_head_           = next_head;
        }
    }
}

void STM32_uart_port::assert_de() {
	HAL_GPIO_WritePin(de_port_, de_pin_, GPIO_PIN_SET);
}
void STM32_uart_port::release_de() {
	HAL_GPIO_WritePin(de_port_, de_pin_, GPIO_PIN_RESET);
}

// ISR handlers — each dispatches to the correct registry slot
extern "C" void USART1_IRQHandler(void)
{

    if (g_port_registry[0U] != NULL) { g_port_registry[0U]->rx_isr(); }
}

extern "C" void USART2_IRQHandler(void)
{

    if (g_port_registry[1U] != NULL) { g_port_registry[1U]->rx_isr(); }
}

#if defined(USART3)
extern "C" void USART3_IRQHandler(void)
{

    if (g_port_registry[2U] != NULL) { g_port_registry[2U]->rx_isr(); }
}
#endif

#if defined(UART4)
extern "C" void UART4_IRQHandler(void)
{

    if (g_port_registry[3U] != NULL) { g_port_registry[3U]->rx_isr(); }
}
#endif

#if defined(UART5)
extern "C" void UART5_IRQHandler(void)
{

    if (g_port_registry[4U] != NULL) { g_port_registry[4U]->rx_isr(); }
}
#endif

#if defined(USART6)
extern "C" void USART6_IRQHandler(void)
{

    if (g_port_registry[5U] != NULL) { g_port_registry[5U]->rx_isr(); }
}
#endif

#if defined(UART7)
extern "C" void UART7_IRQHandler(void)
{

    if (g_port_registry[6U] != NULL) { g_port_registry[6U]->rx_isr(); }
}
#endif

#if defined(UART8)
extern "C" void UART8_IRQHandler(void)
{

    if (g_port_registry[7U] != NULL) { g_port_registry[7U]->rx_isr(); }
}
#endif
