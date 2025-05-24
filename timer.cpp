#include "timer.h"
#include "idt.h"
#include "util.h"
#include "CPU.h"
double cpu_clock_f=1506269091;
u32 timer_f=1;
u64 tcount = 0;
u64 secends = 0;




void timer_handler(regs *reg){
    //out_apic(0x320, 0x10000); // One-shot, Vector=0 (لا مقاطعة)
    //out_apic(0x3E0, 11);  // Divide=128
    //out_apic(0x380, 0xFFFFFFFF); // Initial Count
    tcount++;
    if((int)tcount%(int)timer_f==0){
        secends++;
        printc("\nhello , timer intrupt ",14);
    //printc(Hex(tcount),14);
       printc(Int_To_STR(tcount,10),14);
    }
   
    return;
}

void set_timer_f(u32 freq){
    !freq?freq=1:1;
    timer_f=freq;
    u32 dev = 1;
    u64 counter=(cpu_clock_f)/(dev*freq);
    out_apic(0x320,32|0x20000);
    out_apic(0x3e0,1);
    out_apic(0x380,counter);
}


void timer_init(){
	/*outb(0x43, 0x34); // وضع One-shot
    outb(0x40, 0xFF); // عد من 0xFF

    // 2. ضبط APIC Timer
    out_apic(0x320, 0x10000); // One-shot, Vector=0 (لا مقاطعة)
    out_apic(0x3E0, 0b1011);  // Divide=128
    out_apic(0x380, 0xFFFFFFFF); // Initial Count

    // 3. انتظر حتى ينتهي PIT
    while ((inb(0x40) & 0x80) == 0) {}

    // 4. قراءة Current Count
    u32 current = in_apic(0x390);
    u32 apic_freq = (0xFFFFFFFF - current) * 100 / 128; // 10ms = 0.01 ثانية

    cpu_clock_f = apic_freq; // حفظ التردد
    printc("\nspeed ",10);
    printc(Hex(apic_freq),10);
    */
    outb(0x43, 0x30); // 0x30 = 00110000 (Channel 0, Access low/high byte, Mode 0)
    
    // 2. ضبط القيمة الأولية لـ PIT (عد تنازلي من 65535)
    outb(0x40, 0xFF); // Low byte
    outb(0x40, 0xFF); // High byte (65535)
    
    // 3. قراءة TSC قبل البدء
    u64 start = rdtsc();
    
    // 4. الانتظار حتى ينتهي PIT (البت 5 في منفذ 0x43 يصبح 1 عند الانتهاء)
    while ((inb(0x43) & 0x20) == 0) {}
    
    // 5. قراءة TSC بعد الانتهاء
    u64 end = rdtsc();
    
    // 6. حساب التردد (65535 تك = 54.925ms على 1.193182 MHz)
    u64 delta_tsc = end - start;
    u64 apic_freq = (u32)((u32)delta_tsc / (65535 / 1193182));
    
    printc("\nCPU Frequency: ", 10);
    printc(Hex(cpu_clock_f), 10);
    install_irq(0,timer_handler,0);

    cpu_clock_f=apic_freq;
    set_timer_f(1);
}

void sleep(u32 milisecends){
    u64 secnd = tcount+milisecends;
    while (tcount<secnd){}
    return;
}