#include <stdio.h>
#include <stdlib.h>

#define MAX_STEPS 10

typedef struct {
    char step_name[64];
    double atp_yield;
    double heat_yield;
    double energy_loss;
    char precursor_product[32];
    double utilized_amount;
} MetabolicStep;

typedef struct {
    char substance_name[32];
    double input_mol;
    MetabolicStep steps[MAX_STEPS];
    int actual_steps;
    double total_atp;
    double total_heat;
    double total_loss;
} OrganicSubstance;

extern void calc_glucose(OrganicSubstance* obj);
extern void calc_glycerol(OrganicSubstance* obj);
extern void calc_alanine(OrganicSubstance* obj);
extern void display_small_molecule_table(const OrganicSubstance* obj);

static void clear_input_buffer(void)
{
    int ch;

    while ((ch = getchar()) != '\n' && ch != EOF) {
        /* discard */
    }
}

static int read_menu_choice(int* choice)
{
    if (choice == NULL) {
        return 0;
    }

    if (scanf("%d", choice) != 1) {
        clear_input_buffer();
        return 0;
    }

    clear_input_buffer();
    return 1;
}

static int read_positive_mol(double* mol)
{
    if (mol == NULL) {
        return 0;
    }

    if (scanf("%lf", mol) != 1) {
        clear_input_buffer();
        return 0;
    }

    clear_input_buffer();

    if (*mol <= 0.0) {
        return 0;
    }

    return 1;
}

int main(void)
{
    while (1) {
        int choice = -1;
        double mol = 0.0;
        OrganicSubstance obj;

        printf("\n==== Biochemical Energy Calculator ====\n");
        printf("[1] 计算葡萄糖\n");
        printf("[2] 计算甘油\n");
        printf("[3] 计算丙氨酸\n");
        printf("[0] 安全退出\n");
        printf("请输入指令: ");

        if (!read_menu_choice(&choice)) {
            printf("输入无效，请输入 0~3 的数字指令。\n");
            continue;
        }

        if (choice == 0) {
            printf("系统已安全退出。\n");
            break;
        }

        if (choice < 1 || choice > 3) {
            printf("请输入 0~3 之间的有效指令。\n");
            continue;
        }

        printf("请输入物质的量 (mol): ");
        if (!read_positive_mol(&mol)) {
            printf("输入无效，物质的量必须是大于 0 的数字。\n");
            continue;
        }

        obj.input_mol = mol;
        obj.actual_steps = 0;
        obj.total_atp = 0.0;
        obj.total_heat = 0.0;
        obj.total_loss = 0.0;
        obj.substance_name[0] = '\0';

        switch (choice) {
            case 1:
                calc_glucose(&obj);
                break;
            case 2:
                calc_glycerol(&obj);
                break;
            case 3:
                calc_alanine(&obj);
                break;
            default:
                /* 理论上不会到达这里 */
                continue;
        }

        display_small_molecule_table(&obj);
    }

    return 0;
}
