# This work is licensed under the MIT license.
# Copyright (c) 2013-2025 OpenMV LLC. All rights reserved.
# https://github.com/openmv/openmv/blob/master/LICENSE
#
# Hello World Example
#
# Welcome to the OpenMV IDE! Click on the green run arrow button below to run the script!

import sensor
import time
from pyb import UART
import struct

class uart_slave:
    START_BYTE = 0xAA
    THIS_ADRESS = 0x04
    MASTER_ADRESS = 0xFE

    SEND_DATA_CMD = 0x98
    STOP_PROCESSING_CMD = 0xD2
    START_PROCESSING_CMD = 0xD1

    def crc16_ccitt(self, data, poly: int = 0x1021, init: int = 0xFFFF) -> int:
        crc = init
        for byte in data:
            crc ^= byte << 8
            for _ in range(8):
                if crc & 0x8000:
                    crc = (crc << 1) ^ poly
                else:
                    crc <<= 1
                crc &= 0xFFFF
        return crc


    def __init__(self, timeout=1000):
        self.uart_port = UART(1)
        self.uart_port.init(115200, bits=8, parity=None, timeout=timeout, read_buf_len=256)

    def verifyCRC16(self, data):
        byte1 = 0x00
        byte2 = 0x00
        if self.uart_port.any() > 0:
            byte1 = self.uart_port.read(1)
        if self.uart_port.any() > 0:
            byte2 = self.uart_port.read(1)
        foundCrc = struct.unpack("<H", byte1 + byte2)[0]
        return self.crc16_ccitt(data) == foundCrc

    ACK_BYTE = 0x06
    NACK_BYTE = 0x15
    def getIsCmd(self, cmd_byte) -> bool:
        if self.uart_port.any() < 7:
            return False
        data = self.uart_port.read(5)
        if data[0] == self.START_BYTE:
            if data[1] == self.THIS_ADRESS and data[2] == self.MASTER_ADRESS:
                isRightCmd = (data[3] == cmd_byte)
                correctCrc = self.verifyCRC16(data)
                if correctCrc:
                    if isRightCmd:
                        return True
                    elif data[3] == self.ACK_BYTE and data[4] == 0 and self.autoHandshake:
                        self.sendPayload("", self.ACK_BYTE)
        return False

    def getSendData(self):
        return self.getIsCmd(self.SEND_DATA_CMD)
    def getStartProcessing(self):
        return self.getIsCmd(self.START_PROCESSING_CMD)
    def getStopProcessing(self):
        return self.getIsCmd(self.STOP_PROCESSING_CMD)

    autoHandshake = True
    def setAutoHandshake(self, shouldShake):
        self.autoHandshake = shouldShake

    def sendPayload(self, payload, cmd_byte):
        # Accept either str or bytes for payload
        if isinstance(payload, str):
            payload = payload.encode()  # str -> bytes
        print(payload)

        dataToSend = bytearray([
            self.START_BYTE,
            self.MASTER_ADRESS,
            self.THIS_ADRESS,
            cmd_byte,
            len(payload),
        ])
        dataToSend.extend(payload)

        crc = self.crc16_ccitt(dataToSend)
        dataToSend.extend(crc.to_bytes(2, 'little'))

        self.uart_port.write(dataToSend)

slave_device = uart_slave()

sensor.reset()  # Reset and initialize the sensor.
sensor.set_pixformat(sensor.RGB565)  # Set pixel format to RGB565 (or GRAYSCALE)
sensor.set_framesize(sensor.QVGA)  # Set frame size to QVGA (320x240)
sensor.skip_frames(time=2000)  # Wait for settings take effect.
clock = time.clock()  # Create a clock object to track the FPS.

keepChecking = True
while True:
    clock.tick()  # Update the FPS clock.
    img = sensor.snapshot()  # Take a picture and return the image.
    if keepChecking:
        if slave_device.getStartProcessing():
            slave_device.sendPayload("Hello World!", 0x06)
            print ("DONE")
            keepChecking = True
        else:
            time.sleep(1)
            print("None")

    # Note: OpenMV Cam runs about half as fast when connected
    # to the IDE. The FPS should increase once disconnected.
