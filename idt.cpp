#include "idt.h"


extern "C" void irq_handler(regs* regesters);



idt_entry IDT[256];
idt_ptr IDT_ptr;
void* irqs_handlers[256];


extern "C" void isr0();
extern "C" void isr1();
extern "C" void isr2();
extern "C" void isr3();
extern "C" void isr4();
extern "C" void isr5();
extern "C" void isr6();
extern "C" void isr7();
extern "C" void isr8();
extern "C" void isr9();
extern "C" void isr10();
extern "C" void isr11();
extern "C" void isr12();
extern "C" void isr13();
extern "C" void isr14();
extern "C" void isr15();
extern "C" void isr16();
extern "C" void isr17();
extern "C" void isr18();
extern "C" void isr19();
extern "C" void isr20();
extern "C" void isr21();
extern "C" void isr22();
extern "C" void isr23();
extern "C" void isr24();
extern "C" void isr25();
extern "C" void isr26();
extern "C" void isr27();
extern "C" void isr28();
extern "C" void isr29();
extern "C" void isr30();
extern "C" void isr31();
//extern "C" void isr32();


extern "C" void isr128();
extern "C" void isr177();


extern "C" void irq0();
extern "C" void irq1();
extern "C" void irq2();
extern "C" void irq3();
extern "C" void irq4();
extern "C" void irq5();
extern "C" void irq6();
extern "C" void irq7();
extern "C" void irq8();
extern "C" void irq9();
extern "C" void irq10();
extern "C" void irq11();
extern "C" void irq12();
extern "C" void irq13();
extern "C" void irq14();
extern "C" void irq15();
//extern "C" void irq16();


void systemcall(){
    isr128();
}
bool APIC_enable = false;
bool SSE3_enable = false;

u32 APIC_Local = 0xFEE00000;
volatile u32* APICIO = (volatile u32*) 0xFEC00000;





void out_apic(u32 offset, u32 data){
   // *(volatile u32*)((APIC_Local+offset))/*hey , i dont know but mybe this most be reg*4*/=data;
    *(volatile u32*)((offset+APIC_Local))=(volatile int)data;
    //APIC_Local[offset]=data;
}

u32 in_apic(u32 offset){
    return *(volatile u32*)(APIC_Local + offset );//APIC_Local[offset];//  
}


void out_apic_io(u8 reg, u32 val){
    APICIO[0]=reg;
    *(volatile u32*)((u8*)APICIO+0x10)=val;
}

u32 in_apic_io(u8 reg){
    APICIO[0]=reg;
    return *(volatile u32*)((u8*)APICIO+0x10);    
}








void PIC_disable(){
    // إرسال أوامر لتعطيل PIC
    outb(0xA1, 0xFF);
    outb(0x21, 0xFF);

    outb(0x20, 0x11);       // Initialize both PICs
    	outb(0xA0, 0x11);       
    	outb(0x21, 0x20);       // Set vector offsets
    	outb(0xA1, 0x28);
    	outb(0x21, 0x04);       // tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    	outb(0xA1, 0x02);       // tell Slave PIC its cascade identity (0000 0010)
    	outb(0x21, 0x01);       // have the PICs use 8086 mode (and not 8080 mode)
    	outb(0xA1, 0x01);
    	outb(0x21, 0xff);        // set masks
    	outb(0xA1, 0xff);	
}




void EOI(regs* reg){
    if(APIC_enable){
        out_apic(0xb0,0);
        //*(volatile char*)(APIC_Local+0xb0)=-1;
        return;
    }
    else{
        if(reg->int_no >=40){
            outb(0xA0,0x20);
        }
        outb(0x20,0x20);
    }
    return;
}

/*********************************/





