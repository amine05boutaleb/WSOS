#include "./io.h"
#include "./types.h"

// Track the current cursor's row and column
volatile int cursorCol = 0;
volatile int cursorRow = 0;

// Define a keymap to convert keyboard scancodes to ASCII
char keymap[128] = {};
volatile char *video_memory = (char *)0xB8000;

// C version of assembly I/O port instructions
// Allows for reading and writing with I/O
// The keyboard status port is 0x64
// The keyboard data port is 0x60
// More info here:
// https://wiki.osdev.org/I/O_Ports
// https://wiki.osdev.org/Port_IO
// https://bochs.sourceforge.io/techspec/PORTS.LST

// outb (out byte) - write an 8-bit value to an I/O port address (16-bit)
void initkeymap()
{
    keymap[0x1E] = 'a';
    keymap[0x30] = 'b';
    keymap[0x2E] = 'c';
    keymap[0x20] = 'd';
    keymap[0x12] = 'e';
    keymap[0x21] = 'f';
    keymap[0x22] = 'g';
    keymap[0x23] = 'h';
    keymap[0x17] = 'i';
    keymap[0x24] = 'j';
    keymap[0x25] = 'k';
    keymap[0x26] = 'l';
    keymap[0x32] = 'm';
    keymap[0x31] = 'n';
    keymap[0x18] = 'o';
    keymap[0x19] = 'p';
    keymap[0x10] = 'q';
    keymap[0x13] = 'r';
    keymap[0x1F] = 's';
    keymap[0x14] = 't';
    keymap[0x16] = 'u';
    keymap[0x2F] = 'v';
    keymap[0x11] = 'w';
    keymap[0x2D] = 'x';
    keymap[0x15] = 'y';
    keymap[0x2C] = 'z';
    keymap[0x0B] = '0';
    keymap[0x02] = '1';
    keymap[0x03] = '2';
    keymap[0x04] = '3';
    keymap[0x05] = '4';
    keymap[0x06] = '5';
    keymap[0x07] = '6';
    keymap[0x08] = '7';
    keymap[0x09] = '8';
    keymap[0x0A] = '9';
    keymap[0x39] = ' ';
    keymap[0x1C] = '\n';

    return;
}
void outb(uint16 port, uint8 value)
{
    asm volatile("outb %1, %0" : : "dN"(port), "a"(value));
    return;
}

// outw (out word) - write an 16-bit value to an I/O port address (16-bit)
void outw(uint16 port, uint16 value)
{
    asm volatile("outw %1, %0" : : "dN"(port), "a"(value));
    return;
}

// inb (in byte) - read an 8-bit value from an I/O port address (16-bit)
uint8 inb(uint16 port)
{
    uint8 ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "dN"(port));
    return ret;
}

// inw (in word) - read an 16-bit value from an I/O port address (16-bit)
uint16 inw(uint16 port)
{
    uint16 ret;
    asm volatile("inw %1, %0" : "=a"(ret) : "dN"(port));
    return ret;
}

// Setting the cursor does not display anything visually
// Setting the cursor is simply used by putchar() to find where to print next
// This can also be set independently of putchar() to print at any x, y coordinate on the screen
void setcursor(int x, int y)
{
    if (x < 0)
    {
        x = 0;
    }
    if (y < 0)
    {
        y = 0;
    }
    cursorCol = x % 80;
    cursorRow = y % 25;
    return;
}

char putchar(char character)
{
    if (character == '\n')
    {
        cursorRow++;
        cursorCol = 0;
    }
    else
    {
        video_memory[(cursorRow * 80 + cursorCol) * 2] = character;
        video_memory[(cursorRow * 80 + cursorCol) * 2 + 1] = 0x07;
        cursorCol++;
    }
    if (cursorCol >= 80)
    {
        cursorCol = 0;
        cursorRow++;
    }

    if (cursorRow >= 25)
    {
        cursorRow = 0;
    }
    return character;
}
char getchar()
{
    uint8 scancode = 0;

    while (1)
    {
        if (inb(0x64) & 1)
        {
            scancode = inb(0x60);
            if (!(scancode & 0x80))
            {
                return keymap[scancode];
            }
        }
    }
}

void scanf(char string[])
{
    int i = 0;
    char c;

    while (1)
    {
        c = getchar();
        if (c == '\n')
        {
            putchar('\n');
            break;
        }
        string[i++] = c;
        putchar(c);
    }

    string[i] = '\0';
}
int printf(char string[])
{
    int i = 0;
    while (string[i] != '\0')
    {
        putchar(string[i]);
        i = i + 1;
    }
    return i;
}

int printint(uint32 n)
{
    int characterCount = 0;
    if (n >= 10)
    {
        characterCount = printint(n / 10);
    }
    putchar('0' + (n % 10));
    characterCount++;

    return characterCount;
}

void clearscreen()
{
    for (int i = 0; i < 80 * 25 * 2; i += 2)
    {
        video_memory[i] = ' ';
        video_memory[i + 1] = 0x07;
    }
    cursorCol = 0;
    cursorRow = 0;
}