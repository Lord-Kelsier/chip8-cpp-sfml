#pragma once
#include <iostream>
#include <string>

enum IfCond {
    equ, neq
};

class CPU {
public:
    unsigned short int opcode; // 2bytes opcode
    unsigned char memory[4096]; // 4096 bytes memory
    /* 0x000-0x1FF font set, intepreter
    (inside) 0x050-0x0A0 for the 4x5 pixel font set
    0x200-0xFFF rom y ram
    */
    unsigned char V[16]; // 15 1 byte resgisters
    unsigned short I;
    unsigned short pc; // program counter
    unsigned char gfx[64 * 32 / 8]; // screen 
    unsigned char delay_timer;
    unsigned char sound_timer;
    /*timers que cuentan a 60 Hz, cuando estan por encima de 0
    hacen una cuenta regresiva a 0*/
    unsigned short int stack[16]; // 16 leves of stack
    unsigned short int sp; // stack pointer
    unsigned char key[16]; // key pad (0x0-0xF)
    bool drawFlag; /*
    Because the system does not draw every cycle, we should set a draw flag when we need to update our screen. Only two opcodes should set this flag:
        0x00E0 – Clears the screen
        0xDXYN – Draws a sprite on the screen*/
public:
    void drawSprite();
    void initialize(); // Clear the memory, registers and screen
    void loadGame(std::string game); // Copy the program into the memory
    void emulateCycle(); // Emulate one cycle of the system
    void setKeys(); // If we press or release a key, we should store this state in the part that emulates the keypad
private:
    void decodeOpcode(unsigned short first, unsigned short second, unsigned short third, unsigned short fourth);
    void returnSubroutine();
    void gotoDir(unsigned short direction);
    void callSubRoutine(unsigned short direction);
    void ifJump(unsigned short value1, unsigned short value2, IfCond conditional);
};