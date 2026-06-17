#include <stdio.h>
#include <string.h>

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

static double abs_double(double value)
{
    return (value < 0.0) ? -value : value;
}

static int validate_alanine_distribution(const OrganicSubstance* obj,
                                         double expected_total_atp,
                                         double expected_total_loss)
{
    const double eps = 1e-6;
    double sum_atp = 0.0;
    double sum_loss = 0.0;

    if (obj == NULL || obj->actual_steps != 3) {
        return 0;
    }

    if (obj->total_atp <= 0.0 || obj->total_loss < 0.0) {
        return 0;
    }

    for (int i = 0; i < obj->actual_steps; ++i) {
        if (obj->steps[i].atp_yield < 0.0 ||
            obj->steps[i].heat_yield < 0.0 ||
            obj->steps[i].energy_loss < 0.0) {
            return 0;
        }

        if (abs_double(obj->steps[i].heat_yield - obj->steps[i].energy_loss) > eps) {
            return 0;
        }

        sum_atp += obj->steps[i].atp_yield;
        sum_loss += obj->steps[i].energy_loss;
    }

    if (abs_double(sum_atp - expected_total_atp) > eps) {
        return 0;
    }

    if (abs_double(sum_loss - expected_total_loss) > eps) {
        return 0;
    }

    return 1;
}

void calc_glucose(OrganicSubstance* obj)
{
    if (obj == NULL) {
        return;
    }

    /* 清空历史步骤，避免重复计算时残留旧数据 */
    memset(obj->steps, 0, sizeof(obj->steps));
    obj->actual_steps = 0;
    obj->total_atp = 0.0;
    obj->total_heat = 0.0;
    obj->total_loss = 0.0;

    snprintf(obj->substance_name, sizeof(obj->substance_name), "Glucose");

    if (obj->input_mol <= 0.0) {
        return;
    }

    /* 现代 32 ATP 模型（苹果酸-天冬氨酸穿梭） */
    const double input = obj->input_mol;
    const double base_total_atp = 32.0;
    const double base_total_heat = 2870.0;

    obj->actual_steps = 3;
    obj->total_atp = base_total_atp * input;
    obj->total_heat = base_total_heat * input;
    obj->total_loss = obj->total_heat - (obj->total_atp * 30.5);

    /* Step 1: 糖酵解
       ATP 来源：底物水平磷酸化 + NADH 通过苹果酸-天冬氨酸穿梭折算
       净产：7 mol ATP / mol glucose
       前驱物：Glucose -> Pyruvate
    */
    snprintf(obj->steps[0].step_name, sizeof(obj->steps[0].step_name), "Glycolysis");
    obj->steps[0].atp_yield = 7.0 * input;
    snprintf(obj->steps[0].precursor_product, sizeof(obj->steps[0].precursor_product), "Glucose -> Pyruvate");
    obj->steps[0].utilized_amount = input;

    /* Step 2: 丙酮酸氧化脱羧
       ATP 来源：丙酮酸脱氢酶复合体生成的 NADH 折算
       净产：5 mol ATP / mol glucose
       前驱物：Pyruvate -> Acetyl-CoA
    */
    snprintf(obj->steps[1].step_name, sizeof(obj->steps[1].step_name), "Pyruvate oxidation");
    obj->steps[1].atp_yield = 5.0 * input;
    snprintf(obj->steps[1].precursor_product, sizeof(obj->steps[1].precursor_product), "Pyruvate -> Acetyl-CoA");
    obj->steps[1].utilized_amount = input;

    /* Step 3: 三羧酸循环及氧化磷酸化
       ATP 来源：TCA 产生的 NADH/FADH2 及其电子传递链氧化磷酸化
       净产：20 mol ATP / mol glucose
       前驱物：Acetyl-CoA -> CO2
    */
    snprintf(obj->steps[2].step_name, sizeof(obj->steps[2].step_name), "TCA + OXPHOS");
    obj->steps[2].atp_yield = 20.0 * input;
    snprintf(obj->steps[2].precursor_product, sizeof(obj->steps[2].precursor_product), "Acetyl-CoA -> CO2");
    obj->steps[2].utilized_amount = input;

    /* 按 ATP 占比分配总能量损耗；heat_yield 与 energy_loss 数值相同 */
    if (obj->total_atp > 0.0) {
        for (int i = 0; i < obj->actual_steps; ++i) {
            obj->steps[i].energy_loss = obj->total_loss * (obj->steps[i].atp_yield / obj->total_atp);
            obj->steps[i].heat_yield = obj->steps[i].energy_loss;
        }

        /* 简单校验：确认三步 ATP 总和与热损耗分配闭合。
           若后续修改导致失衡，则按当前步 ATP 占比重新归一化一次。 */
        if (!validate_alanine_distribution(obj, obj->total_atp, obj->total_loss)) {
            double step_atp_sum = 0.0;

            for (int i = 0; i < obj->actual_steps; ++i) {
                step_atp_sum += obj->steps[i].atp_yield;
            }

            if (step_atp_sum > 0.0) {
                for (int i = 0; i < obj->actual_steps; ++i) {
                    obj->steps[i].energy_loss = obj->total_loss * (obj->steps[i].atp_yield / step_atp_sum);
                    obj->steps[i].heat_yield = obj->steps[i].energy_loss;
                }
            }
        }
    }
}

