# Audit an FT232RL (YP-05) USB-to-UART Adapter and Other Serial UART Interfaces (Windows)
This program uses the C programming language and the Windows.h library to identify a UART Interface and have it send itself the message "Hello world!" in order to make sure that it works properly.

## Installation and Usage
1) Download Serial_Audit.c into any folder.
2) Plug your FT232RL (or other USB-to-UART adapter) into a USB port. Wait for Windows to install the driver.
3) Identify the assigned port in Device Manager -> Ports (COM & LPT)
4) Reset serial_port_label on line 24 of Serial_Audit.c to the port that your USB-to-UART adapter is connected to.
5) Close any tools that may be using the port.
6) Connect the Receive and Transmit pins of the UART Interface directly to one another (TXD -> RXD). Do not connect any other pins.
7) Compile and run Serial_Audit.c on Windows.
8) The pipeline of information is sure to work if the program reports an echo of the text message.
