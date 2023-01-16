# sx1280-stm32Lf030
开发环境： IAR for ARM

作用：用于RSSI的获取，需要两个MCU，一收一发

main函数中第49行isMaster为主机从机变量，true为发送，false为接收

143~145行选择调制模式 可支持FLRC、GFSK、LORA