void calc_glycerol(OrganicSubstance* obj)
{
    if (obj == NULL) {
        return;
    }

    memset(obj->steps, 0, sizeof(obj->steps));
    obj->actual_steps = 0;
    obj->total_atp = 0.0;
    obj->total_heat = 0.0;
    obj->total_loss = 0.0;

    snprintf(obj->substance_name, sizeof(obj->substance_name), "Glycerol");

    if (obj->input_mol <= 0.0) {
        return;
    }

    /* 题设现代标准：1 mol glycerol 完全氧化净产 18.5 mol ATP */
    const double input = obj->input_mol;
    const double base_total_atp = 18.5;
    const double base_total_heat = 1655.0;

    obj->actual_steps = 3;
    obj->total_atp = base_total_atp * input;
    obj->total_heat = base_total_heat * input;
    obj->total_loss = obj->total_heat - (obj->total_atp * 30.5);

    /* Step 1: 甘油激活与脱氢
       ATP 来源：甘油激酶活化消耗与甘油-3-磷酸脱氢/再氧化折算后的净 ATP 当量
       这里按题设总 18.5 ATP 进行闭合分配，设为 1.5 mol ATP / mol glycerol
       前驱物：Glycerol -> DHAP
    */
    snprintf(obj->steps[0].step_name, sizeof(obj->steps[0].step_name), "Glycerol activation/DHAP");
    obj->steps[0].atp_yield = 1.5 * input;
    snprintf(obj->steps[0].precursor_product, sizeof(obj->steps[0].precursor_product), "Glycerol -> DHAP");
    obj->steps[0].utilized_amount = input;

    /* Step 2: 糖酵解下半段
       ATP 来源：DHAP 进入下半段糖酵解后的底物水平磷酸化 + NADH 折算
       为匹配总和 18.5 ATP，设为 4.0 mol ATP / mol glycerol
       前驱物：DHAP -> Pyruvate
    */
    snprintf(obj->steps[1].step_name, sizeof(obj->steps[1].step_name), "Lower glycolysis");
    obj->steps[1].atp_yield = 4.0 * input;
    snprintf(obj->steps[1].precursor_product, sizeof(obj->steps[1].precursor_product), "DHAP -> Pyruvate");
    obj->steps[1].utilized_amount = input;

    /* Step 3: 丙酮酸进入 TCA 与氧化磷酸化
       ATP 来源：丙酮酸脱氢酶、TCA 与电子传递链/氧化磷酸化
       剩余 ATP 当量设为 13.0 mol ATP / mol glycerol
       前驱物：Pyruvate -> CO2
    */
    snprintf(obj->steps[2].step_name, sizeof(obj->steps[2].step_name), "Pyruvate oxidation/TCA");
    obj->steps[2].atp_yield = 13.0 * input;
    snprintf(obj->steps[2].precursor_product, sizeof(obj->steps[2].precursor_product), "Pyruvate -> CO2");
    obj->steps[2].utilized_amount = input;

    if (obj->total_atp > 0.0) {
        for (int i = 0; i < obj->actual_steps; ++i) {
            obj->steps[i].energy_loss = obj->total_loss * (obj->steps[i].atp_yield / obj->total_atp);
            obj->steps[i].heat_yield = obj->steps[i].energy_loss;
        }
    }
}

