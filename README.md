# TM4C123 IR Remote Recorder & Transmitter

An embedded systems project that turns the **Texas Instruments TM4C123GH6PM** microcontroller into a programmable infrared remote control.

The system can **record**, **store**, and **replay** IR commands using a **TSOP134 IR receiver**, an **IR333 infrared LED**, and a UART-based Common Terminal Interface (CTI).

---

## Features

- Record IR commands from an existing TV remote
- Store up to **16 different keys**
- Hardcoded **Power (ON/OFF)** command in slot 1
- Replay recorded commands using a 38 kHz IR carrier
- UART terminal interface for controlling the system
- Interrupt-driven IR reception
- Hardware timer-based waveform generation for accurate transmission

---

## Hardware

### Microcontroller
- Texas Instruments **TM4C123GH6PM**
- EK-TM4C123GXL LaunchPad

### Components
- TSOP134 IR Receiver
- IR333 Infrared LED
- 100 Ω resistor (required for TSOP134 supply)
- 4.7 µF capacitor across TSOP134 power pins
- USB UART connection

---

## Hardware Connections

| Device | TM4C Pin | Purpose |
|---------|----------|---------|
| UART RX | PA0 | Terminal input |
| UART TX | PA1 | Terminal output |
| IR LED | PB0 (T2CCP0) | PWM output (38 kHz carrier) |
| TSOP134 Output | PE2 | Edge interrupt input |

---

## System Architecture

```
                +----------------------+
                |   UART Terminal      |
                +----------+-----------+
                           |
                           v
                  Command Parser
                           |
           +---------------+---------------+
           |                               |
           v                               v
     IR Transmission                 IR Reception
        (IR333 LED)                  (TSOP134)
           |                               |
           |                               |
        Timer 2 PWM                  GPIO Interrupt
           |                               |
        Timer 3 Delay                 Timer 0 Capture
           |                               |
           +---------------+---------------+
                           |
                           v
                    Key Storage Array
                     (16 IR Commands)
```

---

# Project Structure

```
.
├── main.c
├── commands.c
├── storage.c
├── IR333.c
├── TSOP134.c
├── uart0.c
├── clock.c
├── wait.c
├── *.h
```

---

# Software Overview

## main.c

Initializes the hardware:

- System clock (40 MHz)
- UART0
- Stores the hardcoded ON/OFF key
- Starts the Common Terminal Interface loop

```c
while(1)
{
    getsUart0(&data);
    commands(&data);
}
```

---

## UART Interface

Provides a simple serial command interface running at:

- **115200 baud**
- 8 data bits
- No parity
- 1 stop bit (8N1)

Implemented functions include:

- `initUart0()`
- `getsUart0()`
- `putsUart0()`

---

## Command Interpreter

Supported commands:

```
recv <number>
```

Records the next IR command into memory slot `<number>`.

Example:

```
recv 2
```

---

```
xmit <number>
```

Transmits the stored command.

Example:

```
xmit 2
```

---

## IR Reception

Implemented in **TSOP134.c**

Reception is interrupt-driven.

### Hardware Used

- GPIO Port E Pin 2
- Timer 0

Every edge detected from the TSOP134 generates an interrupt.

The interrupt:

1. Reads the current timer value
2. Calculates elapsed time
3. Stores pulse width inside

```c
key[index].array[]
```

This reconstructs the complete IR waveform.

---

## IR Transmission

Implemented in **IR333.c**

Transmission uses two hardware timers.

### Timer 2

Configured as PWM.

Produces approximately **38 kHz** carrier with 50% duty cycle.

```
Period = 1052 counts
Duty = 526 counts
```

This drives the IR LED.

---

### Timer 3

Configured as a one-shot delay timer.

Each recorded pulse width determines how long Timer 2 remains enabled or disabled.

The waveform generated closely matches the original remote.

---

## Data Storage

All IR commands are stored in

```c
KEYS key[16];
```

Each key contains an array of pulse widths.

```
Start Pulse
↓

9000
4500

↓

32 Data Bits

↓

560
560 / 1690

↓

560
```

Each command occupies 67 timing values.

---

## Hardcoded Power Button

Slot **1** is initialized during startup.

```c
storer(1, 0x20DF10EF);
```

This corresponds to the NEC IR code:

```
0x20DF10EF
```

which is a common LG TV Power command.

---

# Timing

The project relies entirely on hardware timers for accurate timing.

| Function | Timer | Purpose |
|-----------|-------|----------|
| Timer0 | 32-bit periodic | Measure received pulse widths |
| Timer2 | PWM | Generate 38 kHz carrier |
| Timer3 | One-shot | Pulse timing during transmission |

System clock:

```
40 MHz
```

Timer resolution:

```
25 ns
```

Pulse durations are converted into microseconds before storage.

---

# Foreground and Background Tasks

## Foreground Thread

Runs continuously inside `main()`.

Responsibilities:

- Read UART commands
- Parse user input
- Start transmission
- Enable recording

---

## Background Thread

Runs as GPIO interrupt.

Triggered by:

```
Port E Pin 2
```

Responsibilities:

- Detect every IR edge
- Measure elapsed time
- Save waveform into memory

---

# Supported Commands

| Command | Description |
|----------|-------------|
| `recv <1-16>` | Record an IR command into memory |
| `xmit <1-16>` | Replay a stored IR command |

Example session:

```
recv 2

Press TV remote button...

xmit 2

Stored command is replayed.
```

---

# NEC Protocol

The project records raw timing values rather than decoding protocol bits.

The default hardcoded command follows the NEC protocol:

```
Leader

9 ms HIGH
4.5 ms LOW

Bit 0

560 us HIGH
560 us LOW

Bit 1

560 us HIGH
1690 us LOW

Stop Bit

560 us HIGH
```

---

# Building

Designed for:

- Code Composer Studio (CCS)
- TM4C123GH6PM
- TivaWare headers

Required files include:

- tm4c123gh6pm.h
- clock.c
- uart0.c
- wait.c

---

# Example Usage

Power command:

```
xmit 1
```

Record Volume Up:

```
recv 2
```

Press the Volume Up button on the TV remote.

Replay later:

```
xmit 2
```

---

# Design Highlights

- Hardware PWM generation for precise IR carrier
- Interrupt-driven waveform capture
- Accurate timer-based playback
- UART command interface
- Modular software architecture
- Minimal CPU overhead during IR timing

---

# Future Improvements

- Decode and display NEC addresses/commands
- EEPROM or Flash storage for persistent key memory
- Support multiple IR protocols (Sony, RC5, RC6, Samsung)
- Repeat-code detection
- Command listing over UART
- Error checking and timeout handling
- Learning mode with automatic protocol detection

---