void irq_capabiltis(){
    u32 eax=1,ebx=0,ecx=0,edx=0;
    cpuid(eax,ebx,ecx,edx);
    
    if(edx&(1<<9)){
        PIC_disable();	
        APIC_enable=true;
        u64 readmsr = rdmsr(0x1B);
        /*if(!(readmsr&(1<<11))){
            //readmsr|=(1<<11);
            wrmsr(0x1B,readmsr|(1<<11));/*hi , my be wrmsr fonction have a problam , how are you doing ?*

        }*/
        readmsr&= ~(1<<11);
        readmsr|= (1<<11);
        out_apic(0xF0, in_apic(0xF0) | 0x100);
        wrmsr(0x1b,readmsr);
        
        readmsr = rdmsr(0x1B);

        APIC_Local = (volatile u32)((readmsr & 0xFFFFF000));
       	//APIC_Local[0x3f0]=0x1ff;
        out_apic(0x3f0, 0x1ff);
        out_apic(0xF0, in_apic(0xF0) | 0x100);
        /*if(readmsr>0){printc("rdmsr!=0",12);} 
        printc("\nrdmsr : ",10);
        printc(Hex((u64)(readmsr)),14);
        printc("\nAPIC Enabel ! at adress : ",10);
        printc(Hex((u32)APIC_Local),14);
        */
        printc("\n",10);

    }else{
        APIC_enable=false;
    }
    if(ecx&1){
        SSE3_enable=true;
    }else{
        SSE3_enable=false;
    }

}
void PIC_APIC_INIT(){
    if(APIC_enable){
            
        outb(0x64, 0xA8);
        outb(0x64, 0x20);
        u8 status = inb(0x60) | 2;
        outb(0x64, 0x60);
        outb(0x60, status);
        
        outb(0x64,0xD4);
        outb(0x60,0xf6);
        //mouse_write(0xF6);
        
        //mouse_read();
        inb(0x60);

        outb(0x64,0xD4);
        outb(0x60,0xf4);
        //mouse_write(0xF4);
        inb(0x60);

        // تهيئة IOAPIC لمقاطعة لوحة المفاتيح (IRQ1 -> Vector 33)
        out_apic_io(0x12, 0x21);  // Vector 33 لـ IRQ1
        out_apic_io(0x13, 0x0);  // Destination: CPU0
        //out_apic_io(0x10+2*12, 0x20+12);  // Vector 33 لـ IRQ1
        //out_apic_io(0x10+2*12+1, 0x0);  // Destination: CPU0

    }else{
        // تهيئة PIC التقليدي
        outb(0x20, 0x11);       // Initialize both PICs
    	outb(0xA0, 0x11);       
    	outb(0x21, 0x20);       // Set vector offsets
    	outb(0xA1, 0x28);
    	outb(0x21, 0x04);       // tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    	outb(0xA1, 0x02);       // tell Slave PIC its cascade identity (0000 0010)
    	outb(0x21, 0x01);       // have the PICs use 8086 mode (and not 8080 mode)
    	outb(0xA1, 0x01);
    	outb(0x21, 0x0);        // set masks
    	outb(0xA1, 0x0);	
    }
}
/*void PIC_APIC_INIT(){
    if(APIC_enable){
        //outb(0x22,0x70);
        //outb(0x23,1);
        //out_apic_io(0x10+2 ,0x21/*|(1<<15)*);//i dont kwox xay , but this enable keybeard irss , but just one intrupt 
        //out_apic_io(0x13,(in_apic(0x20)>>24)<<24);
        //out_apic(0xE0, 0xFFFFFFFF); // ضبط DFR إلى Flat Mode
       //out_apic(0x300, 0x40 | (1 << 14));  // إرسال IPI إلى نفس المعالج
        //while(in_apic(0x200));

    }else{
	    outb(0x20, 0x11);       // Initialize both PICs
    	outb(0xA0, 0x11);       
    	outb(0x21, 0x20);       // Set vector offsets
    	outb(0xA1, 0x28);
    	outb(0x21, 0x04);       // tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
    	outb(0xA1, 0x02);       // tell Slave PIC its cascade identity (0000 0010)
    	outb(0x21, 0x01);       // have the PICs use 8086 mode (and not 8080 mode)
    	outb(0xA1, 0x01);
    	outb(0x21, 0x0);        // set masks
    	outb(0xA1, 0x0);	
    }
}


*/




/*************************************************************/
void set_hadler(u32 index, u32 addr){
    IDT[index].offset_low=addr&0xffff;
    IDT[index].offset_high=(addr>>16)&0xffff;
    IDT[index].zero=0;
    IDT[index].selector=0x8;
    IDT[index].type_attr=0x8e;
}
void set_system_call(u32 index, u32 addr){
    IDT[index].offset_low=addr&0xffff;
    IDT[index].offset_high=(addr>>16)&0xffff;
    IDT[index].zero=0;
    IDT[index].selector=0x8;
    IDT[index].type_attr=0xee;//acss user interupt
}
/************************************************************/



