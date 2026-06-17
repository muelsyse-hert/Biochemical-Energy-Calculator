#include <stdio.h>
#include <string.h>

#define MAX_STEPS 10
#define MAX_MACRO_STEPS 20

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

typedef struct {
    char macro_name[32];
    double input_grams;
    MetabolicStep steps[MAX_MACRO_STEPS];
    int actual_steps;
    double total_atp;
    double total_heat;
    double total_loss;
} Macromolecule;

static int is_placeholder_precursor(const char* text)
{
    if (text == NULL) {
        return 1;
    }

    return strcmp(text, "N/A") == 0 || strcmp(text, "\xE6\x97\xA0") == 0;
}

void display_small_molecule_table(const OrganicSubstance* obj)
{
    if (obj == NULL) {
        return;
    }

    printf("==== Small Molecule Oxidation Summary ====\n");
    printf("----\n");
    printf("%-20s %-15s %-15s %-15s\n",
           "Organic Type", "Total Heat(kJ)", "Total ATP(mol)", "Total Loss(kJ)");
    printf("----\n");
    printf("%-20s %-15.2f %-15.2f %-15.2f\n",
           obj->substance_name,
           obj->total_heat,
           obj->total_atp,
           obj->total_loss);
    printf("====\n");
}

void display_macro_molecule_table(const Macromolecule* macro)
{
    if (macro == NULL) {
        return;
    }

    printf("==== Macromolecule Oxidation Summary ====\n");
    printf("----\n");
    printf("%-20s %-15s %-15s %-15s\n",
           "Macro Type", "Total Heat(kJ)", "Total ATP(mol)", "Total Loss(kJ)");
    printf("----\n");
    printf("%-20s %-15.2f %-15.2f %-15.2f\n",
           macro->macro_name,
           macro->total_heat,
           macro->total_atp,
           macro->total_loss);
    printf("====\n");

    printf("==== Full Pathway Detail ====\n");
    printf("----\n");
    printf("%-8s %-24s %-28s %-15s %-15s %-15s\n",
           "Step", "Reaction Stage", "Precursor / Amount", "Heat(kJ)", "ATP(mol)", "Loss(kJ)");
    printf("----\n");

    for (int i = 0; i < macro->actual_steps && i < MAX_MACRO_STEPS; ++i) {
        char precursor_text[80];
        const char* precursor = macro->steps[i].precursor_product;

        if (is_placeholder_precursor(precursor)) {
            snprintf(precursor_text, sizeof(precursor_text), "-");
        } else {
            snprintf(precursor_text,
                     sizeof(precursor_text),
                     "%s / %.2f",
                     precursor,
                     macro->steps[i].utilized_amount);
        }

        printf("%-8d %-24s %-28s %-15.2f %-15.2f %-15.2f\n",
               i + 1,
               macro->steps[i].step_name,
               precursor_text,
               macro->steps[i].heat_yield,
               macro->steps[i].atp_yield,
               macro->steps[i].energy_loss);
    }

    printf("====\n");
}
