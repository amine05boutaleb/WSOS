# GitHub Codespaces - Complete Fix for Display Library Error

## The Problem

You're getting this error:
```
>>PANIC<< bochsrc-linux.bxrc:6: display library 'nogui' not available
```

This happens because Bochs in Codespaces doesn't have all display libraries compiled in.

---

## ✅ SOLUTION - 3 Methods (Try in Order)

### **Method 1: Use Minimal Config File** (Recommended)

```bash
cd OS
bochs -f bochsrc-linux-simple.bxrc -q
```

At the `<bochs:1>` prompt, type `c` and press Enter.

---

### **Method 2: Use Shell Script** (Easiest)

```bash
cd OS
chmod +x run-bochs.sh
./run-bochs.sh
```

At the `<bochs:1>` prompt, type `c` and press Enter.

---

### **Method 3: Run Bochs with Command-Line Options** (Most Reliable)

This bypasses config files entirely:

```bash
cd OS
bochs -q \
    'boot:floppy' \
    'floppya: 1_44=./build/os.img, status=inserted' \
    'megs: 32' \
    'romimage: file=$BXSHARE/BIOS-bochs-latest' \
    'vgaromimage: file=$BXSHARE/VGABIOS-lgpl-latest'
```

At the `<bochs:1>` prompt, type `c` and press Enter.

---

## 🎯 Complete Step-by-Step (Method 3 - Most Reliable)

Copy and paste these commands one by one:

### 1. Install Dependencies (if not done)
```bash
sudo apt update
sudo apt install -y build-essential nasm bochs bochs-x gcc-multilib
```

### 2. Navigate and Build
```bash
cd /workspaces/*/OS  # Or: cd OS
make clean
make
```

### 3. Verify Build
```bash
ls -lh build/os.img
```
You should see a ~1.4M file.

### 4. Run Bochs (Command-Line Method)
```bash
bochs -q 'boot:floppy' 'floppya: 1_44=./build/os.img, status=inserted' 'megs: 32' 'romimage: file=$BXSHARE/BIOS-bochs-latest' 'vgaromimage: file=$BXSHARE/VGABIOS-lgpl-latest'
```

### 5. At the Bochs Prompt
When you see:
```
<bochs:1>
```

Type:
```
c
```

Then press **Enter**.

### 6. Expected Output
You should see:
```
Kernel Process Starting...
ABCDEBCDECDC
Kernel Process Exiting...
Kernel Process Exiting...
...
Kernel Process Terminated
```

**Success!** The output **ABCDEBCDECDC** proves your scheduler works correctly!

### 7. Exit Bochs
- Press **Ctrl+C**
- Type `quit` or `q`
- Press Enter

---

## 🔍 Alternative: Install X Server Support

If you want to try running with a virtual display:

```bash
# Install X virtual framebuffer
sudo apt install -y xvfb

# Run with virtual X server
xvfb-run -a bochs -q 'boot:floppy' 'floppya: 1_44=./build/os.img, status=inserted' 'megs: 32'
```

Then type `c` at the prompt.

---

## 📝 One-Liner (Copy-Paste Everything)

If you've already built the OS, just use this one command:

```bash
cd OS && bochs -q 'boot:floppy' 'floppya: 1_44=./build/os.img, status=inserted' 'megs: 32' 'romimage: file=$BXSHARE/BIOS-bochs-latest' 'vgaromimage: file=$BXSHARE/VGABIOS-lgpl-latest'
```

Then type `c` and press Enter.

---

## 🛠️ Create an Alias (Optional)

To make running easier in the future:

```bash
# Add to your bash profile
echo "alias run-wsos='cd /workspaces/*/OS && bochs -q \"boot:floppy\" \"floppya: 1_44=./build/os.img, status=inserted\" \"megs: 32\" \"romimage: file=\$BXSHARE/BIOS-bochs-latest\" \"vgaromimage: file=\$BXSHARE/VGABIOS-lgpl-latest\"'" >> ~/.bashrc

# Reload
source ~/.bashrc

# Now you can just type:
run-wsos
```

---

## ❓ Troubleshooting

### Error: "Cannot find ROM image"

**Try specifying full path:**
```bash
bochs -q \
    'boot:floppy' \
    'floppya: 1_44=./build/os.img, status=inserted' \
    'megs: 32' \
    'romimage: file=/usr/share/bochs/BIOS-bochs-latest' \
    'vgaromimage: file=/usr/share/bochs/VGABIOS-lgpl-latest'
```

### Error: "Cannot open floppy image"

**Check you're in the OS directory:**
```bash
pwd  # Should end with /OS
ls build/os.img  # Should exist
```

### Bochs Hangs or Shows Weird Characters

**This is normal!** Type `c` and press Enter to continue.

### No Output After Typing 'c'

**Wait a few seconds** - the OS takes time to boot. You should see output within 5-10 seconds.

### Error: "bochs: command not found"

**Install Bochs:**
```bash
sudo apt install -y bochs bochs-x
```

---

## 🎓 Understanding What's Happening

When you run Bochs with these options:

1. **`-q`** = Quick start (skip configuration menu)
2. **`boot:floppy`** = Boot from floppy disk
3. **`floppya: 1_44=./build/os.img`** = Load your OS image as floppy disk A
4. **`megs: 32`** = Allocate 32MB RAM
5. **`romimage`** = BIOS ROM file
6. **`vgaromimage`** = VGA BIOS file

Bochs emulates a complete x86 PC and boots your custom OS!

---

## 📊 Quick Reference

| Command | Purpose |
|---------|---------|
| `make clean` | Remove old build files |
| `make` | Build the OS |
| `bochs -q ...` | Run the OS in emulator |
| `c` (at bochs prompt) | Continue/start execution |
| `Ctrl+C` | Interrupt Bochs |
| `quit` | Exit Bochs |

---

## ✅ Success Checklist

- [ ] Dependencies installed
- [ ] In the `OS` directory
- [ ] Build completed successfully (`build/os.img` exists)
- [ ] Bochs starts without errors
- [ ] Typed `c` at the `<bochs:1>` prompt
- [ ] See output: **ABCDEBCDECDC**
- [ ] Can exit Bochs with Ctrl+C and `quit`

---

## 🚀 Next Steps

Once you see **ABCDEBCDECDC**:

1. ✅ Your scheduler is fixed and working!
2. ✅ You've successfully run a custom OS!
3. Try modifying the processes in `src/kernel.c`
4. Rebuild with `make` and test again

---

## 💡 Pro Tip

Create a simple script `test.sh`:

```bash
#!/bin/bash
cd OS
make clean
make
bochs -q 'boot:floppy' 'floppya: 1_44=./build/os.img, status=inserted' 'megs: 32' 'romimage: file=$BXSHARE/BIOS-bochs-latest' 'vgaromimage: file=$BXSHARE/VGABIOS-lgpl-latest' <<EOF
c
EOF
```

Make it executable:
```bash
chmod +x test.sh
```

Run with:
```bash
./test.sh
```

This will automatically build and run your OS, typing `c` for you!

---

**Still having issues?** Make sure you're using **Method 3** (command-line options) - it's the most reliable!
