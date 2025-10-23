# Step-by-Step Guide to Build and Run WSOS

This guide will walk you through building and running the WSOS operating system to see the expected output: **ABCDEBCDECDC**

## Prerequisites

Before you begin, ensure you have the following installed on your Windows system:

1. **GCC (GNU Compiler Collection)** - for compiling C code
   - Must support 32-bit compilation (`-m32` flag)
   - Recommended: Install via MSYS2, Cygwin, or MinGW

2. **NASM (Netwide Assembler)** - for assembling x86 assembly code
   - Download from: https://www.nasm.us/

3. **GNU Make** - for running the build system
   - Usually included with MSYS2/Cygwin

4. **Bochs Emulator** - for running the OS
   - Version 3.0 or compatible
   - Must be installed at: `C:\Program Files\Bochs-3.0\`
   - Download from: https://bochs.sourceforge.io/

### Verifying Prerequisites

Open a terminal (Command Prompt, PowerShell, MSYS2, or Cygwin) and verify:

```bash
# Check GCC
gcc --version

# Check NASM
nasm --version

# Check Make
make --version

# Check Bochs installation
dir "C:\Program Files\Bochs-3.0\"
```

## Step 1: Navigate to the OS Directory

Open your terminal and navigate to the OS directory:

```bash
cd "C:\Users\AmineBOUTALEB\Downloads\project 3\WSOS\OS"
```

## Step 2: Clean Previous Builds (Optional but Recommended)

Before building, it's good practice to clean any previous build artifacts:

```bash
make clean
```

You should see output like:
```
rm -rf ./build/*
```

## Step 3: Build the OS

Run the make command to build the entire operating system:

```bash
make
```

### What Happens During Build:

The build process will:
1. Assemble the bootloader (`bootloader.asm`) → `build/bootloader.bin`
2. Assemble the FAT12 table (`fat.asm`) → `build/fat.bin`
3. Assemble the root directory (`root_dir.asm`) → `build/root_dir.bin`
4. Assemble the kernel entry point (`kernel_entry.asm`) → `build/kernel_entry.o`
5. Assemble the interrupt handlers (`interrupt.asm`) → `build/interrupt.o`
6. Compile all C source files (kernel.c, multitasking.c, idt.c, etc.) → `build/*.o`
7. Link everything together → `build/kernel.bin`
8. Concatenate all components → `build/os.img` (final bootable image)

### Expected Build Output:

You should see commands like:
```
nasm ./asm/bootloader.asm -f bin -o ./build/bootloader.bin
nasm ./asm/fat.asm -f bin -o ./build/fat.bin
nasm ./asm/root_dir.asm -f bin -o ./build/root_dir.bin
nasm ./asm/kernel_entry.asm -f elf -o ./build/kernel_entry.o
gcc -m32 -fno-pie -ffreestanding -Wall -Wextra -I./include -c ./src/kernel.c -o ./build/kernel.o
gcc -m32 -fno-pie -ffreestanding -Wall -Wextra -I./include -c ./src/multitasking.c -o ./build/multitasking.o
...
ld -m elf_i386 -s -o ./build/kernel.bin -Ttext 0x1000 ... --oformat binary
cat ./build/bootloader.bin ./build/fat.bin ./build/root_dir.bin ./build/kernel.bin > ./build/os.img
```

### Verify Build Success:

Check that the OS image was created:

```bash
ls -lh build/os.img
```

Or on Windows Command Prompt:
```cmd
dir build\os.img
```

The file should exist and be approximately 1.44 MB (floppy disk size).

## Step 4: Run the OS in Bochs Emulator

Now that the OS image is built, run it in Bochs:

```bash
cd ..
bochs -f OS/bochsrc.bxrc
```

Or from the OS directory:
```bash
bochs -f bochsrc.bxrc
```

### What to Expect:

1. **Bochs Configuration Interface** will appear
2. You may see a dialog asking about configuration - click "Continue" or press Enter
3. **Bochs Display Window** will open showing the OS booting

### Bochs Troubleshooting:

If Bochs doesn't start or shows errors:

**Error: "Cannot open file 'C:\Users\AmineBOUTALEB\Downloads\project 3\WSOS\OS\build\os.img'"**
- Solution: Make sure the path in `bochsrc.bxrc` line 10 matches your actual file location
- Edit line 10 if needed to match your directory structure

**Error: "ROM image not found"**
- Solution: Verify Bochs is installed at `C:\Program Files\Bochs-3.0\`
- If installed elsewhere, update lines 6-7 in `bochsrc.bxrc`

## Step 5: View the Output

Once Bochs starts and the OS boots, you should see output in the Bochs display window.

### Expected Output:

```
Kernel Process Starting...
ABCDEBCDECDC
Kernel Process Exiting...
[repeated several times]
Kernel Process Terminated
```

### Breaking Down the Output:

The sequence **ABCDEBCDECDC** represents:

1. **A** - Process A runs once, prints "A", then exits
2. **B** - Process B runs first time, prints "B", then yields
3. **C** - Process C runs first time, prints "C", then yields
4. **D** - Process D runs first time, prints "D", then yields
5. **E** - Process E runs first time, prints "E", then yields
6. **B** - Process B runs second time, prints "B", then exits
7. **C** - Process C runs second time, prints "C", then yields
8. **D** - Process D runs second time, prints "D", then yields
9. **E** - Process E runs second time, prints "E", then exits
10. **C** - Process C runs third time, prints "C", then yields
11. **D** - Process D runs third time, prints "D", then yields
12. **C** - Process C runs fourth time, prints "C", then exits
13. **D** - Process D runs fourth time, prints "D", then exits

This demonstrates proper **round-robin scheduling** where processes are given CPU time in a circular order.

## Step 6: Exit Bochs

To close the Bochs emulator:
- Click the X button on the Bochs display window, OR
- In the Bochs control window, type: `quit` or `q`

## Understanding What Changed

The fixes applied to make this work correctly:

### File: `OS/src/multitasking.c`

**1. Fixed `schedule()` function (lines 18-36):**
- **Before**: Used `running->pid` which pointed to the kernel process
- **After**: Uses `prev->pid` which points to the last scheduled user process
- **Why**: This ensures round-robin continues from where it left off

**2. Updated `yield()` function (lines 115-132):**
- **Added**: `prev = next;` when kernel schedules a process (line 128)
- **Why**: Tracks which process was just scheduled for the next round-robin cycle

## Verifying Round-Robin Behavior

Each process is defined in `OS/src/kernel.c`:

- **proc_a()**: Prints "A" once, then exits
- **proc_b()**: Prints "B", yields, prints "B" again, then exits
- **proc_c()**: Prints "C", yields, prints "C", yields, prints "C", yields, prints "C", then exits (4 times total)
- **proc_d()**: Prints "D", yields, prints "D", yields, prints "D", then exits (3 times total)
- **proc_e()**: Prints "E", yields, prints "E", then exits (2 times total)

The scheduler gives each process a turn in order (A→B→C→D→E), then wraps around to the beginning, skipping terminated processes.

## Common Issues and Solutions

### Issue: Make command not found
**Solution**: Install GNU Make via MSYS2, Cygwin, or add it to your PATH

### Issue: GCC error about 32-bit support
**Solution**:
- On 64-bit systems, install `gcc-multilib`
- On MSYS2: `pacman -S mingw-w64-i686-gcc`

### Issue: NASM command not found
**Solution**: Install NASM and add it to your PATH environment variable

### Issue: Wrong output (not ABCDEBCDECDC)
**Solution**:
- Make sure all changes to `multitasking.c` are saved
- Run `make clean` then `make` to rebuild from scratch
- Verify the changes in lines 22 and 128 of `multitasking.c`

### Issue: Bochs crashes or hangs
**Solution**:
- Check that `os.img` was built successfully and is ~1.44 MB
- Verify all paths in `bochsrc.bxrc` are correct
- Try running Bochs with the `-q` flag: `bochs -q -f OS/bochsrc.bxrc`

## Quick Reference Commands

```bash
# Navigate to OS directory
cd "C:\Users\AmineBOUTALEB\Downloads\project 3\WSOS\OS"

# Clean build
make clean

# Build OS
make

# Run in Bochs (from project root)
bochs -f OS/bochsrc.bxrc
```

## Success Criteria

You have successfully completed the setup if:
- ✅ Build completes without errors
- ✅ `build/os.img` file is created (~1.44 MB)
- ✅ Bochs emulator starts without errors
- ✅ Output displays: **ABCDEBCDECDC**
- ✅ Kernel reports "Kernel Process Terminated" at the end

## Additional Notes

- The OS runs entirely in the Bochs emulator - it does not affect your actual system
- You can modify process behavior in `OS/src/kernel.c` functions `proc_a()` through `proc_e()`
- To add more processes, update `MAX_USER_PROCS` in `OS/include/multitasking.h`
- The scheduler implementation is in `OS/src/multitasking.c`

## Need Help?

If you encounter issues:
1. Check the build output for specific error messages
2. Verify all prerequisites are installed correctly
3. Ensure file paths match your system configuration
4. Review the CLAUDE.md file for architecture details