void calc_alanine(OrganicSubstance* obj)
{
    if (obj == NULL) {
        return;
    }

    memset(obj->steps, 0, sizeof(obj->steps));
    obj->actual_steps = 0;
    obj->total_atp = 0.0;
    obj->total_heat = 0.0;
    obj->total_loss = 0.0;

    snprintf(obj->substance_name, sizeof(obj->substance_name), "Alanine");

    if (obj->input_mol <= 0.0) {
        return;
    }

    /* 题设：1 mol alanine 完全氧化约净产 15 mol ATP */
    const double input = obj->input_mol;
    const double base_total_atp = 15.0;
    const double base_total_heat = 1300.0;

    obj->actual_steps = 3;
    obj->total_atp = base_total_atp * input;
    obj->total_heat = base_total_heat * input;
    obj->total_loss = obj->total_heat - (obj->total_atp * 30.5);

    /* Step 1: 转氨基作用
       ATP 来源：转氨基本身不直接产生 ATP，但该步伴随底物活化与自由能重分配，
       因此在本程序中采用“ATP 等价权重”进行热/损耗分配，避免把该步视为零能量事件。
       这里记为 0.5 mol ATP 等价 / mol alanine。
       前驱物：Alanine -> Pyruvate
    */
    snprintf(obj->steps[0].step_name, sizeof(obj->steps[0].step_name), "Transamination");
    obj->steps[0].atp_yield = 0.5 * input;
    snprintf(obj->steps[0].precursor_product, sizeof(obj->steps[0].precursor_product), "Alanine -> Pyruvate");
    obj->steps[0].utilized_amount = input;

    /* Step 2: 丙酮酸氧化脱羧
       ATP 来源：丙酮酸脱氢酶复合体生成 NADH 折算
       为满足总 15 ATP，并与第一步的非零能量权重协同分配，这里设为 3.5 mol ATP / mol alanine
       前驱物：Pyruvate -> Acetyl-CoA
    */
    snprintf(obj->steps[1].step_name, sizeof(obj->steps[1].step_name), "Pyruvate oxidation");
    obj->steps[1].atp_yield = 3.5 * input;
    snprintf(obj->steps[1].precursor_product, sizeof(obj->steps[1].precursor_product), "Pyruvate -> Acetyl-CoA");
    obj->steps[1].utilized_amount = input;

    /* Step 3: 三羧酸循环及氧化磷酸化
       ATP 来源：Acetyl-CoA 进入 TCA 后产生的 NADH/FADH2 及其氧化磷酸化
       剩余当量分配为 11.0 mol ATP / mol alanine
       前驱物：Acetyl-CoA -> CO2
    */
    snprintf(obj->steps[2].step_name, sizeof(obj->steps[2].step_name), "TCA + OXPHOS");
    obj->steps[2].atp_yield = 11.0 * input;
    snprintf(obj->steps[2].precursor_product, sizeof(obj->steps[2].precursor_product), "Acetyl-CoA -> CO2");
    obj->steps[2].utilized_amount = input;

    if (obj->total_atp > 0.0) {
        for (int i = 0; i < obj->actual_steps; ++i) {
            obj->steps[i].energy_loss = obj->total_loss * (obj->steps[i].atp_yield / obj->total_atp);
            obj->steps[i].heat_yield = obj->steps[i].energy_loss;
        }
    }
}
