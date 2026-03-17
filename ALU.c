/*
 * Team Number: 1
 * Team Members: GHEMARSON JOHN D. NACUA
 *               JUSTINE ANN U. CATALAN 
 *               REMY GABRIEL K. CORDERO
 */

#include <stdio.h>

/*
 * Global ALU state:
 * - ACC is 16-bit so we can detect carry from 8-bit operations.
 * - SF, OF, ZF, CF are status flags (Sign, Overflow, Zero, Carry).
 */
unsigned int ACC;
unsigned char SF, OF, ZF, CF;

/* Function prototypes required by the exercise. */
int ALU(unsigned char operand1, unsigned char operand2, unsigned char control_signal);
unsigned char twosComp(unsigned data);
unsigned char setFlags(unsigned int acc_value);
void printBin(int data, unsigned char data_width);

void main(void) {
    ALU(0x03, 0x05, 0x02); /* 00000011 - 00000101 (Subtract) */
    printf("\n");
    ALU(0x88, 0x85, 0x01); /* 10001000 + 10000101 (Add) */
    printf("\n");
    ALU(0xC0, 0x02, 0x03); /* 11000000 * 00000010 (Multiply) */
}

int ALU(unsigned char operand1, unsigned char operand2, unsigned char control_signal) {
    /*
     * temp_OP1/temp_OP2 are working copies used by algorithms.
     * result8 stores the lower 8 bits to evaluate signed overflow on add/sub.
     */
    unsigned char temp_OP1 = 0x00;
    unsigned char temp_OP2 = 0x00;
    unsigned char result8 = 0x00;

    /* Reset ALU outputs before each operation. */
    ACC = 0x0000;
    SF = 0;
    CF = 0;
    ZF = 0;
    OF = 0;

    printf("****************\n");
    printf("Fetching operands...\n");
    printf("OP1: ");
    printBin(operand1, 8);
    printf("\n");
    printf("OP2: ");
    printBin(operand2, 8);
    printf("\n");

    /* Arithmetic operations: ADD and SUB (SUB uses 2's complement). */
    if (control_signal == 0x01 || control_signal == 0x02) {
        printf("Operation: %s\n", control_signal == 0x01 ? "ADD" : "SUB");
        temp_OP1 = operand1;

        if (control_signal == 0x02) {
            printf("2's complement OP2\n");
            temp_OP2 = twosComp(operand2);
        } else {
            temp_OP2 = operand2;
        }

        printf("Adding OP1 & OP2...\n");
        ACC = (unsigned int)temp_OP1 + (unsigned int)temp_OP2;

        /* Keep only 8 bits for signed overflow checks. */
        result8 = (unsigned char)(ACC & 0x00FF);
        setFlags(ACC);

        /*
         * Signed overflow formulas:
         * ADD: same-sign inputs produce different-sign result.
         * SUB: different-sign inputs produce different-sign result from operand1.
         */
        if (control_signal == 0x01) {
            OF = ((~(operand1 ^ operand2) & (operand1 ^ result8) & 0x80) != 0U) ? 1U : 0U;
        } else {
            OF = (((operand1 ^ operand2) & (operand1 ^ result8) & 0x80) != 0U) ? 1U : 0U;
        }

        printf("ACC: ");
        printBin(ACC, 16);
        printf("\n");
        printf("ZF=%d, CF=%d, SF=%d, OF=%d\n", ZF, CF, SF, OF);
    } else if (control_signal == 0x03) {
        /* Booth's multiplication registers: A=temp_prod, Q=temp_OP2, Q_n1. */
        unsigned char temp_prod = 0x00;
        unsigned char Q_n1 = 0;

        printf("Operation: MUL\n");
        printf("A\t\tQ\t\tQ_n1\tM\t\tn\n");

        temp_OP2 = operand2;
        printBin(temp_prod, 8);
        printf("\t");
        printBin(temp_OP2, 8);
        printf("\t%d\t", Q_n1);
        printBin(operand1, 8);
        printf("\t0\n");

        /* 8 cycles because operands are 8-bit wide. */
        for (int n = 1; n <= 8; n++) {
            unsigned char Q_0 = temp_OP2 & 0x01;
            unsigned char msb_a = (temp_prod >> 7) & 0x01;
            unsigned char lsb_a;
            unsigned char lsb_q;

            /* Booth decision rules based on Q0 and Q_n1. */
            if (Q_0 == 1 && Q_n1 == 0) {
                temp_prod = (unsigned char)(temp_prod + twosComp(operand1));
            } else if (Q_0 == 0 && Q_n1 == 1) {
                temp_prod = (unsigned char)(temp_prod + operand1);
            }

            /* Save bits needed to emulate arithmetic right shift of (A,Q,Q_n1). */
            lsb_a = temp_prod & 0x01;
            lsb_q = temp_OP2 & 0x01;

            temp_prod = (unsigned char)(temp_prod >> 1);
            temp_OP2 = (unsigned char)(temp_OP2 >> 1);

            /* Move old LSB(A) into MSB(Q), keep old MSB(A) as sign bit of A. */
            temp_OP2 = (unsigned char)(temp_OP2 | (unsigned char)(lsb_a << 7));
            temp_prod = (unsigned char)(temp_prod | (unsigned char)(msb_a << 7));
            Q_n1 = lsb_q;

            printBin(temp_prod, 8);
            printf("\t");
            printBin(temp_OP2, 8);
            printf("\t%d\t", Q_n1);
            printBin(operand1, 8);
            printf("\t%d\n", n);
        }

        /* Final 16-bit product is A concatenated with Q. */
        ACC = ((unsigned int)temp_prod << 8) | (unsigned int)temp_OP2;
        setFlags(ACC);

        /* Overflow for signed 8-bit result expectation: upper bits must be sign-extension. */
        OF = ((ACC & 0xFF80U) != 0U && (ACC & 0xFF80U) != 0xFF80U) ? 1U : 0U;
        printf("ACC: ");
        printBin(ACC, 16);
        printf("\n");
        printf("ZF=%d, CF=%d, SF=%d, OF=%d\n", ZF, CF, SF, OF);
    } else if (control_signal >= 0x04 && control_signal <= 0x09) {
        /* Logic and shift operations. */
        printf("Operation: ");
        switch (control_signal) {
            case 0x04:
                printf("AND\n");
                ACC = (unsigned int)(operand1 & operand2);
                break;
            case 0x05:
                printf("OR\n");
                ACC = (unsigned int)(operand1 | operand2);
                break;
            case 0x06:
                printf("NOT\n");
                ACC = (unsigned int)((~operand1) & 0xFFU);
                break;
            case 0x07:
                printf("XOR\n");
                ACC = (unsigned int)(operand1 ^ operand2);
                break;
            case 0x08:
                printf("SHR\n");
                ACC = (unsigned int)(operand1 >> (operand2 & 0x07));
                break;
            case 0x09:
                printf("SHL\n");
                ACC = (unsigned int)((operand1 << (operand2 & 0x07)) & 0xFFU);
                break;
            default:
                break;
        }

        /* For logic operations we do not treat OF as signed arithmetic overflow. */
        setFlags(ACC);
        OF = 0;
        printf("ACC: ");
        printBin(ACC, 16);
        printf("\n");
        printf("ZF=%d, CF=%d, SF=%d, OF=%d\n", ZF, CF, SF, OF);
    } else {
        printf("Invalid control signal. Use values 0x01 to 0x09.\n");
    }

    return 0;
}

unsigned char twosComp(unsigned data) {
    /* Two's complement: invert all bits then add 1. */
    return (unsigned char)((~data) + 1U);
}

unsigned char setFlags(unsigned int acc_value) {
    /* Use only lower 8 bits as ALU result width for ZF and SF. */
    unsigned char low8 = (unsigned char)(acc_value & 0x00FFU);

    ZF = (low8 == 0U) ? 1U : 0U;
    SF = ((low8 & 0x80U) != 0U) ? 1U : 0U;

    /* CF is set when arithmetic result exceeds 8-bit unsigned range. */
    CF = (acc_value > 0x00FFU) ? 1U : 0U;

    return 0;
}

void printBin(int data, unsigned char data_width) {
    /* Print from most significant bit down to least significant bit. */
    for (int i = data_width - 1; i >= 0; i--) {
        printf("%d", (data >> i) & 1);
    }
}