#include "CPU.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

const int CHIP8_FONT_SET[80] = {
    0xf0, 0x90, 0x90, 0x90, 0xf0,
    0x20, 0x60, 0x20, 0x20, 0x70,

    0xf0, 0x10, 0xf0, 0x80, 0xf0,
    0xf0, 0x10, 0xf0, 0x10, 0xf0,

    0x90, 0x90, 0xf0, 0x10, 0x10,
    0xf0, 0x80, 0xf0, 0x10, 0xf0,

    0xf0, 0x80, 0xf0, 0x90, 0xf0,
    0x10, 0x20, 0x40, 0x40, 0x40,

    0xf0, 0x90, 0xf0, 0x90, 0xf0,
    0xf0, 0x90, 0xf0, 0x10, 0xf0,

    0xf0, 0x90, 0xf0, 0x90, 0x90,
    0xe0, 0x90, 0xe0, 0x90, 0xe0,

    0xf0, 0x80, 0x80, 0x80, 0xf0,
    0xe0, 0x90, 0x90, 0x90, 0xe0,

    0xf0, 0x80, 0xf0, 0x80, 0xf0,
    0xf0, 0x80, 0xf0, 0x80, 0x80
};

void CPU::initialize() {
    // initialize register and memory once
    pc = 0x200;
    opcode = 0;
    I = 0;
    sp = 0;
    clearScreenFlag = false;
    drawFlag = false;
    for (int i = 0; i < 16; i++) {
        V[i] = 0;
        stack[i] = 0;
    }
    for (int i = 0; i < 4096; i++) {
        memory[i] = 0;
    }
    for (int i = 0; i < 64 * 32; i++) {
        gfx[i] = 0;
    }
    // Load fontset
    for (int i = 0; i < 80; ++i)
        memory[i] = CHIP8_FONT_SET[i];
    /*// Clear display


    // Reset timers*/
    delay_timer = 0;
    sound_timer = 0;
    return;
}

unsigned short int groupThree(unsigned short first, unsigned short second, unsigned short third) {
    return (first << 8) | (second << 4) | third;
}

unsigned short int groupTwo(unsigned short first, unsigned short second) {
    return (first << 4) | second;
}

void CPU::returnSubroutine() {
    sp--;
    unsigned short int pointer = stack[sp];
    pc = pointer;
}

void CPU::gotoDir(unsigned short direction) {
    pc = direction;
}

void CPU::callSubRoutine(unsigned short direction) {
    stack[sp] = pc;
    sp++;
    pc = direction;
}

void CPU::ifJump(unsigned short value1, unsigned short value2, IfCond conditional) {
    switch (conditional)
    {
    case equ:
        if (value1 == value2) {
            pc += 2;
        }
        break;
    case neq:
        if (value1 != value2) {
            pc += 2;
        }
        break;
    default:
        break;
    }
}

