#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define ROM_SIZE 32768
#define RAM_SIZE 65536
#define OK 0
#define ERROR 1

// rom check helper
static inline bool can_read(size_t pc, size_t n) {
    return pc + n <= ROM_SIZE;
}

// ensure read is within bounds
#define CHECK_ROM(n) do { \
    if (!can_read(cpu.PC, (n))) { \
        fprintf(stderr, "Truncated instruction at PC=%zu\n", (size_t)cpu.PC); \
        return EXIT_FAILURE; \
    } \
} while (0)

// ensure read is within bounds
#define CHECK_RAM(addr) do { \
    if ((addr) >= RAM_SIZE) { \
        fprintf(stderr, "RAM out of bounds: 0x%04X at PC=%zu\n", (unsigned)(addr), (size_t)cpu.PC); \
        return EXIT_FAILURE; \
    } \
} while (0)

uint8_t rom[ROM_SIZE];
uint8_t ram[RAM_SIZE];

typedef struct {
    uint16_t A, B, C, D;
    size_t PC; // unsigned and large capacity
    bool Z; // zero flag
} cpu_state;

int load_rom(const char* filename) {
    memset(rom, 0, ROM_SIZE);
    FILE* file = fopen(filename, "rb");
    if (!file) {
        perror("Couldn't open ROM file.");
        return ERROR;
    }

    size_t bytes_read = fread(rom, 1, ROM_SIZE, file);
    fclose(file);

    printf("Loaded %zu bytes\n", bytes_read);
    return OK;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <romfile>\n", argv[0]);
        return EXIT_FAILURE; // expands to 1
    }

    if (load_rom(argv[1]) != OK) {
        fprintf(stderr, "Error loading ROM.\n");
        return EXIT_FAILURE;
    }

    cpu_state cpu;
    cpu.A = 0;
    cpu.B = 0;
    cpu.C = 0;
    cpu.D = 0;
    cpu.PC = 0;
    cpu.Z = false;

    short instr_len = 0;
    bool running = true;

    while (cpu.PC < ROM_SIZE && running) {
        uint8_t opcode = rom[cpu.PC];

        switch (opcode) {
        case 0x00: { // ADD A, IMM8
            CHECK_ROM(2);
            cpu.A += rom[cpu.PC + 1];
            instr_len = 2;
            break;
        }
        case 0x01: { // ADD B, IMM8
            CHECK_ROM(2);
            cpu.B += rom[cpu.PC + 1];
            instr_len = 2;
            break;
        }
        case 0x02: { // ADD C, IMM8
            CHECK_ROM(2);
            cpu.C += rom[cpu.PC + 1];
            instr_len = 2;
            break;
        }
        case 0x03: { // ADD D, IMM8
            CHECK_ROM(2);
            cpu.D += rom[cpu.PC + 1];
            instr_len = 2;
            break;
        }
        case 0x04: { // SUB A, IMM8
            CHECK_ROM(2);
            cpu.A -= rom[cpu.PC + 1];
            instr_len = 2;
            break;
        }
        case 0x05: { // SUB B, IMM8
            CHECK_ROM(2);
            cpu.B -= rom[cpu.PC + 1];
            instr_len = 2;
            break;
        }
        case 0x06: { // SUB C, IMM8
            CHECK_ROM(2);
            cpu.C -= rom[cpu.PC + 1];
            instr_len = 2;
            break;
        }
        case 0x07: { // SUB D, IMM8
            CHECK_ROM(2);
            cpu.D -= rom[cpu.PC + 1];
            instr_len = 2;
            break;
        }
        case 0x08: { // INC A
            cpu.A += 1;
            instr_len = 1;
            break;
        }
        case 0x09: { // INC B
            cpu.B += 1;
            instr_len = 1;
            break;
        }
        case 0x0A: { // INC C
            cpu.C += 1;
            instr_len = 1;
            break;
        }
        case 0x0B: { // INC D
            cpu.D += 1;
            instr_len = 1;
            break;
        }
        case 0x0C: { // DEC A
            cpu.A -= 1;
            instr_len = 1;
            break;
        }
        case 0x0D: { // DEC B
            cpu.B -= 1;
            instr_len = 1;
            break;
        }
        case 0x0E: { // DEC C
            cpu.C -= 1;
            instr_len = 1;
            break;
        }
        case 0x0F: { // DEC D
            cpu.D -= 1;
            instr_len = 1;
            break;
        }
        case 0x10: { // MOV A, IMM8
            CHECK_ROM(2);
            cpu.A = rom[cpu.PC + 1];
            instr_len = 2;
            break;
        }
        case 0x11: { // MOV B, IMM8
            CHECK_ROM(2);
            cpu.B = rom[cpu.PC + 1];
            instr_len = 2;
            break;
        }
        case 0x12: { // MOV C, IMM8
            CHECK_ROM(2);
            cpu.C = rom[cpu.PC + 1];
            instr_len = 2;
            break;
        }
        case 0x13: { // MOV D, IMM8
            CHECK_ROM(2);
            cpu.D = rom[cpu.PC + 1];
            instr_len = 2;
            break;
        }
        case 0x14: { // JMP IMM16
            CHECK_ROM(3);
            uint16_t addr = rom[cpu.PC + 1] | (rom[cpu.PC + 2] << 8);
            cpu.PC = addr;
            continue; // skip PC increment entirely
        }
        case 0x15: { // ADD A, IMM16
            CHECK_ROM(3);
            cpu.A += rom[cpu.PC + 1] | (rom[cpu.PC + 2] << 8);
            instr_len = 3;
            break;
        }
        case 0x16: { // ADD B, IMM16
            CHECK_ROM(3);
            cpu.B += rom[cpu.PC + 1] | (rom[cpu.PC + 2] << 8);
            instr_len = 3;
            break;
        }
        case 0x17: { // ADD C, IMM16
            CHECK_ROM(3);
            cpu.C += rom[cpu.PC + 1] | (rom[cpu.PC + 2] << 8);
            instr_len = 3;
            break;
        }
        case 0x18: { // ADD D, IMM16
            CHECK_ROM(3);
            cpu.D += rom[cpu.PC + 1] | (rom[cpu.PC + 2] << 8);
            instr_len = 3;
            break;
        }
        case 0x19: { // SUB A, IMM16
            CHECK_ROM(3);
            cpu.A -= rom[cpu.PC + 1] | (rom[cpu.PC + 2] << 8);
            instr_len = 3;
            break;
        }
        case 0x1A: { // SUB B, IMM16
            CHECK_ROM(3);
            cpu.B -= rom[cpu.PC + 1] | (rom[cpu.PC + 2] << 8);
            instr_len = 3;
            break;
        }
        case 0x1B: { // SUB C, IMM16
            CHECK_ROM(3);
            cpu.C -= rom[cpu.PC + 1] | (rom[cpu.PC + 2] << 8);
            instr_len = 3;
            break;
        }
        case 0x1C: { // SUB D, IMM16
            CHECK_ROM(3);
            cpu.D -= rom[cpu.PC + 1] | (rom[cpu.PC + 2] << 8);
            instr_len = 3;
            break;
        }
        case 0x1D: { // MOV A, IMM16
            CHECK_ROM(3);
            cpu.A = rom[cpu.PC + 1] | (rom[cpu.PC + 2] << 8);
            instr_len = 3;
            break;
        }
        case 0x1E: { // MOV B, IMM16
            CHECK_ROM(3);
            cpu.B = rom[cpu.PC + 1] | (rom[cpu.PC + 2] << 8);
            instr_len = 3;
            break;
        }
        case 0x1F: { // MOV C, IMM16
            CHECK_ROM(3);
            cpu.C = rom[cpu.PC + 1] | (rom[cpu.PC + 2] << 8);
            instr_len = 3;
            break;
        }
        case 0x20: { // MOV D, IMM16
            CHECK_ROM(3);
            cpu.D = rom[cpu.PC + 1] | (rom[cpu.PC + 2] << 8);
            instr_len = 3;
            break;
        }
        case 0x21: { // CMP A, IMM16
            CHECK_ROM(3);
            if (cpu.A == (rom[cpu.PC + 1] | (rom[cpu.PC + 2] << 8))) {
                cpu.Z = true;
            }
            else {
                cpu.Z = false;
            }
            instr_len = 3;
            break;
        }
        case 0x22: { // JZ IMM16
            CHECK_ROM(3);
            if (cpu.Z) {
                cpu.PC = rom[cpu.PC + 1] | (rom[cpu.PC + 2] << 8);
                continue; // skip PC += instr_len
            }
            instr_len = 3;
            break;
        }
        case 0x23: { // JNZ IMM16
            CHECK_ROM(3);
            if (!cpu.Z) {
                cpu.PC = rom[cpu.PC + 1] | (rom[cpu.PC + 2] << 8);
                continue;
            }
            instr_len = 3;
            break;
        }
        case 0x24: { // LOAD A, [IMM16]
            CHECK_ROM(3);
            uint16_t addr = rom[cpu.PC + 1] | (rom[cpu.PC + 2] << 8);
            CHECK_RAM(addr);
            cpu.A = ram[addr];
            instr_len = 3;
            break;
        }
        case 0x25: { // LOAD B, [IMM16]
            CHECK_ROM(3);
            uint16_t addr = rom[cpu.PC + 1] | (rom[cpu.PC + 2] << 8);
            CHECK_RAM(addr);
            cpu.B = ram[addr];
            instr_len = 3;
            break;
        }
        case 0x26: { // LOAD C, [IMM16]
            CHECK_ROM(3);
            uint16_t addr = rom[cpu.PC + 1] | (rom[cpu.PC + 2] << 8);
            CHECK_RAM(addr);
            cpu.C = ram[addr];
            instr_len = 3;
            break;
        }
        case 0x27: { // LOAD D, [IMM16]
            CHECK_ROM(3);
            uint16_t addr = rom[cpu.PC + 1] | (rom[cpu.PC + 2] << 8);
            CHECK_RAM(addr);
            cpu.D = ram[addr];
            instr_len = 3;
            break;
        }
        case 0x28: { // STORE A, [IMM16]
            CHECK_ROM(3);
            uint16_t addr = rom[cpu.PC + 1] | (rom[cpu.PC + 2] << 8);
            CHECK_RAM(addr);
            ram[addr] = cpu.A & 0xFF;
            instr_len = 3;
            break;
        }
        case 0x29: { // STORE B, [IMM16]
            CHECK_ROM(3);
            uint16_t addr = rom[cpu.PC + 1] | (rom[cpu.PC + 2] << 8);
            CHECK_RAM(addr);
            ram[addr] = cpu.B & 0xFF;
            instr_len = 3;
            break;
        }
        case 0x2A: { // STORE C, [IMM16]
            CHECK_ROM(3);
            uint16_t addr = rom[cpu.PC + 1] | (rom[cpu.PC + 2] << 8);
            CHECK_RAM(addr);
            ram[addr] = cpu.C & 0xFF;
            instr_len = 3;
            break;
        }
        case 0x2B: { // STORE D, [IMM16]
            CHECK_ROM(3);
            uint16_t addr = rom[cpu.PC + 1] | (rom[cpu.PC + 2] << 8);
            CHECK_RAM(addr);
            ram[addr] = cpu.D & 0xFF;
            instr_len = 3;
            break;
        }
        case 0x2C: { // PRINT A AS ASCII
            putchar(cpu.A & 0xFF);
            instr_len = 1;
            break;
        }
        case 0x2D: { // IN A
            int c = getchar();
            /*
             * Convert EOF (-1) to 0 to prevent passing invalid data to the CPU.
             * This allows input loops to treat 0x00 as end-of-input.
             */
            if (c == EOF) c = 0;
            cpu.A = c & 0xFF;
            instr_len = 1;
            break;
        }
        case 0x2E: { // PRINT A AS DECIMAL
            printf("%u", cpu.A);
            instr_len = 1;
            break;
        }
        case 0x2F: { // PRINT A AS BITS
            for (int i = 7; i >= 0; i--)
                putchar((cpu.A & (1 << i)) ? '1' : '0');
            putchar('\n');
            instr_len = 1;
            break;
        }
        case 0x30: { // IN A (DECIMAL)
            int value = 0;
            int c;
            while ((c = getchar()) != EOF && c >= '0' && c <= '9') {
                value = value * 10 + (c - '0');
            }
            cpu.A = value & 0xFF;
            instr_len = 1;
            break;
        }
        case 0x31: { // IN A (BINARY)
            int value = 0;
            int c;
            while ((c = getchar()) != EOF && (c == '0' || c == '1')) {
                value = (value << 1) | (c - '0');
            }
            cpu.A = value & 0xFF;
            instr_len = 1;
            break;
        }
        case 0xFF: { // HALT
            return EXIT_SUCCESS;
        }
        default: {
            printf("Unknown opcode: 0x%02X at PC=%zu\n", opcode, cpu.PC);
            return EXIT_FAILURE;
        }
        } // switch end

        cpu.PC += instr_len;
    }
}
