#include "chip8.h"
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <ios>
#include <iostream>
#include <string>
#include <type_traits>

chip8::chip8()
{
    //Nothing to be initialize
}

chip8::~chip8()
{
    //No resource require automatic cleanup
}

void chip8::initialize()
{
    //Initialize memory and register once
    PC = 0x200;
    opcode = 0;
    I = 0;
    sp = 0;

    //display clear
    for (int i = 0; i < 2048; ++i)
    {
        gfx[i] = 0;
    }

    //clear stack]
    for (int i = 0; i < 16; ++i)
    {
        stack[i] = 0;
    }

    for (int i = 0; i < 16; ++i)
    {
        key[i] = V[i] = 0;
    }

    //clear memory
    for (int i = 0; i < 4096; i++)
    {
        memory[i] = 0;
    }

    //load font
    for (int i = 0; i < 80; ++i)
    {
        memory[i] = chip8_font[i];
    }

    //Reset Timers
    delay_timer = 0;
    sound_timer = 0;

    //Clear Screen once
    drawFlag = true;

    srand(time(NULL));
}

//Chip8 Opcode details
//From: http://devernay.free.fr/hacks/chip8/C8TECH10.HTM This has a lot of good information
void chip8::emulate_cycle()
{
    //Fetch Opcodes
    opcode = memory[PC] << 8 | memory[PC + 1];
    //Decode Opcodes
    //In Vx,Vy x = (opcode & 0x0F00)>>8 and y = (opcode & 0x00F0)>>4
    unsigned short x = (opcode & 0x0F00) >> 8;
    unsigned short y = (opcode & 0x00F0) >> 4;
    unsigned short n = (opcode & 0x000F);
    unsigned short nn = (opcode & 0x00FF);
    unsigned short nnn = (opcode & 0x0FFF);
    switch (opcode & 0xF000)
    {
    case 0x0000:
    {
        switch (opcode & 0xF00)
        {
        case 0x0000: // 0x00E0: Clears the screen
            for (int i = 0; i < 2048; i++)
                gfx[i] = 0x0;
            drawFlag = true;
            PC = PC + 2;
            break;
        case 0x000E: // 0x00EE: Returns from subroutine
            --sp;
            PC = stack[sp];
            PC = PC + 2;
            break;
        default:
            std::cout << "Unknown Opcode [0x0000]: 0x" << opcode << "\n";
        }
    }
    break;
    case 0x1000: //1nnn - JP addr .Jump to location nnn
        PC = opcode & 0x0F00;
        PC += 2;
        break;
    case 0x2000: //2nnn - CALL addr .Call subroutine at nnn.
        stack[sp] = PC;
        ++sp;
        PC = opcode & 0x0FFF;
        break;
    case 0x3000: //3xkk - SE Vx, byte Skip next instruction if Vx = kk.
        if (V[x] == nn)
            PC += 4;
        else
            PC += 2;
        break;
    case 0x4000: //4xkk - SNE Vx, byte Skip next instruction if Vx != kk.
        if (V[x] != nn)
            PC += 4;
        else
            PC += 2;
        break;
    case 0x5000: // 5xy0 - SE Vx,Vy Skip next instruction if Vx = Vy.
        if (V[x] == V[y])
            PC += 4;
        else
            PC += 2;
        break;
    case 0x6000: //Set Vx = kk. The interpreter puts the value kk into register Vx.
        V[x] = (opcode & 0x00FF);
        PC += 2;
        break;
    case 0x7000: //Set Vx = Vx + kk. Adds the value kk to the value of register Vx, then stores the result in Vx
        V[x] = V[x] + nn;
        PC += 2;
        break;
    case 0x8000:
        switch (opcode & 0x000F)
        {
        case 0x0000: //8XY0 - LD Vx,Vy
            V[x] = V[y];
            PC += 2;
            break;
        case 0x0001: //8XY1 - OR Vx,Vy
            V[x] = V[x] | V[y];
            PC += 2;
            break;
        case 0x0002: //8XY2 - AND Vx,Vy
            V[x] = V[x] & V[y];
            PC += 2;
            break;
        case 0x0003: //8xy3 - XOR Vx,Vy
            V[x] = V[x] ^ V[y];
            PC += 2;
            break;
        case 0x0004:
            //8xy4 - ADD Vx, Vy Set Vx = Vx + Vy, set VF = carry. The values of Vx and Vy are added together.
            // If the result is greater than 8 bits(i.e., > 255, ) VF is set to 1, otherwise 0. Only the lowest 8 bits of the result are kept, and stored in Vx.
            if (V[y] > (0xFF - V[x]))
                V[0xF] = 1; //carry
            else
                V[0xF] = 0;
            V[x] = V[x] + V[y];
            PC += 2;
            break;
        case 0x0005: //8XY5 - SUB Vx,Vy
            if (V[y] > V[x])
                V[0xF] = 0;
            else
                V[0xF] = 1;
            V[x] = V[x] - V[y];
            PC += 2;
            break;
        case 0x0006:
            break;
        case 0x0007:
            break;
        case 0x000E:
            break;
        default:
            std::cout << "Unknown opcode: 0x" << opcode << "\n";
        }
        break;
    case 0x9000:
        break;
    case 0xA000: // ANNN: Sets I to the address NNN
    {
        I = opcode & 0x0FFF;
        PC = PC + 2;
        break;
    }
    case 0xB000: // Bnnn - JP V0, addr Jump to location nnn + V0. The program counter is set to nnn plus the value of V0.
        break;
    case 0xC000: //Cxkk - RND Vx, byte Set Vx = random byte AND kk. The interpreter generates a random number from 0 to 255,
        // which is then ANDed with the value kk.The results are stored in Vx
        break;
    case 0xD000: //Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision. The interpreter reads n bytes from memory,
        // starting at the address stored in I.These bytes are then displayed as sprites on screen at coordinates(Vx, Vy).
        // Sprites are XORed onto the existing screen.If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0.
        // If the sprite is positioned so part of itis outside the coordinates of the display, it wraps around to the opposite side of the screen.
        {
            unsigned short height = (opcode & 0x000F);
            unsigned short pixel;

            V[0xF] = 0;
            for (int yline = 0; yline < height; yline++)
            {
                pixel = memory[I + yline];
                for (int xline = 0; xline < 8; xline++)
                {
                    if ((pixel & (0x80 >> xline)) != 0)
                    {
                        if (gfx[(x + xline + ((y + yline) * 64))] != 0)
                        {
                            V[0xF] = 1;
                        }
                        gfx[x + xline + ((y + yline) * 64)] ^= 1;
                    }
                }
            }
            drawFlag = true;
            PC += 2;
        }
        break;
    case 0xE000: //Ex9E - SKP Vx Skip next instruction if key with the value of Vx is pressed.
        // Checks the keyboard, and if the key corresponding to the value of Vx is currently in thedown position,
        // PC is increased by 2.
        switch (opcode & 0x00FF)
        {
        case 0x0001:
            break;
        case 0x000E:
            break;
        default:
            std::cout << "Unknown Opcode:0x" << (opcode & 0x0F000) << "\n";
        }
        break;
    case 0xF000:
        switch (opcode & 0x00FF)
        {
        case 0x0007:
            break;
        case 0x000A:
            break;
        case 0x0015:
            break;
        case 0x0018:
            break;
        case 0x001E:
            break;
        case 0x0029:
            break;
        case 0x0033:
            memory[I] = V[x] / 100;
            memory[I + 1] = (V[x] / 10) % 10;
            memory[I + 2] = (V[x] % 100) % 10;
            PC += 2;
            break;
        case 0x0055:
            break;
        case 0x065:
            break;
        default:
            std::cout << "Unknown Opcode:0x" << (opcode & 0xF000) << "\n";
        }
        break;
    default:
        std::cout << "Unknown Opcode: 0x" << opcode;
    }
    //Execute Opcodes

    //Update Timers
    if (delay_timer > 0)
    {
        --delay_timer;
    }
    if (sound_timer > 0)
    {
        if (sound_timer == 1)
        {
            std::cout << "BEEP\n";
        }
        --sound_timer;
    }
}

void chip8::load_game(const std::string &file_name)
{
    initialize();
    std::ifstream input_file;
    input_file.open(file_name, std::ios::binary); //Read in binary mode
    if (!input_file)
    {
        std::cerr << "Cannot open file for CHIP8 Emulator.\n";
    }
    int i = 0;
    char b;
    while (input_file.get(b))
    {
        memory[i + 512] = b;
        i++;
    }
    input_file.close();
    std::cout << "Successfully parsed CHIP8 file......\n";
}