void CPU::decodeOpcode(unsigned short first, unsigned short second, unsigned short third, unsigned short fourth) {
    if (first == 0x0) {
        unsigned short remainder = groupThree(second, third, fourth);
        if (remainder == 0x0E0) {
            clearScreenFlag = true;
        }
        else if (remainder == 0x0EE) {
            returnSubroutine();
        }
        else {
            // Calls machine code routine (RCA 1802 for COSMAC VIP) at address NNN. Not necessary for most ROMs.[22]
            std::cout << "deprecated 0NNN code executed" << std::endl;
        }
    }
    else if (first == 0x1) {
        unsigned short remainder = groupThree(second, third, fourth);
        gotoDir(remainder);
    }
    else if (first == 0x2) {
        unsigned short remainder = groupThree(second, third, fourth);
        callSubRoutine(remainder);
    }
    else if (first == 0x3) {
        unsigned short remainder = groupTwo(third, fourth);
        ifJump(V[second], remainder, equ);
    }
    else if (first == 0x4) {
        unsigned short remainder = groupTwo(third, fourth);
        ifJump(V[second], remainder, neq);
    }
    else if (first == 0x5) {
        ifJump(V[second], V[third], equ);
    }
    else if (first == 0x6) {
        V[second] = (third << 4 | fourth) & 0xFF;
    }
    else if (first == 0x7) {
        V[second] += (third << 4 | fourth) & 0xFF;
    }
    else if (first == 0x8) {
        if (fourth == 0x0) {
            V[second] = V[third];
        }
        else if (fourth == 0x1) {
            V[second] |= V[third];
        }
        else if (fourth == 0x2) {
            V[second] &= V[third];
        }
        else if (fourth == 0x3) {
            V[second] ^= V[third];
        }
        else if (fourth == 0x4) {
            if (V[second] + V[third] > 0xFF) {
                V[0xF] = 1;
                V[second] = (V[second] + V[third]) & 0xFF;
            }
            else {
                V[0xF] = 0;
                V[second] += V[third];
            }
        }
        else if (fourth == 0x5) {
            if (V[second] < V[third]) {
                V[0xF] = 0;
                V[second] = V[third] - V[second];
            }
            else {
                V[0xF] = 1;
                V[second] -= V[third];
            }
        }
        else if (fourth == 0x6) {
            V[0xF] = 0b1 & V[second];
            V[second] >>= 1;
        }
        else if (fourth == 0x7) {
            if (V[second] < V[third]) {
                V[0xF] = 1;
                V[second] = V[third] - V[second];
            }
            else {
                V[0xF] = 0;
                V[second] = V[second] - V[third];
            }
        }
        else if (fourth == 0xE) {
            V[0xF] = (0b1 << 7) & V[second];
            V[second] = (V[second] << 1) & 0xFF;
        }
    }
    else if (first == 0x9) {
        ifJump(V[second], V[third], neq);
    }
    else if (first == 0xA) {
        unsigned short remainder = groupThree(second, third, fourth);
        I = remainder;
    }
    else if (first == 0xB) {
        unsigned short remainder = groupThree(second, third, fourth);
        pc = V[0x0] + remainder;
    }
    else if (first == 0xC) {
        unsigned short remainder = groupTwo(third, fourth);
        V[second] = (rand() & remainder) & 0xFF;
    }
    else if (first == 0xD) {
        // Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N pixels. Each row of 8 pixels is read as bit-coded starting from memory location I; I value does not change after the execution of this instruction. As described above, VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that does not happen.[22]
        drawSprite(V[second], V[third], fourth);
    }
    else if (first == 0xE) {
        if (third == 0x9 && fourth == 0xE) {
            // Skips the next instruction if the key stored in VX is pressed (usually the next instruction is a jump to skip a code block).[22]
            unsigned char keyCheck = key[V[second]];
            if (keyCheck != 0) {
                pc += 2;
            }
        }
        else if (third == 0xA && fourth == 0x1) {
            //Skips the next instruction if the key stored in VX is not pressed (usually the next instruction is a jump to skip a code block).[22]
            unsigned char keyCheck = key[V[second]];
            if (keyCheck == 0) {
                pc += 2;
            }
        }
    }
    else if (first == 0xF) {
        unsigned short lastTwo = groupTwo(third, fourth);
        if (lastTwo == 0x07) {
            V[second] = delay_timer;
        }
        else if (lastTwo == 0x0A) {
            // A key press is awaited, and then stored in VX (blocking operation, all instruction halted until next key event).[22]
        }
        else if (lastTwo == 0x15) {
            delay_timer = V[second];
        }
        else if (lastTwo == 0x18) {
            sound_timer = V[second];
        }
        else if (lastTwo == 0x1E) {
            I += V[second];
        }
        else if (lastTwo == 0x29) {
            // Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.[22]
            unsigned short value = V[second];
            I = value * 5;
        }
        else if (lastTwo == 0x33) {
            // Stores the binary-coded decimal representation of VX, with the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.[22]
            unsigned short value = V[second];
            unsigned char hund = value / 100;
            value %= 100;
            unsigned char dec = value / 10;
            value %= 10;
            unsigned char uni = value;
            memory[I] = hund;
            memory[I + 1] = dec;
            memory[I + 2] = uni;
        }
        else if (lastTwo == 0x55) {
            // Stores from V0 to VX (including VX) in memory, starting at address I. The offset from I is increased by 1 for each value written, but I itself is left unmodified.[d][22]
            for (int i = 0; i < 0x10; i++) {
                memory[I + i] = V[i];
            }
        }
        else if (lastTwo == 0x65) {
            // Fills from V0 to VX (including VX) with values from memory, starting at address I. The offset from I is increased by 1 for each value read, but I itself is left unmodified.[d][22]
            for (int i = 0; i < 0x10; i++) {
                V[i] = memory[I + i];
            }
        }
    }
    return;
}

void CPU::loadGame(std::string game) {
    std::ifstream file(game, std::ios::binary);
    if (!file) {
        std::cerr << "Unable to open file.\n";
        exit(1);
    }
    // Read the file byte by byte and store each byte in a vector
    std::vector<unsigned char> byteArray;
    unsigned char byte;
    int offset = 0x200;
    while (file.get(reinterpret_cast<char&>(byte))) {
        std::cout << "Load to: " << offset << std::endl;
        memory[offset] = byte;
        offset++;
    }
    std::cout << "End Loading" << std::endl;
    file.close();
    return;
}
void CPU::drawSprite(unsigned short x, unsigned short y, unsigned short N) {
    drawFlag = true;
    int startPos = y * 64 + x;
    V[0xF] = 0;
    for (int row = 0; row < N; row++) {
        unsigned char byte = memory[I + row];
        for (int bitPos = 0; bitPos < 8; bitPos++) {
            unsigned short offset = 0x80 >> bitPos;
            if ((byte & offset) != 0) {
                unsigned short newPos = startPos + bitPos + (row * 64);
                if (gfx[newPos] == 1) {
                    V[0xF] = 1;
                }
                gfx[newPos] ^= 1;
            }
        }
    }
}

void CPU::emulateCycle() {
    // Fetch Opcode
    clearScreenFlag = false;
    drawFlag = false;
    opcode = memory[pc] << 8 | memory[pc + 1];
    std::cout << "Exec opcode: " << std::hex << opcode << std::endl;
    unsigned short int
        firstHalfByte,
        secodnHalfByte,
        thirdHalfByte,
        fourthHalfByte;
    firstHalfByte = opcode >> 12;
    secodnHalfByte = (opcode & 0x0F00) >> 8;
    thirdHalfByte = (opcode & 0x00F0) >> 4;
    fourthHalfByte = opcode & 0x000F;
    decodeOpcode(firstHalfByte, secodnHalfByte, thirdHalfByte, fourthHalfByte);
    pc += 2;
    // Update timers
    if (delay_timer > 0) {
        delay_timer--;
    }
    if (sound_timer > 0) {
        sound_timer--;
    }
    return;
}

