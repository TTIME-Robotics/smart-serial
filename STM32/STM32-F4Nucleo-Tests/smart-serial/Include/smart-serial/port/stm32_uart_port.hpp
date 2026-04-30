/**
 * @file stm32_uart_port.hpp
 * @brief Contains header files for the port interface specialisation (for STM32 Uart)
 * @author Daniel Dew
 * @version 0.1
 * @date 30.04.26
 */

#ifndef SMART_SERIAL_STM32_UART_PORT_HPP
#define SMART_SERIAL_STM32_UART_PORT_HPP

#include "smart-serial/api.hpp"
#include "smart-serial/port/iport.hpp"

namespace Smart_serial {

	class STM32_uart_port : public I_port {

		public:
			/**
			 * @brief Construct a uart port
			 * @param huart pointer to the HAL uart handler
			 * @param de_port pointer to the drive enable GPIO handler
			 * @param de_pin pin number for drive enable
			 */
			STM32_uart_port(UART_HandleTypeDef* huart,
							GPIO_TypeDef* de_port,
							uint16_t* de_pin);
			/**
			 * @brief trivial destructor
			 */
			~STM32_uart_port();

			/**
			 * @brief Write a byte buffer to the uart port
			 * @param buf The buffer of bytes to send
			 * @param len the length of byte buffer to write
			 * @return int32_t number of bytes written, or error code
			 */
			int32_t write(const uint8_t* buf, const uint16_t len) override;

			/**
			 * @brief read a single byte from rx buffer
			 * @return 1 if successful, error code if not
			 */
			int32_t read_byte() override;

			/**
			 * @brief flush the rx buffer
			 */
			void flush_rx() override;
		private:

			UART_HandleTypeDef* huart_;
			GPIO_TypeDef* de_port_;
			uint16_t de_pin_;

			/**
			 * @brief drive the DE pin high to signal a write
			 */
			void assert_de();
			/**
			 * @brief release the DE pin to low to wait for transmission
			 */
			void release_de();

			STM32_uart_port(STM32_uart_port&) = delete;
			STM32_uart_port& operator=(STM32_uart_port&) = delete;

	};

}

#endif // SMART_SERIAL_STM32_UART_PORT_HPP
