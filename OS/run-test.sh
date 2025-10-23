#!/bin/bash
# Script to run WSOS and capture output

echo "Running WSOS Operating System..."
echo "================================"
echo ""

# Create temporary config
cat > /tmp/bochs-temp.cfg << 'EOF'
megs: 32
romimage: file=/usr/share/bochs/BIOS-bochs-latest
vgaromimage: file=/usr/share/bochs/VGABIOS-lgpl-latest
boot: floppy
floppya: 1_44=./build/os.img, status=inserted
log: /tmp/bochs-output.log
error: action=report
info: action=ignore
panic: action=fatal
EOF

# Run bochs with automatic commands
{
    echo "6"  # Begin simulation
    echo "c"  # Continue
    sleep 3    # Let it run for 3 seconds
    echo "q"  # Quit
} | DISPLAY= bochs -f /tmp/bochs-temp.cfg -q 2>&1 | tee /tmp/bochs-run.log

# Try to extract the output from the log
echo ""
echo "Bochs Output Log:"
echo "================="
if [ -f /tmp/bochs-output.log ]; then
    cat /tmp/bochs-output.log | grep -A 50 "Kernel Process Starting"
else
    echo "Log file not found. Checking run output..."
    cat /tmp/bochs-run.log | tail -50
fi

# Clean up
rm -f /tmp/bochs-temp.cfg
