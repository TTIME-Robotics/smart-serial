/**
 * @file stm32_hal_clock.hpp
 * @brief Contains definitions for specialised clock using the HAL (no RTOS)
 * @author Daniel Dew
 * @version 0.1
 * @date 30.04.26
 */

#ifndef SMART_SERIAL_STM32_HAL_CLOCK_HPP
#define SMART_SERIAL_STM32_HAL_CLOCK_HPP

#include "smart-serial/clock/IClock.hpp"

namespace Smart_serial { namespace Clock {

	class STM32_hal_clock : public I_clock {
		public:
			STM32_hal_clock() {}
			uint32_t millis() const override;

			void delay(const uint32_t delay_time) const override;

	};
}}
#endif // SMART_SERIAL_STM32_HAL_CLOCK_HPP
