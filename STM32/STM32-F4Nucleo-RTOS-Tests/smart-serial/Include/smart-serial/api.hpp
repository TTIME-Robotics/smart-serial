/**
 * @file api.hpp
 * @brief Contains all header files needed to use smart serial (STM32)
 * @Author Daniel Dew
 * @version 0.1
 * @date 29.04.2026
 */

#ifndef SMART_SERIAL_API_HPP
#define SMART_SERIAL_API_HPP

#include "smart-serial/slave.hpp"
#include "smart-serial/master.hpp"
#include "smart-serial/error.h"
#include "smart-serial/types.h"
#include "smart-serial/frame.hpp"
#include "smart-serial/port/stm32_uart_port.hpp"

#ifdef SMART_SERIAL_STM32_RTOS_CLK

#include "smart-serial/clock/stm32_rtos_clock.hpp"

#else

#include "smart-serial/clock/stm32_hal_clock.hpp"

#endif


#endif // SMART_SERIAL_API_HPP
