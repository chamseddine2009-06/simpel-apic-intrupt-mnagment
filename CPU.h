#ifndef CPU_h
#define CPU_h

#include "util.h"

void cpuid(u32& eax, u32& ebx, u32& ecx , u32& edx);
u64 rdmsr(u32 msr);
void wrmsr(u32 msr, u64 val);
u64 rdtsc();
namespace CPU_info
{
    typedef struct RSDPDescriptor {
        char Signature[8];
        u8 Checksum;
        char OEMID[6];
        u8 Revision;
        u32 RsdtAddress;
    } __attribute__ ((packed));

    typedef struct FADT {
        char Signature[4];        // "FACP"
        u32 Length;
        // ... حقول أخرى ...
        u32 PM_TMR_BLK;      // عنوان منفذ PM Timer
        u32 PM_TMR_LEN;      // طول منطقة PM Timer
        u8 PM_TMR_FREQ;      // تردد PM Timer (على X86 عادةً 3.579545 MHz)
        // ... حقول أخرى ...
    } __attribute__ ((packed));
    typedef struct HPET {
        char Signature[4];        // "HPET"
        u32 Length;
        u8 HardwareRevID;
        u8 ComparatorCount;
        u8 CounterSize;
        u8 Reserved;
        u16 LegacyRoute;
        u64 CounterClockPeriod; // الفترة بالفيمتوثانية (10^-15 ثانية)
        // ... حقول أخرى ...
    } __attribute__ ((packed));
    u64 get_cpu_freqence();
    char* get_cpu_name();
} // namespace CPU_info
#endif