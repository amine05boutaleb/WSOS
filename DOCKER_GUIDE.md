# Docker Guide for WSOS

## Quick Start with Docker

If you have Docker installed, you can build and test WSOS without installing NASM, GCC, or Bochs locally.

### Prerequisites

- Docker Desktop for Windows
- Download from: https://www.docker.com/products/docker-desktop/

### Steps

1. **Open PowerShell or Command Prompt**

2. **Navigate to project directory:**
   ```bash
   cd "C:\Users\AmineBOUTALEB\Downloads\project 3\WSOS"
   ```

3. **Build Docker image:**
   ```bash
   docker build -t wsos .
   ```

4. **Run the container:**
   ```bash
   docker run -it wsos
   ```

### Alternative: Interactive Development

To work interactively inside the container:

```bash
# Run container with shell
docker run -it --rm -v "${PWD}:/wsos" ubuntu:22.04 bash

# Inside container, install tools
apt-get update
apt-get install -y build-essential nasm bochs

# Build and run
cd /wsos/OS
make clean && make
bochs -f bochsrc.bxrc -q
```

### Note

Docker runs in text mode, so you won't see the graphical Bochs window, but you will see the text output including the expected **ABCDEBCDECDC** sequence.
