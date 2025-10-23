#!/bin/bash
# Simple script to run WSOS in Bochs on Linux/Codespaces

# Check if os.img exists
if [ ! -f "./build/os.img" ]; then
    echo "Error: os.img not found. Please run 'make' first."
    exit 1
fi

# Run Bochs with minimal configuration using command-line options
bochs -q \
    'boot:floppy' \
    'floppya: 1_44=./build/os.img, status=inserted' \
    'megs: 32' \
    'romimage: file=$BXSHARE/BIOS-bochs-latest' \
    'vgaromimage: file=$BXSHARE/VGABIOS-lgpl-latest'
