# stm32-dma-uart-echo

Simple STM32CubeIDE project demonstrating UART communication using DMA and FreeRTOS on STM32F103C8T6. Implements an **echo functionality**: whatever you send to the UART is echoed back automatically.

> ⚠️ Note: This is a beginner-friendly example. Code may contain bugs and is **not production-ready**.

<img width="1258" height="663" alt="image" src="https://github.com/user-attachments/assets/ed554b99-d341-4259-81cb-c5e114d20eb6" />

---

## Features

- STM32CubeIDE project structure
- UART receive/transmit using DMA
- FreeRTOS integration (tasks, scheduling)
- Circular buffer for RX/TX
- Handles data asynchronously with DMA and ring buffers
- Simple echo example

---

## Project Structure
- /Core
- /Inc # Header files
- /Src # Source files
- /Drivers # HAL and CMSIS drivers
- /Middlewares # Optional, e.g. FreeRTOS
- *.ioc # CubeMX configuration file


### Key files:

- `usart.c` / `usart.h` — DMA RX/TX functions
- `uart_rings.c` / `uart_rings.h` - UART DMA RX/TX rings managing
- `main.c` — FreeRTOS task `StartDefaultTask` handling echo
- `stm32xxxx_hal_conf.h` — HAL configuration

---

## How to Use

1. Clone the repository:

```bash
git clone <your_repo_url>
```

2. Open the .ioc file in STM32CubeIDE.

3. Build and flash the project to your STM32 MCU.

4. Open a UART terminal (19200 baud by default) and type characters. They should be echoed back.

## Notes

- RX/TX DMA ring buffers ensure asynchronous handling of UART data.

- FreeRTOS task reads received data from RX buffer and queues it for TX.

- Small delays are added in the main task loop to prevent CPU hogging.

- Can receive messages larger than the buffer while the main thread is reading the receive buffer.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
