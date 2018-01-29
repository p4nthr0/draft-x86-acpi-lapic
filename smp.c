/*
* functions for smp management
* p4nthr0, 2018
*/

#include <types.h>
#include <acpi.h>

#define MAX_CPU 255
typedef struct _CPU
{
    u32 isbsp;
    u32 lapic_id;
    u64 lapic_base;
} __attribute__((packed)) cpu_t;

static cpu_t cpu_info[MAX_CPU] = { 0 };
static int num_of_cpus = 0;

static void get_lapic_info_from_madt(void *madt_intc_hdr)
{
    struct MADT_INTC_LAPIC *madt_intc_lapic = (struct MADT_INTC_LAPIC *) madt_intc_hdr;

    if (madt_intc_lapic->flags)
    {
        cpu_info[num_of_cpus].lapic_id = madt_intc_lapic->apic_id;
        if (num_of_cpus == 0)
        {
            cpu_info[num_of_cpus].isbsp = 1;
        }

        num_of_cpus++;
    }
}

static void update_lapic_addr_for_all_cpus(void *madt_intc_hdr)
{
    struct MADT_INTC_LAPIC_OVERRIDE *madt_intc_lapic_override = (struct MADT_INTC_LAPIC_OVERRIDE *) madt_intc_hdr;

    for (int i = 0; i < num_of_cpus; i++)
    {
        cpu_info[i].lapic_base = madt_intc_lapic_override->local_apic_addr;
    }
}

/*
* Fills cpu_info, parsing ACPI tables (MADT entries)
* returns: number of cpus found
*/
int smp_init_cpu_info()
{
    rsdp_t *rsdp;
    madt_t *madt;

    if (acpi_get_rsdp(&rsdp))
    {
        if (acpi_get_madt(rsdp, &madt))
        {
            acpi_for_each_madt_intc(madt, MADT_INTC_LAPIC_TYPE, get_lapic_info_from_madt);

            for (int i = 0; i < num_of_cpus; i++)
            {
                cpu_info[i].lapic_base.low = madt->local_apic_addr;
                cpu_info[i].lapic_base.high = 0;
            }

            acpi_for_each_madt_intc(madt, MADT_INTC_LAPIC_OVERRIDE_TYPE, update_lapic_addr_for_all_cpus);
        }
    }

    return num_of_cpus;
}
