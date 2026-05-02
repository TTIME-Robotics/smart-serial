/**
 * @file stm32_hal_clock
 * @brief Contains implementations for clock specialisation for HAL (no RTOS)
 * @author Daniel Dew
 * @version 0.1
 * @date 30.04.26
 */

#include "smart-serial/clock/stm32_hal_clock.hpp"
#include "main.h"

using namespace Smart_serial;

uint32_t Clock::STM32_hal_clock::millis() const {
	return HAL_GetTick();
}

void Clock::STM32_hal_clock::delay(const uint32_t delay_time) const {
	HAL_Delay(delay_time);
}
