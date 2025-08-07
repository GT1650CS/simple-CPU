MOV A, 10
STORE A, [0x1000]
MOV B, 10 ; 10 is newline in ASCII
STORE B, [0x1001]

loop_start:
LOAD A, [0x1000] ; Load counter
PRINT_DECIMAL A
LOAD A, [0x1001] ; Load newline char
PRINT_ASCII A

LOAD A, [0x1000]
DEC A
STORE A, [0x1000]

CMP A, 0
JNZ loop_start
HALT
