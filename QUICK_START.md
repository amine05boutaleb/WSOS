# Quick Start Guide - WSOS

## TL;DR - Get Expected Output in 4 Commands

```bash
cd "C:\Users\AmineBOUTALEB\Downloads\project 3\WSOS\OS"
make clean
make
bochs -f bochsrc.bxrc
```

**Expected Output:** `ABCDEBCDECDC`

---

## What Changed to Fix the Scheduler

### Problem
Original output was `BBCCCCDDDEEA` instead of `ABCDEBCDECDC`

### Root Cause
Scheduler was using `running->pid` (kernel process) instead of `prev->pid` (last user process), breaking round-robin order.

### Solution

**File: `OS/src/multitasking.c`**

**Change 1 - Line 22 in `schedule()`:**
```c
// BEFORE:
int start_pid = (running) ? running->pid : -1;

// AFTER:
int start_pid = (prev) ? prev->pid : -1;
```

**Change 2 - Line 128 in `yield()`:**
```c
// BEFORE:
if (schedule() == 1)
{
    next->status = PROC_STATUS_RUNNING;
    contextswitch();
}

// AFTER:
if (schedule() == 1)
{
    next->status = PROC_STATUS_RUNNING;
    prev = next;  // Track last scheduled process
    contextswitch();
}
```

---

## Verification Checklist

- [ ] Code changes saved in `OS/src/multitasking.c`
- [ ] Line 22: Uses `prev->pid` not `running->pid`
- [ ] Line 128: Added `prev = next;`
- [ ] `make clean` executed
- [ ] `make` completes without errors
- [ ] `build/os.img` exists (~1.44 MB)
- [ ] `bochsrc.bxrc` line 10 points to correct path
- [ ] Bochs displays: **ABCDEBCDECDC**

---

## Process Execution Flow (Expected Output Explained)

| Step | Process | Action | Output | Remaining Processes |
|------|---------|--------|--------|-------------------|
| 1 | A | Print & Exit | **A** | B, C, D, E |
| 2 | B | Print & Yield | **B** | B, C, D, E |
| 3 | C | Print & Yield | **C** | B, C, D, E |
| 4 | D | Print & Yield | **D** | B, C, D, E |
| 5 | E | Print & Yield | **E** | B, C, D, E |
| 6 | B | Print & Exit | **B** | C, D, E |
| 7 | C | Print & Yield | **C** | C, D, E |
| 8 | D | Print & Yield | **D** | C, D, E |
| 9 | E | Print & Exit | **E** | C, D |
| 10 | C | Print & Yield | **C** | C, D |
| 11 | D | Print & Yield | **D** | C, D |
| 12 | C | Print & Exit | **C** | D |
| 13 | D | Print & Exit | **D** | - |

**Final Output: ABCDEBCDECDC** ✓

---

## Key Files Modified

1. **`OS/src/multitasking.c`** - Fixed scheduler logic
2. **`OS/bochsrc.bxrc`** - Updated path to os.img

---

## Prerequisites

- GCC with 32-bit support
- NASM assembler
- GNU Make
- Bochs 3.0 at `C:\Program Files\Bochs-3.0\`

---

## Troubleshooting

**Build fails:**
- Run `make clean` first
- Check GCC, NASM, Make are installed

**Wrong output:**
- Verify changes to `multitasking.c` lines 22 and 128
- Rebuild with `make clean && make`

**Bochs won't start:**
- Check path in `bochsrc.bxrc` line 10
- Verify `build/os.img` exists

---

For detailed explanations, see `STEP_BY_STEP_GUIDE.md`
