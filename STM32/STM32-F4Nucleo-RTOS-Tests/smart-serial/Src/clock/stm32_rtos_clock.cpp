/**
 * @file stm32_rtos_clock.cpp
 * @brief Contains implementations for an RTOS OS specialisation of clock interface
 * @author Daniel Dew
 * @version 0.1
 * @date 03.05.26
 */

#include "smart-serial/clock/stm32_rtos_clock.hpp"
#include "cmsis_os.h"

using namespace Smart_serial;

uint32_t Clock::STM32_rtos_clock::millis() const {
	return (osKernelGetTickCount() * 1000U) / osKernelGetTickFreq();
}

void Clock::STM32_rtos_clock::delay(const uint32_t delay_time) const {
	osDelay(delay_time);
}