void idt_init(){
    irq_capabiltis();
     //APIC_enable=false;
    PIC_APIC_INIT();
    set_hadler(0,(u32)isr0);
    set_hadler(1,(u32)isr1);
    set_hadler(2,(u32)isr2);
    set_hadler(3,(u32)isr3);
    set_hadler(4,(u32)isr4);
    set_hadler(5,(u32)isr5);
    set_hadler(6,(u32)isr6);
    set_hadler(7,(u32)isr7);
    set_hadler(8,(u32)isr8);
    set_hadler(9,(u32)isr9);
    set_hadler(10,(u32)isr10);
    set_hadler(11,(u32)isr11);
    set_hadler(12,(u32)isr12);
    set_hadler(13,(u32)isr13);
    set_hadler(14,(u32)isr14);
    set_hadler(15,(u32)isr15);
    set_hadler(16,(u32)isr16);
    set_hadler(17,(u32)isr17);
    set_hadler(18,(u32)isr18);
    set_hadler(19,(u32)isr19);
    set_hadler(20,(u32)isr20);
    set_hadler(21,(u32)isr21);
    set_hadler(22,(u32)isr22);
    set_hadler(23,(u32)isr23);
    set_hadler(24,(u32)isr24);
    set_hadler(25,(u32)isr25);
    set_hadler(26,(u32)isr26);
    set_hadler(27,(u32)isr27);
    set_hadler(28,(u32)isr28);
    set_hadler(29,(u32)isr29);
    set_hadler(30,(u32)isr30);
    set_hadler(31,(u32)isr31);
    //set_hadler(32,(u32)isr32);

    set_system_call(128,(u32)isr128);
    set_system_call(177,(u32)isr177);
   	
	
    set_hadler(32,(u32)irq0);
    set_hadler(33,(u32)irq1);
    set_hadler(34,(u32)irq2);
    set_hadler(35,(u32)irq3);
    set_hadler(36,(u32)irq4);
    set_hadler(37,(u32)irq5);
    set_hadler(38,(u32)irq6);
    set_hadler(39,(u32)irq7);
    set_hadler(40,(u32)irq8);
    set_hadler(41,(u32)irq9);
    set_hadler(42,(u32)irq10);
    set_hadler(43,(u32)irq11);
    set_hadler(44,(u32)irq12);
    set_hadler(45,(u32)irq13);
    set_hadler(46,(u32)irq14);
    set_hadler(47,(u32)irq15);
    //set_hadler(50,(u32)irq16);
    
    


    IDT_ptr.limit=sizeof(IDT)-1;
    IDT_ptr.base=(u32)(&IDT);
    asm("lidt %0" :: "m"(IDT_ptr));

    for(int i = 0 ; i < 256 ; i++){
        irqs_handlers[i]=(void*)0;
    }

    //syscall1();
    //irq0();
    
    

    
    
    //out_apic(0x320,32|0x20000);
  //  out_apic(0x3e0,3);
  //  out_apic(0x380,10000000);
  //  out_apic_io(0x10,32);

    // asm("int $33");
    //while(!in_apic(0x200)){}
    //printc("INTRUPT",10);
    //printc(Hex(in_apic(0x200)),10);
   
    //u32 id = in_apic_io(0x20);
    //out_apic_io(0x22,1);
    //printc((id>0)?(char*)"APIC ID: ":(char*)"N", 10);
    //printc(Hex(id>0), 14);
    //printc("\n", 10);

    return;
}
extern "C" void isr_handler(regs* regesters){
//	if(regesters->int_no==1){return;}
    printc("\n\nISR : ",10);
    printc(Hex(regesters->int_no),14);
    if(regesters->int_no>=1){
        regesters->eflags&=0xfffeff;//0xFFFFFf7F;
    };
    if(regesters->int_no<32){
        printc("\nSYSTEM CRACHE ",12);
        printc(Hex(regesters->int_no),12);
        printc("\n",12);
        if(regesters->int_no==0){
            regesters->eip+=1;
        }else{
            //hello , idont know her but..
        }
        //asm("hlt");
    }else if(regesters->int_no==128||regesters->int_no==177){
        printc("\nsystem call !!!\n",10);
    }else{
        //irq_handler(regesters);
        printc("\nIRQ\n",10);
    //    *(volatile char*)(APIC_Local+0xb0)=0;

    }
    printc("\n\n",10);
    //EOI(regesters);
    //ecx, 0x80B    

    //wrmsr(0x80b,0);           
    *(volatile u32*)0xFEE000B0=0;

    return;
}



void install_irq(u32 irq , void (*irq_hand)(regs* reg),u32 CPU){
    if(irq==96/*128-32 system call*/ || irq==145/*177-32*/){return;}
    out_apic_io(0x10+irq*2, 0x20+irq);  // Vector 33 لـ IRQ1
    out_apic_io(0x13, CPU);  // Destination: CPU0

    irqs_handlers[irq]=(void*)(irq_hand);
    return;
}


void uninstall_irq(u32 irq){
    out_apic_io(0x10+irq*2, 0);  // Vector 33 لـ IRQ1
    out_apic_io(0x13, 0);  // Destination: CPU0
    irqs_handlers[irq]=(void*)0;
    return;
}


extern "C" void irq_handler(regs* regesters){
    if(irqs_handlers[regesters->int_no-0x20]!=(void*)0){
        void (*handler)(regs* reg) = (void(*)(regs*))irqs_handlers[regesters->int_no-0x20];
        handler(regesters);
    }
    EOI(regesters);
    //wrmsr(0x80b,0);
    //out_apic(0xb0,(u32)0);
    //*(volatile u32*)0xFEE000B0=0;  
    return;
}

