# 8-Bit ALU in C

This project is a simple 8-bit Arithmetic Logic Unit (ALU) written in C.

It performs arithmetic and logic operations based on an 8-bit control signal. The program also shows the step-by-step process of the computation, especially for subtraction and multiplication.

## What This Program Can Do

The ALU supports these operations:

| Control Signal | Operation |
| --- | --- |
| `0x01` | Addition |
| `0x02` | Subtraction |
| `0x03` | Multiplication using Booth's Algorithm |
| `0x04` | AND |
| `0x05` | OR |
| `0x06` | NOT |
| `0x07` | XOR |
| `0x08` | Shift Right (logical) |
| `0x09` | Shift Left (logical) |

## Main Parts of the Program

The program uses these global variables:

- `ACC` = accumulator/result register
- `SF` = sign flag
- `OF` = overflow flag
- `ZF` = zero flag
- `CF` = carry flag

Even though the ALU is 8-bit, `ACC` is declared as `unsigned int` so the code can detect carry or larger results.

## Functions Used

### `int ALU(unsigned char operand1, unsigned char operand2, unsigned char control_signal)`

This is the main ALU function.

It receives:

- `operand1` = first 8-bit input
- `operand2` = second 8-bit input
- `control_signal` = selects the operation

### `unsigned char twosComp(unsigned data)`

This gets the 2's complement of a value.

It is used in subtraction because:

`A - B = A + (2's complement of B)`

### `unsigned char setFlags(unsigned int acc_value)`

This updates the ALU flags:

- `ZF = 1` if result is zero
- `SF = 1` if the sign bit is `1`
- `CF = 1` if the result exceeds 8 bits

`OF` is computed separately where signed arithmetic rules are needed.

### `void printBin(int data, unsigned char data_width)`

This prints a value in binary form.

Example:

`printBin(3, 8)` prints `00000011`

## How the Program Runs

The `main()` function does two things:

1. It runs built-in test cases for all ALU operations.
2. It asks the user to enter one custom test in hexadecimal.

That means when you run the program, it will first show sample outputs, then it will ask for:

- `OP1`
- `OP2`
- `CTRL`

## Example Input

If you want to test subtraction `3 - 5`, enter:

```text
03
05
02
```

This means:

- `03` = operand 1
- `05` = operand 2
- `02` = subtraction

The ALU interprets it as:

```text
00000011 - 00000101
```

The result is `-2`, which in 8-bit signed binary is:

```text
11111110
```

So the output includes:

```text
ACC: 0000000011111110
ZF=0, CF=0, SF=1, OF=0
```

## Sample Tests You Can Try

### Addition

Input:

```text
88
85
01
```

Meaning:

- `0x88 + 0x85`
- control signal `0x01` = ADD

### Subtraction

Input:

```text
03
05
02
```

Meaning:

- `0x03 - 0x05`

### Multiplication

Input:

```text
C0
02
03
```

Meaning:

- `0xC0 * 0x02`
- control signal `0x03` = MUL

This also prints the Booth multiplication table step by step.

### AND

Input:

```text
0F
0F
04
```

### OR

Input:

```text
0F
F0
05
```

### NOT

Input:

```text
AA
00
06
```

For NOT, the second operand is not used, so `00` is entered.

### XOR

Input:

```text
FF
0F
07
```

### Shift Right

Input:

```text
04
01
08
```

Meaning:

- shift `0x04` right by `1`

### Shift Left

Input:

```text
04
01
09
```

Meaning:

- shift `0x04` left by `1`

## How To Build

Using GCC:

```powershell
gcc ALU.c -o ALU.exe
```

If your GCC is in MSYS2 like this setup:

```powershell
C:\msys64\ucrt64\bin\gcc.exe ALU.c -o ALU.exe
```

## How To Run

```powershell
.\ALU.exe
```

The program will print built-in test cases first, then ask for custom input.

## How To Check If It Is Working

Your program is working if:

- it builds without errors
- it prints binary values for operands and results
- subtraction shows the `2's complement OP2` step
- multiplication prints the Booth table
- each operation prints `ACC`, `ZF`, `CF`, `SF`, and `OF`

## Notes for Beginners

- Hexadecimal values like `03`, `0F`, and `FF` are base-16 numbers.
- `0x` in C means the number is hexadecimal.
- 8-bit signed values use the highest bit as the sign bit.
- A result with leading `1` in bit 7 is treated as negative in signed arithmetic.
- `ACC` is shown as 16 bits so carry and extended results can be seen clearly.

## Files in This Repo

- `ALU.c` = source code
- `ALU.exe` = compiled executable on Windows
- PDF files = lab instructions and reference materials

## Summary

This project demonstrates:

- 8-bit arithmetic operations
- logic operations
- signed number handling for arithmetic
- 2's complement subtraction
- Booth's multiplication algorithm
- status flag generation
- binary output formatting

It is a lab-style implementation meant to show both the result and the internal process of the ALU.