#include "chip8.h"
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <ios>
#include <iostream>
#include <string>
#include <type_traits>

chip8::chip8() {
  //Nothing to be initialize
}

chip8::~chip8() {
  //No resource require automatic cleanup
}

void chip8::initialize() {
  //Initialize memory and register once
  PC = 0x200;
  opcode = 0;
  I = 0;
  sp = 0;

  //display clear
  for (int i = 0; i < 2048; ++i) {
    gfx[i] = 0;
  }

  //clear stack]
  for (int i = 0; i < 16; ++i) {
    stack[i] = 0;
  }

  for (int i = 0; i < 16; ++i) {
    key[i] = V[i] = 0;
  }

  //clear memory
  for (int i = 0; i < 4096; i++) {
    memory[i] = 0;
  }

  //load font
  for (int i = 0; i < 80; ++i) {
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
void chip8::emulate_cycle() {
  //Fetch Opcodes
  opcode = memory[PC] << 8 | memory[PC + 1];
  //Decode Opcodes
  //In Vx,Vy Vx = (opcode & 0x0F00)>>8 and Vy = (opcode & 0x00F0)>>4
  //  x = (opcode & 0x0F00) >> 8;
  //  y = (opcode & 0x00F0) >> 4;
  //  n = (opcode & 0x000F);
  //  nn = (opcode & 0x00FF);
  //  nnn = (opcode & 0x0FFF);
  //std::cout << "Opcode [0x0000]: 0x0" << opcode << "\n";
  switch (opcode & 0xF000) {
    case 0x0000: {
      switch (opcode & 0xF00) {
        case 0x0000:  // 0x00E0: Clears the screen
          for (int i = 0; i < 2048; i++)
            gfx[i] = 0x0;
          drawFlag = true;
          PC = PC + 2;
          //std::cout << "clear" << std::endl;
          break;
        case 0x000E:  // 0x00EE: Returns from subroutine
          --sp;
          PC = stack[sp];
          PC = PC + 2;
          break;
        default:
          std::cout << "Unknown Opcode [0x0000]: 0x0" << opcode << "\n";
      }
    } break;
    case 0x1000:  //1nnn - JP addr .Jump to location nnn
      PC = opcode & 0x0F00;
      //std::cout << "jump to " << PC << std::endl;
      break;
    case 0x2000:  //2nnn - CALL addr .Call subroutine at nnn.
      stack[sp] = PC;
      ++sp;
      PC = (opcode & 0x0FFF);
      break;
    case 0x3000:  //3xkk - SE Vx, byte Skip next instruction if Vx = kk.
      if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
        PC += 4;
      else
        PC += 2;
      break;
    case 0x4000:  //4xkk - SNE Vx, byte Skip next instruction if Vx != kk.
      if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
        PC += 4;
      else
        PC += 2;
      break;
    case 0x5000:  // 5xy0 - SE Vx,Vy Skip next instruction if Vx = Vy.
      if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
        PC += 4;
      else
        PC += 2;
      break;
    case 0x6000:  //6xkk - LD Vx, byte .Set Vx = kk. The interpreter puts the value kk into register Vx.
      V[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF);
      PC += 2;
      //std::cout << "set v" << ((opcode & 0x0F00) >> 8) << " to "<< (opcode & 0x00FF) << std::endl;
      break;
    case 0x7000:  //7xkk - ADD Vx, byte.Set Vx = Vx + kk. Adds the value kk to the value of register Vx, then stores the result in Vx
      V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] + (opcode & 0x00FF);
      PC += 2;
      //std::cout << "add " << (opcode & 0x00FF) << " to v"<< ((opcode & 0x0F00) >> 8) << std::endl;
      break;
    case 0x8000:
      switch (opcode & 0x000F) {
        case 0x0000:  //8XY0 - LD Vx,Vy
          V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
          PC += 2;
          break;
        case 0x0001:  //8XY1 - OR Vx,Vy
          V[(opcode & 0x0F00) >> 8] =
              V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4];
          PC += 2;
          break;
        case 0x0002:  //8XY2 - AND Vx,Vy
          V[(opcode & 0x0F00) >> 8] =
              V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4];
          PC += 2;
          break;
        case 0x0003:  //8xy3 - XOR Vx,Vy
          V[(opcode & 0x0F00) >> 8] =
              V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4];
          PC += 2;
          break;
        case 0x0004:
          //8xy4 - ADD Vx, Vy Set Vx = Vx + Vy, set VF = carry. The values of Vx and Vy are added together.
          // If the result is greater than 8 bits(i.e., > 255, ) VF is set to 1, otherwise 0. Only the lowest 8 bits of the result are kept, and stored in Vx.
          if (V[(opcode & 0x00F0) >> 4] > (0xFF - V[(opcode & 0x0F00) >> 8]))
            V[0xF] = 1;  //carry
          else
            V[0xF] = 0;
          V[(opcode & 0x0F00) >> 8] =
              V[(opcode & 0x0F00) >> 8] + V[(opcode & 0x00F0) >> 4];
          PC += 2;
          break;
        case 0x0005:  //8XY5 - SUB Vx,Vy
          if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8])
            V[0xF] = 0;
          else
            V[0xF] = 1;
          V[(opcode & 0x0F00) >> 8] =
              V[(opcode & 0x0F00) >> 8] - V[(opcode & 0x00F0) >> 4];
          PC += 2;
          break;
        case 0x0006:
          V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
          V[(opcode & 0x0F00) >> 8] >>= 1;
          PC += 2;
          break;
        case 0x0007:
          if (V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8])
            V[0xF] = 1;
          else
            V[0xF] = 0;
          PC += 2;
          break;
        case 0x000E:
          V[0xF] = V[(opcode & 0x0F00) >> 8] >> 8;
          V[(opcode & 0x0F00) >> 8] <<= 1;
          PC += 2;
          break;
        default:
          std::cout << "Unknown opcode: 0x8" << opcode << "\n";
      }
      break;
    case 0x9000:
      if (V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
        PC += 4;
      else
        PC += 2;
      break;
    case 0xA000:  // ANNN: Sets I to the address NNN
    {
      I = (opcode & 0x0FFF);
      PC = PC + 2;
      //std::cout << "set index to " << (opcode & 0x0FFF) << std::endl;
      break;
    }
    case 0xB000:  // Bnnn - JP V0, addr Jump to location nnn + V0. The program counter is set to nnn plus the value of V0.
      PC = ((opcode & 0x0FFF) + V[0x0]);
      break;
    case 0xC000:  //Cxkk - RND Vx, byte Set Vx = random byte AND kk. The interpreter generates a random number from 0 to 255,
      // which is then ANDed with the value kk.The results are stored in Vx
      V[(opcode & 0x0F00) >> 8] =
          (0 + (std::rand() % (255 - 0 + 1))) & (opcode & 0x00FF);
      PC += 2;
      break;
    case 0xD000:  //Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision. The interpreter reads n bytes from memory,
      // starting at the address stored in I.These bytes are then displayed as sprites on screen at coordinates(Vx, Vy).
      // Sprites are XORed onto the existing screen.If this causes any pixels to be erased, VF is set to 1, otherwise it is set to 0.
      // If the sprite is positioned so part of itis outside the coordinates of the display, it wraps around to the opposite side of the screen.
      {
        unsigned short Vx = V[(opcode & 0x0F00) >> 8];
        unsigned short Vy = V[(opcode & 0x00F0) >> 4];
        unsigned short height = (opcode & 0x000F);
        unsigned short pixel;
        //std::cout << "draw (" << Vx << ", " << Vy << ")" << std::endl;
        V[0xF] = 0;
        for (int yline = 0; yline < height; yline++) {
          pixel = memory[I + yline];
          for (int xline = 0; xline < 8; xline++) {
            if ((pixel & (0x80 >> xline)) != 0) {
              if (gfx[(Vx + xline + ((Vy + yline) * 64))] != 0) {
                V[0xF] = 1;
              }
              gfx[Vx + xline + ((Vy + yline) * 64)] ^= 1;
            }
          }
        }
        drawFlag = true;
        PC += 2;
      }
      break;
    case 0xE000:  //Ex9E - SKP Vx Skip next instruction if key with the value of Vx is pressed.
      // Checks the keyboard, and if the key corresponding to the value of Vx is currently in thedown position,
      // PC is increased by 2.
      switch (opcode & 0x00FF) {
        case 0x009E:  //EX9E - SKP VX
          if (key[V[(opcode & 0x0F00) >> 8]] == 1)
            PC += 4;
          else
            PC += 2;
          break;
        case 0x00A1:  //EXA1 - SKNP VX. Skip the next instruction if the key with the value of VX is currently not pressed.
          if (key[V[(opcode & 0x0F00) >> 8]] == 0)
            PC += 4;
          else
            PC += 2;
          break;
        default:
          std::cout << "Unknown Opcode:0xE" << (opcode) << "\n";
      }
      break;
    case 0xF000:
      switch (opcode & 0x00FF) {
        case 0x0007:  //FX07 - LD VX, DT . Read the delay timer register value into VX
          V[(opcode & 0x0F00) >> 8] = delay_timer;
          PC += 2;
          break;
        case 0x000A:  // FX0A - LD VX, K.Wait for a key press, and then store the value of the key to VX
        {
          bool key_press = false;
          for (int i = 0; i < 16; ++i) {
            if (key[i] != 0)
              V[(opcode & 0x0F00) >> 8] = i;
            key_press = true;
          }
          //If we didn't have any key pressed we skip the cycle eniterly
          if (!key_press)
            return;

          PC += 2;
        } break;
        case 0x0015:  //FX15 - LD DT, VX .Load the value of VX into the delay timer DT.
          delay_timer = V[(opcode & 0x0F00) >> 8];
          PC += 2;
          break;
        case 0x0018:  //FX18 - LD ST, VX.Load the value of VX into the sound time ST
          sound_timer = V[(opcode & 0x0F00) >> 8];
          PC += 2;
          break;
        case 0x001E:  //FX1E - ADD I, VX.Add the values of I and VX, and store the result in I.
          if (I + V[(opcode & 0x0F00) >> 8] > 0xFFF)
            V[0xF] = 1;
          else
            V[0xF] = 0;
          I += V[(opcode & 0x0F00) >> 8];
          PC += 2;
          break;
        case 0x0029:  //FX29 - LD F, VX. Set the location of the sprite for the digit VX to I.
          //The font sprites start at address 0x000, and contain the hexadecimal digits from 1..F.
          //Each font has a length of 0x05 bytes. The memory address for the value in VX is put in I
          I = V[(opcode & 0x0F00) >> 8] * 0x05;
          PC += 2;
          break;
        case 0x0033:  //FX33 - LD B, VX. Store the binary-coded decimal in VX and put it in three consecutive memory slots starting at I.
          //VX is a byte, so it is in 0…255. The interpreter takes the value in VX (for example the decimal value 174, or 0xAE in hex), converts it into a decimal and separates the hundreds, the tens and the ones (1, 7 and 4 respectively).
          //Then, it stores them in three memory locations starting at I (1 to I, 7 to I+1 and 4 to I+2).
          memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
          memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
          memory[I + 2] = (V[(opcode & 0x0F00) >> 8] % 100) % 10;
          PC += 2;
          break;
        case 0x0055:  //FX55 - LD [I], VX.Store registers from V0 to VX in the main memory, starting at location I.
          //Note that X is the number of the register, so we can use it in the loop.
          for (int i = 0; i < ((opcode & 0x0F00) >> 8); ++i) {
            memory[I + i] = V[i];
          }
          I = I + ((opcode & 0x0F00) >> 8) + 1;  //I = I + x + 1
          PC += 2;
          break;
        case 0x065:  //FX65 - LD VX, [I]. Load the memory data starting at address I into the registers V0 to VX.
          for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
            V[i] = memory[I + i];
          I = I + ((opcode & 0x0F00) >> 8) + 1;  //I = I + x +1
          PC += 2;
          break;
        default:
          std::cout << "Unknown Opcode:0xF" << opcode << "\n";
      }
      break;
    default:
      std::cout << "Unknown Opcode: 0x" << opcode;
  }
  //Execute Opcodes

  //Update Timers
  if (delay_timer > 0) {
    --delay_timer;
  }
  if (sound_timer > 0) {
    if (sound_timer == 1) {
      std::cout << "BEEP\n";
    }
    --sound_timer;
  }
}

bool chip8::load_game(const std::string& file_name) {
  //initialize();
  std::ifstream input_file;
  input_file.open(file_name,
                  std::ios::binary | std::ios::in);  //Read in binary mode
  if (!input_file) {
    std::cerr << "Could not open the file.\n "
              << "Check for correct filename or file extension.";
    return false;
  }
  int i = 0;
  char b;
  while (input_file.get(b)) {
    memory[i + 512] = b;
    i++;
  }
  input_file.close();
  std::cout << "Successfully parsed CHIP8 file......\n";
  return true;
}
