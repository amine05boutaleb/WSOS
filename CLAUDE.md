# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

WSOS is a simple x86 operating system written in C and x86 assembly. It boots from a floppy disk image, runs in 32-bit protected mode, and implements basic multitasking with cooperative scheduling.

## Build System

### Building the OS

The project uses a Makefile with NASM for assembly and GCC for C compilation. All builds target 32-bit x86.

```bash
cd OS
make
```

This produces `build/os.img` - a bootable floppy disk image.

### Cleaning Build Artifacts

```bash
cd OS
make clean
```

### Build Configuration

- **Compiler**: GCC with `-m32 -fno-pie -ffreestanding` flags
- **Assembler**: NASM for all assembly files
- **Linker**: GNU ld with `-m elf_i386` targeting address `0x1000`
- **Target**: 32-bit x86 (i386)

## Running the OS

The OS is configured to run in Bochs emulator. Configuration is in `OS/bochsrc.bxrc`:

```bash
bochs -f OS/bochsrc.bxrc
```

The Bochs configuration expects the OS image at `C:\CEG4350\WSOS\OS\build\os.img`. Update line 10 in `bochsrc.bxrc` if your path differs.

## Architecture

### Boot Sequence

1. **Bootloader** (`asm/bootloader.asm`): Loads at 0x7C00, sets up FAT12 boot sector
   - Loads kernel from disk (48 sectors)
   - Disables blinking cursor in real mode
   - Switches to protected mode via GDT setup

2. **Protected Mode Entry** (`asm/switch.asm`, `asm/gdt.asm`):
   - Configures Global Descriptor Table
   - Switches CPU to 32-bit protected mode
   - Jumps to kernel at 0x1000

3. **Kernel Entry** (`asm/kernel_entry.asm`): Assembly stub that calls C `main()`

4. **Kernel Initialization** (`src/kernel.c:main()`):
   - Clears screen
   - Initializes keyboard
   - Installs IDT, ISRs, and IRQs
   - Starts kernel process

### Multitasking System

The OS implements **cooperative multitasking** with a simple round-robin scheduler.

#### Process Model

- Maximum 6 processes: 1 kernel + 5 user processes
- Process Control Block (PCB) stores: pid, type, status, all CPU registers (EAX-EDI, ESP, EBP, EIP, EFLAGS, CS, CR3)
- Process types: `PROC_TYPE_KERNEL` or `PROC_TYPE_USER`
- Process states: `PROC_STATUS_READY`, `PROC_STATUS_RUNNING`, `PROC_STATUS_TERMINATED`

#### Scheduling (`src/multitasking.c`)

- **Round-robin**: Cycles through process array starting from current PID
- Only schedules `READY` user processes
- Kernel process runs between user process yields

#### Context Switching

- Triggered by `yield()` or `exit()` system calls
- User processes yield control back to kernel
- Kernel schedules next user process and context switches to it
- Context switch uses interrupt 0x80 with syscall code 0x01
- Assembly stub (`asm/interrupt.asm`) saves/restores all registers

### Interrupt Handling

#### IDT Setup (`src/idt.c`)

- 256-entry Interrupt Descriptor Table
- ISRs (0-31): CPU exception handlers
- IRQs (32-47): Hardware interrupt handlers (remapped from 8-15)

#### System Call (INT 0x80)

Currently handles one syscall:
- **Code 0x01**: Context switch
  - EBX = pointer to current process PCB
  - ECX = pointer to next process PCB
  - Saves current registers to EBX PCB
  - Restores registers from ECX PCB

### Memory Layout

- **0x7C00**: Bootloader load address
- **0x8000**: Initial stack (grows downward)
- **0x1000**: Kernel load address
- **0x10000, 0x20000, 0x30000, 0x40000, 0x50000**: User process stacks

### Disk Structure

The OS image is a FAT12 floppy disk (1.44MB):
1. Bootloader (512 bytes)
2. FAT table (`asm/fat.asm`)
3. Root directory (`asm/root_dir.asm`)
4. Kernel binary (up to 48 sectors)

### Floppy Disk Driver (`src/fdc.c`, `src/dma.c`)

- Implements floppy disk controller (FDC) driver
- Uses DMA channel 2 for data transfers
- Supports read/write operations with LBA addressing
- Functions: `floppy_init()`, `floppy_read()`, `floppy_write()`

## Code Organization

```
OS/
├── asm/                  # Assembly sources
│   ├── bootloader.asm    # Boot sector and loader
│   ├── kernel_entry.asm  # Kernel C entry point
│   ├── interrupt.asm     # ISR/IRQ stubs and context switch
│   ├── gdt.asm           # Global Descriptor Table
│   ├── switch.asm        # Protected mode transition
│   ├── fat.asm           # FAT12 table
│   └── root_dir.asm      # Root directory entries
├── src/                  # C sources
│   ├── kernel.c          # Main kernel and test processes
│   ├── multitasking.c    # Process management and scheduler
│   ├── idt.c             # Interrupt descriptor table
│   ├── isr.c             # Interrupt service routines
│   ├── irq.c             # Hardware interrupt handlers
│   ├── io.c              # Screen output and keyboard input
│   ├── fdc.c             # Floppy disk controller driver
│   └── dma.c             # DMA controller for floppy
├── include/              # Header files
│   └── *.h               # Corresponding headers for src/
├── build/                # Build output directory
└── Makefile              # Build configuration
```

## Key APIs

### Multitasking (`include/multitasking.h`)

- `int startkernel(void func())` - Start kernel process
- `int createproc(void *func, char *stack)` - Create user process
- `void yield()` - Cooperatively yield CPU to scheduler
- `void exit()` - Terminate current process
- `int schedule()` - Select next ready user process
- `int ready_process_count()` - Count ready user processes

### I/O (`include/io.h`)

- `void clearscreen()` - Clear VGA text buffer
- `void printf(char *str)` - Print string to screen
- `void initkeymap()` - Initialize keyboard handler

### Interrupts

- `void idt_install()` - Install interrupt descriptor table
- `void isrs_install()` - Install CPU exception handlers
- `void irq_install()` - Install hardware IRQ handlers
- `void irq_install_handler(int irq, void (*handler)(regs *r))` - Register IRQ callback

## Development Notes

### Adding New Processes

Edit `src/kernel.c` and add process functions following the pattern of `proc_a()` through `proc_e()`. Create them in `prockernel()` using `createproc(proc_func, (void *)stack_address)`.

### Modifying Scheduler

The scheduler in `src/multitasking.c:schedule()` is round-robin. To change scheduling policy, modify the loop logic to select processes differently.

### Adding System Calls

1. Define new syscall code (e.g., 0x02)
2. Add handler logic in `asm/interrupt.asm` ISR 128 handler
3. Create C wrapper function that loads registers and calls `int $0x80`

### Debugging

Bochs magic breakpoint is enabled (`magic_break: enabled=1`). Insert `xchg bx, bx` in assembly for breakpoints.

## Compilation Requirements

- GCC with 32-bit support (`gcc-multilib` on 64-bit Linux)
- NASM assembler
- GNU ld linker
- GNU make
- Bochs emulator (for testing)

On Windows, use MSYS2, Cygwin, or WSL with the above tools installed.
