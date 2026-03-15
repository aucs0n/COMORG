/*
 * Team Number: 1
 * Team Members: GHEMARSON JOHN D. NACUA
 *               JUSTINE ANN U. CATALAN 
 *               REMY GABRIEL K. CORDERO
 */

#include <stdio.h>

unsigned int ACC;
unsigned char SF, OF, ZF, CF;

int ALU(unsigned char operand1, unsigned char operand2, unsigned char control_signal);
unsigned char twosComp(unsigned data);
unsigned char setFlags(unsigned int acc_value);
void printBin(int data, unsigned char data_width);

int main(void) {
    unsigned int op1_input = 0, op2_input = 0, cs_input = 0;

    ALU(0x03, 0x05, 0x02);
    printf("\n");
    ALU(0x88, 0x85, 0x01);
    printf("\n");
    ALU(0xC0, 0x02, 0x03);
    printf("\n");

    ALU(0x0F, 0x0F, 0x04);
    printf("\n");
    ALU(0x0F, 0xF0, 0x05);
    printf("\n");
    ALU(0xAA, 0x00, 0x06);
    printf("\n");
    ALU(0xFF, 0x0F, 0x07);
    printf("\n");
    ALU(0x04, 0x01, 0x08);
    printf("\n");
    ALU(0x04, 0x01, 0x09);

    printf("\nEnter custom ALU input in hex (e.g., OP1=03 OP2=05 CTRL=02).\n");
    printf("OP1 (00-FF): ");
    if (scanf("%x", &op1_input) != 1) {
        return 0;
    }
    printf("OP2 (00-FF): ");
    if (scanf("%x", &op2_input) != 1) {
        return 0;
    }
    printf("CTRL (01-09): ");
    if (scanf("%x", &cs_input) != 1) {
        return 0;
    }

    printf("\n");
    ALU((unsigned char)op1_input, (unsigned char)op2_input, (unsigned char)cs_input);

    return 0;
}

int ALU(unsigned char operand1, unsigned char operand2, unsigned char control_signal) {
    unsigned char temp_OP1 = 0x00;
    unsigned char temp_OP2 = 0x00;
    unsigned char result8 = 0x00;

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
        result8 = (unsigned char)(ACC & 0x00FF);
        setFlags(ACC);

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

        for (int n = 1; n <= 8; n++) {
            unsigned char Q_0 = temp_OP2 & 0x01;
            unsigned char msb_a = (temp_prod >> 7) & 0x01;
            unsigned char lsb_a;
            unsigned char lsb_q;

            if (Q_0 == 1 && Q_n1 == 0) {
                temp_prod = (unsigned char)(temp_prod + twosComp(operand1));
            } else if (Q_0 == 0 && Q_n1 == 1) {
                temp_prod = (unsigned char)(temp_prod + operand1);
            }

            lsb_a = temp_prod & 0x01;
            lsb_q = temp_OP2 & 0x01;

            temp_prod = (unsigned char)(temp_prod >> 1);
            temp_OP2 = (unsigned char)(temp_OP2 >> 1);

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

        ACC = ((unsigned int)temp_prod << 8) | (unsigned int)temp_OP2;
        setFlags(ACC);
        OF = ((ACC & 0xFF80U) != 0U && (ACC & 0xFF80U) != 0xFF80U) ? 1U : 0U;
        printf("ACC: ");
        printBin(ACC, 16);
        printf("\n");
        printf("ZF=%d, CF=%d, SF=%d, OF=%d\n", ZF, CF, SF, OF);
    } else if (control_signal >= 0x04 && control_signal <= 0x09) {
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
    return (unsigned char)((~data) + 1U);
}

unsigned char setFlags(unsigned int acc_value) {
    unsigned char low8 = (unsigned char)(acc_value & 0x00FFU);

    ZF = (low8 == 0U) ? 1U : 0U;
    SF = ((low8 & 0x80U) != 0U) ? 1U : 0U;
    CF = (acc_value > 0x00FFU) ? 1U : 0U;

    return 0;
}

void printBin(int data, unsigned char data_width) {
    for (int i = data_width - 1; i >= 0; i--) {
        printf("%d", (data >> i) & 1);
    }
}