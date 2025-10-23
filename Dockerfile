# Dockerfile for WSOS Build Environment
FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    nasm \
    bochs \
    bochs-x \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /wsos

# Copy project files
COPY . .

# Build the OS
WORKDIR /wsos/OS
RUN make clean && make

# Default command
CMD ["bochs", "-f", "bochsrc.bxrc", "-q"]
