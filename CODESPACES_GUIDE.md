# GitHub Codespaces Testing Guide

## Complete Step-by-Step Instructions

Follow these steps to build and test WSOS in GitHub Codespaces.

---

## Step 1: Install Dependencies

In the Codespaces terminal, run:

```bash
sudo apt update
sudo apt install -y build-essential nasm bochs bochs-x
```

**Expected output**: Various package installation messages ending with "Done"

---

## Step 2: Navigate to OS Directory

```bash
cd OS
```

---

## Step 3: Clean and Build

```bash
make clean
make
```

**Expected output**:
- Various compilation messages
- Should end without errors
- `build/os.img` file should be created

**Verify the build:**
```bash
ls -lh build/os.img
```

You should see a file approximately 1.44 MB in size.

---

## Step 4: Run with Linux-Compatible Configuration

**Use the Linux-specific configuration file:**

```bash
bochs -f bochsrc-linux.bxrc -q
```

### What the flags mean:
- `-f bochsrc-linux.bxrc` - Use Linux-compatible configuration
- `-q` - Quick start (skip initial prompts)

---

## Step 5: Interact with Bochs

When Bochs starts in text mode, you'll see a prompt like:

```
========================================================================
                        Bochs x86 Emulator 2.7
              Built from SVN snapshot on August  1, 2021
                  Compiled on Aug  2 2021 at 12:00:00
========================================================================
00000000000i[      ] reading configuration from bochsrc-linux.bxrc
...
<bochs:1>
```

### At the `<bochs:1>` prompt, type:

```
c
```

Then press **Enter**. This tells Bochs to "continue" and start running the OS.

---

## Step 6: View the Output

After typing `c` and pressing Enter, you should see the OS boot and display:

```
Kernel Process Starting...
ABCDEBCDECDC
Kernel Process Exiting...
Kernel Process Exiting...
...
Kernel Process Terminated
```

**Success!** You should see **ABCDEBCDECDC** which proves the round-robin scheduler is working correctly.

---

## Step 7: Exit Bochs

To exit Bochs, you may need to:

1. Press **Ctrl+C** to interrupt
2. At the `<bochs:>` prompt, type: `quit` or `q`
3. Or just close the terminal

---

## Troubleshooting

### Error: "config_interface 'win32config' not available"

**Solution**: Use `bochsrc-linux.bxrc` instead of `bochsrc.bxrc`:
```bash
bochs -f bochsrc-linux.bxrc -q
```

### Error: "bochs: command not found"

**Solution**: Install Bochs:
```bash
sudo apt install -y bochs bochs-x
```

### Error: "Cannot open file './build/os.img'"

**Solution**: Make sure you built the OS first:
```bash
make clean
make
ls build/os.img  # Verify it exists
```

### Error: Build fails with "nasm: command not found"

**Solution**: Install NASM:
```bash
sudo apt install -y nasm
```

### Error: Build fails with gcc errors about 32-bit

**Solution**: Install 32-bit support:
```bash
sudo apt install -y gcc-multilib
```

---

## Quick Reference - All Commands

Here's everything in one place:

```bash
# 1. Install dependencies (one time only)
sudo apt update
sudo apt install -y build-essential nasm bochs bochs-x gcc-multilib

# 2. Navigate and build
cd OS
make clean
make

# 3. Run Bochs
bochs -f bochsrc-linux.bxrc -q

# 4. At <bochs:1> prompt, type:
c

# 5. See output: ABCDEBCDECDC

# 6. Exit: Ctrl+C, then type:
quit
```

---

## Differences Between Windows and Linux Configs

| Setting | Windows (bochsrc.bxrc) | Linux (bochsrc-linux.bxrc) |
|---------|------------------------|----------------------------|
| **config_interface** | `win32config` | `textconfig` |
| **display_library** | `win32` | `nogui` |
| **romimage path** | Absolute Windows path | `$BXSHARE/BIOS-bochs-latest` |
| **vgaromimage path** | Absolute Windows path | `$BXSHARE/VGABIOS-lgpl-latest` |
| **floppy path** | Absolute Windows path | Relative: `./build/os.img` |

---

## Understanding the Output

When you see **ABCDEBCDECDC**, here's what's happening:

| Character | Process | What Happened |
|-----------|---------|---------------|
| A | Process A | Printed "A" and exited |
| B | Process B | Printed "B" and yielded (1st time) |
| C | Process C | Printed "C" and yielded (1st time) |
| D | Process D | Printed "D" and yielded (1st time) |
| E | Process E | Printed "E" and yielded (1st time) |
| B | Process B | Printed "B" and exited (2nd time) |
| C | Process C | Printed "C" and yielded (2nd time) |
| D | Process D | Printed "D" and yielded (2nd time) |
| E | Process E | Printed "E" and exited (2nd time) |
| C | Process C | Printed "C" and yielded (3rd time) |
| D | Process D | Printed "D" and yielded (3rd time) |
| C | Process C | Printed "C" and exited (4th time) |
| D | Process D | Printed "D" and exited (4th time) |

This proves your **round-robin scheduler** is working correctly!

---

## Alternative: Using X Server (For GUI - Advanced)

If you want to see the graphical Bochs window in Codespaces, you need an X server:

```bash
# Install X virtual framebuffer
sudo apt install -y xvfb

# Run Bochs with virtual display
xvfb-run bochs -f bochsrc.bxrc -q
```

**Note**: This won't show a window, but allows GUI-based configs to work.

---

## Saving Your Codespace

GitHub Codespaces auto-saves your work. Your changes to the code are saved in the cloud.

To download your built OS image:
1. Right-click `OS/build/os.img` in the file explorer
2. Select "Download"

---

## Next Steps

Once you see the correct output (**ABCDEBCDECDC**):

✅ Your scheduler is working correctly!
✅ You've successfully built and tested an OS!
✅ You can modify processes in `src/kernel.c` and rebuild to experiment

---

## Need Help?

If you encounter issues not covered here:
1. Check that you're in the `OS` directory: `pwd`
2. Verify all files are present: `ls -la`
3. Check the build completed: `ls -lh build/os.img`
4. Review error messages carefully
5. Ensure you're using `bochsrc-linux.bxrc` not `bochsrc.bxrc`
