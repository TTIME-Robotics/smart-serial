/**
 * @file stm32_rtos_clock.hpp
 * @brief Contains definitions for an RTOS OS specialisation of clock interface
 * @author Daniel Dew
 * @version 0.1
 * @date 03.05.26
 */

#ifndef SMART_SERIAL_STM32_RTOS_CLOCK_HPP
#define SMART_SERIAL_STM32_RTOS_CLOCK_HPP

#include "smart-serial/clock/IClock.hpp"

namespace Smart_serial::Clock {

	class STM32_rtos_clock : public I_clock {
		public:
			/** @brief construct the clock */
			STM32_rtos_clock() {}

			/**
			 * @brief Get current tick count from RTOS kernel
			 * @return uint32_t tick count
			 */
			uint32_t millis() const override;

			/**
			 * @brief Delay task for time (non blocking)
			 * @param delay_time time in ms to delay
			 */
			void delay(const uint32_t delay_time) const override;
	};
}

#endif // SMART_SERIAL_STM32_RTOS_CLOCK_HPP
