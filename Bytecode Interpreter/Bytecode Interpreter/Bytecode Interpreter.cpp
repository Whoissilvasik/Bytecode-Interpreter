#include <fstream>
#include <iostream>

using namespace std;

unsigned char sm_byte(unsigned char byte)
{
    return byte & 0xF;
}
unsigned char big_byte(unsigned char byte)
{
    return byte >> 4;
}

bool byte_index(unsigned char byte, unsigned char index)
{
    return (byte & (0x1 << index)) >> index;
    //
}
bool big_bit(unsigned char byte)
{
    return byte_index(byte, 7);
}
void bit_index(unsigned char& byte, unsigned char index, bool bit)
{
    byte = byte & ~(0x1 << index) | bit << index;

}

enum class Opcode
{
    NOP, INC, DEC, MOV, MOVC, LSL, LSR, JMP, JZ, JNZ, JFE, RET, ADD, SUB, XOR, OR, IN, OUT, JO
};
enum class OpcodeFlag
{
    Zero, EndOfFile, Overflow
};

bool Bitflag(unsigned char byte, OpcodeFlag flag)
{
    return byte_index(byte, static_cast<unsigned char>(flag));
}

void Set_Flag(unsigned char& byte, OpcodeFlag flag, bool bit)
{
    return bit_index(byte, static_cast<unsigned char>(flag), bit);
}



int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        cout << "Per mazai arguemntu";
        return 0;
    }

    ifstream file(argv[1], ios::binary);
    if (!file.is_open())
    {
        cout << "Nepavyko atidaryti failo";
        return 0;
    }

    char prog_mem[256];

    for (size_t i = 0; file.get(prog_mem[i]); ++i)
    {
        if (i == 255 && !file.eof())
        {
            cout << "ERROR";
            return 0;
        }

    }

    file.close();
    file.open(argv[2]);

    unsigned char bytesNumber = 0;
    unsigned char registerFlag = 0;
    unsigned char registersGp[16] = {};

    do switch (static_cast<Opcode>(prog_mem[bytesNumber++]))
    {

    case Opcode::INC:

    {
        ++registersGp[sm_byte(prog_mem[bytesNumber])];

        Set_Flag(registerFlag, OpcodeFlag::Zero, !registersGp[sm_byte(prog_mem[bytesNumber])]);
        Set_Flag(registerFlag, OpcodeFlag::Overflow, big_bit(registersGp[sm_byte(prog_mem[bytesNumber])] - 1) != big_bit(registersGp[sm_byte(prog_mem[bytesNumber])]));

        break;
    }

    case Opcode::DEC:
    {
        --registersGp[sm_byte(prog_mem[bytesNumber])];

        Set_Flag(registerFlag, OpcodeFlag::Zero, !registersGp[sm_byte(prog_mem[bytesNumber])]);
        Set_Flag(registerFlag, OpcodeFlag::Overflow, big_bit(registersGp[sm_byte(prog_mem[bytesNumber])] + 1)
            != big_bit(registersGp[sm_byte(prog_mem[bytesNumber])]));

        break;
    }

    case Opcode::MOV:
    {
        registersGp[sm_byte(prog_mem[bytesNumber])] = registersGp[big_byte(prog_mem[bytesNumber])];
        break;
    }

    case Opcode::MOVC:
    {
        registersGp[0] = prog_mem[bytesNumber];
        break;
    }

    case Opcode::LSL:
    {
        registersGp[sm_byte(prog_mem[bytesNumber])] <<= 1;
        break;
    }

    case Opcode::LSR:
    {
        registersGp[sm_byte(prog_mem[bytesNumber])] >>= 1;
        break;
    }

    case Opcode::JMP:
    {
        bytesNumber += prog_mem[bytesNumber] - 2;
        continue;
    }

    case Opcode::JZ:
    {
        if (Bitflag(registerFlag, OpcodeFlag::Zero))
            bytesNumber += prog_mem[bytesNumber] - 2;

        break;
    }

    case Opcode::JNZ:
    {
        if (!Bitflag(registerFlag, OpcodeFlag::Zero))
            bytesNumber += prog_mem[bytesNumber] - 2;

        break;
    }

    case Opcode::JFE:
    {
        if (Bitflag(registerFlag, OpcodeFlag::EndOfFile))
            bytesNumber += prog_mem[bytesNumber] - 2;

        break;
    }

    case Opcode::RET:
    {
        return 0;
    }

    case Opcode::ADD:
    {
        registersGp[sm_byte(prog_mem[bytesNumber])] += registersGp[big_byte(prog_mem[bytesNumber])];

        Set_Flag(registerFlag, OpcodeFlag::Zero, !registersGp[sm_byte(prog_mem[bytesNumber])]);


        break;
    }

    case Opcode::SUB:
    {
        registersGp[sm_byte(prog_mem[bytesNumber])] -= registersGp[big_byte(prog_mem[bytesNumber])];
        Set_Flag(registerFlag, OpcodeFlag::Zero, !registersGp[sm_byte(prog_mem[bytesNumber])]);


        break;
    }

    case Opcode::XOR:
    {
        registersGp[sm_byte(prog_mem[bytesNumber])] ^= registersGp[big_byte(prog_mem[bytesNumber])];
        Set_Flag(registerFlag, OpcodeFlag::Zero, !registersGp[sm_byte(prog_mem[bytesNumber])]);

        break;
    }

    case Opcode::OR:
    {
        registersGp[sm_byte(prog_mem[bytesNumber])] |= registersGp[big_byte(prog_mem[bytesNumber])];
        Set_Flag(registerFlag, OpcodeFlag::Zero, !registersGp[sm_byte(prog_mem[bytesNumber])]);

        break;
    }

    case Opcode::IN:
    {
        registersGp[prog_mem[bytesNumber]] = file.get();

        if (file.eof())
        {
            Set_Flag(registerFlag, OpcodeFlag::EndOfFile, true);
        }

        else if (file.fail())
        {
            cout << "ERROR";
            return 0;
        }
        break;
    }

    case Opcode::OUT:
    {
        cout << registersGp[sm_byte(prog_mem[bytesNumber])];
        break;
    }

    case Opcode::JO:
    {
        if (Bitflag(registerFlag, OpcodeFlag::Overflow))
            bytesNumber += prog_mem[bytesNumber] - 2;
        break;
    }

    }
    while (bytesNumber++);

    file.close();
    return 0;
}