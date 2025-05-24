#include "util.h"
#include "CPU.h"
#include "idt.h"
#include "timer.h"

bool DiskErr = false;
void keyboard_handler(regs* reg);
void mouse_init();
int msx=0,msy=0;


extern "C" void main(){
	*(char*)0xb8000='A';
//	asm("hlt");*
/*    asm volatile(
   		 "pushf\n\t"           // ادفع EFLAGS إلى الستاك
   		 "pop %%eax\n\t"       // خذ القيمة في EAX
   		 "and $0xFFFFFEFF, %%eax\n\t" // أزل البت رقم 8 (TF)
   		 "push %%eax\n\t"      // أعد دفع القيمة المعدّلة
   		 "popf\n\t"            // أعد تحميل EFLAGS
    		::: "eax"
	);
*/
    for(int i=0;i<80*25*2;i++){
	    *(char*)(0xb8000+i)=0;
    }
    printc("plz press Enter whan you ready ....\n",14);
    while(inb(0x60)!=0x9c){}
    printc("*****START32******\n",10);
    printc("Interuupt test ....\n",14);
    idt_init();
    asm("sti");

    //int syscrash = 1/0;
    u64 cpu_frq = 0;//CPU_info::get_cpu_freqence();
    printc("CPU speed : ",10);
    printc(Hex((u64)cpu_frq),10);

    printc("\nCPU name : ",10);
    printc(CPU_info::get_cpu_name(),14);

    install_irq(1,keyboard_handler,0);

    timer_init();
    mouse_init();
    //set_timer_f(1);

    //printc((char*)memfind((u8*)"Hello from the earth" , (u8*)(char [2]){'f','r'} , 2 , 21),10);

    //asm("int $32");
    //int divisor = 1193180 / 1;  	  /* Calculate our divisor     */
	//outb(0x43, 0x36);			 /* Set our command byte 0x36 */
	//outb(0x40, divisor & 0xFF);  /* Set low byte of divisor   */
	//outb(0x40, divisor >> 8);   /* Set high byte of divisor  */

    printc("\nInteruupt test done ....\n",14);
    u32 x=0;
    while(1){
      //  printc("\r",10);
       // printc((char*)(char[2]){"\\|/-"[x%4],0},10);
        x++;
        
        //char l=inb(0x60);
    }
    printc("Reading the disk .....\n",10);
    
    //start_read_sector8(KERNEL_START_LBA,KERNEL_LONG,(u8*)KERNEL_POINTER);
    HARD_DISK_INFO* dinfo;
    disk_info((HARD_DISK_INFO*)dinfo);
    printc("Device_statue : ",15);
	printc(Int_To_STR(dinfo[0].General_Config,10),14);

	printc("\nCylinders number : ",15);
	printc(Int_To_STR(dinfo[0].Cylinders,10),14);

	printc("\nHeads nummber : ",15);
	printc(Int_To_STR(dinfo[0].Heads,10),14);

	printc("\nSectors per track : ",15);
	printc(Int_To_STR(dinfo[0].Track_Sectors,10),14);

	printc("\nSrial number : ",15);
	/*for(int i = 0 ; i <8 ; i++){
		if(dinfo[0].Serial_number[i]==0)dinfo[0].Serial_number[i]=' ';
	}*/
	printc((char*)dinfo[0].Serial_Number,14);

	
	printc("\nATA vertion : ",15);
	printc(Int_To_STR(dinfo[0].ATA_Version_Major,10),14);
	

	printc("\nModel name : ",15);
	/*for(int i = 0 ; i <16 ; i++){
		if(dinfo[0].Model_name[i]==0)dinfo[0].Model_name[i]=' ';
	}*/
	printc((char*)dinfo[0].Model_Name,14);
	

	printc("\nSector size : ",15);
	printc(Int_To_STR(dinfo[0].Sector_Bytes,10),14);
	
	printc("\nLBA count : ",15);
	printc(Int_To_STR(dinfo[0].LBA_28bit_Count,10),14);

    printc("\n\n\nReading the disk end .....",11);
    /*Mem*/	
    DMem_init();
    printc("\nMemory.....\nExtended 1 (frome 0 to 15 Mo   ) : ",15);
	printc(Int_To_STR(get_mem_info().Extend1,10),10);
	printc("  byte installed  .\nExtended 2 (frome 16 Mo to 4 Go) : ",15);
	printc(Int_To_STR(get_mem_info().Extend2,10),10);
	printc(" byte installed  .",15);
    
    u8* Aray=(u8*)malloc(4096);
    printc("\n\nDaynamick MEM ....\nallocating a araay in adress ",15);
    printc(Int_To_STR((u32)(Aray),10),10);
    if(!Aray){
	printc(" .FAIL!!!",12);
    }
    
    u8* Aray2=(u8*)malloc(4096);
    printc("\nallocating a araay in adress ",15);
    printc(Int_To_STR((u32)(Aray2),10),10);
    if(!Aray){
	printc(" .FAIL!!!",12);
    }
    mfree((void*)Aray2,4096);
     u8 hi = inb(0x60);
    while(1){
	
	if(inb(0x60)!=hi){
	      hi=inb(0x60);
	      char Pr[5]={hi,' ',' ', ' ',0};
	      printc((char*)Pr,10);
	}
    }
    return;
    //asm volatile("jmp $0x8200");
    //goto KERNEL_POINTER;
}


#define MEM_INFO_PTR 0x5000
#define BLOCK_SIZE 4096
#define START_KERNEL 0x100000



void keyboard_handler(regs* reg){
    u8 scancode = inb(0x60);
    printc("\n\n\nkey press ",10);
    printc(Int_To_STR(scancode,10),10);
    printc("\n\n\n",10);
    return;
}

 
void mouse_handler(regs* reg){
    static u8 ms_data[3];
    static u32 si = 0;
    
    u8 status = inb(0x64);  
    if (!(status & 0x20)){return;}
    si++;
    si%=3;
    char in = inb(0x60);
    ms_data[si]=in;
    if(si==0){
        if((msx+ms_data[1])<80 && (msx+ms_data[1])>=0){
            msx+=ms_data[1];
        }
        if((msy-ms_data[2])<25 && (msy-ms_data[2])>=0){
            msy-=ms_data[2];
        }
        printc("\nx:",10);
        printc(Int_To_STR(msx,10),10);
        printc("\ny:",10);
        printc(Int_To_STR(msy,10),10);
        //*(short*)(0xb8000+msy*80+msx)=('0'<<8)|15;
    }
    return;

}
void mouse_init(){
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

    install_irq(12,mouse_handler,0);

}

MEM_INFO mem_info;

u32 Using_Mem = 0;
u32 Start_Free_Mem = 0;

u8* MEM_map;//a bit map , evry bit reprisent a block , if it 1 that mean is used , else is free
u32 Map_Long=0;

u32 Map_Pointer=0;
/*Welcom to AyadiOS dynamick Memory mangment*/


void DMem_init(){
    mem_info.Extend1= (*(u16*)(MEM_INFO_PTR))*1024;
    mem_info.Extend2= (*(u16*)(MEM_INFO_PTR+2))<<16;
    
    MEM_map =(u8*)(START_KERNEL+(KERNEL_LONG*SECTOR_SIZE));
    
    Map_Long=((/*rrrr(*/mem_info.Extend2-(START_KERNEL+(KERNEL_LONG*SECTOR_SIZE)))/BLOCK_SIZE)/8;
    Map_Long-=(Map_Long/BLOCK_SIZE)/8;

    Using_Mem=(KERNEL_LONG*SECTOR_SIZE)+Map_Long;
    
    Start_Free_Mem=(START_KERNEL+(KERNEL_LONG*SECTOR_SIZE))+Map_Long;
    /*Exend2 have the full mem*/

    for(u32 i = 0 ; i < Map_Long/8 ; i++){
        MEM_map[i]=0;
    }
}


MEM_INFO get_mem_info(){
    MEM_INFO returned = mem_info;/*some how  ,a think that this make protact , some how ....*/
    return returned;
}

void add_memp(){
    if(Map_Pointer>=(mem_info.Extend2-Start_Free_Mem)){
        Map_Pointer=0;
    }
    Map_Pointer++;
}

char* Int_To_STR(int val, int base){

    static char buf[32] = {0};
    //for(int i = 0 ; i
    int i = 30;

    for(; val && i ; --i, val /= base)

        buf[i] = "0123456789abcdef"[val % base];
        /*if base =1 will return binary string 010101001 ,
          if base =10 will return int 1234567890
          if base = 16 will return hex nember 0x123456789abcdef*/
        //buf[i+1]='\0';
                                                
    return &buf[i+1];

}

char * Hex(u64 num){
	static char ret[17];
	for(int i = 0 ; i < 16 ; i ++){
		ret[i]=' ';
	}
	int ii = 0;
	for(int i = 15 ; i>=0; i--){
		char r= (num>>(i*4))&0xf;
		ret[ii]="0123456789abcdef"[(int)r];	
		ii++;
	}
//	ret[16]=0;
	return (char*)(ret); 

}

u8 xp=0,yp=0;

void printc(char* str, int Clor,...){
    //va_list gf;
	for(int i =0 ; str[i] != '\0' ; ++i){
		int VM=0xb8000+(yp*80+xp)*2;
		if(xp>=80){
			yp++;
			xp=0;
            //i++;
            continue;
		}
		if(str[i]=='\n'){
            yp++;
			xp=0;
			//i++;
            continue;
		}
		else if(str[i]=='\t'){
			xp+=4;
			str[i]=' ';
            continue;
		}
		else if(str[i]=='\r'){
			xp=0;
			//str[i]=str[i++];
            i++;
		}
		if(str[i]==0){return;}

        *(char**)VM=(char*)(Clor*0x100+str[i]);
		//Clear the screan==========================================
		if(yp>=25){
			/*int Clear= 0xb8000;
			while(Clear<(0xb8000+80*25*2)){
				*(char**)Clear=(char*)0x100;
				Clear+=1;
			}*/
			for(int ii = 0 ; ii<80*24*2;ii++){
				*(char*)(0xb8000+ii)=*(char*)(0xb8000+ii+80*2);
			}
			for(int ii = 0 ; ii < 80*2 ; ii++){
				*(short*)(0xb8000+ii+80*24*2)=0;
			}
            i--;
			yp=24;
			xp=0;
		}else{
			xp++;
		}
		

	}
	static volatile u16 pos=0;
    pos=yp*80+xp;
		//pos<0?pos*=-1:1;
		//pos=(u16)(yp<<8+xp<<8);
		
		outb(0x3d4 , 14);
		//outb(0x3d5 , 2 ) ;
		//outb(0x3d4,0x0f);
		outb(0x3d5,(u8)(pos&0xff));
		outb(0x3d4,15);
		outb(0x3d4,(u8)(((pos)>>8)&0xff));
}



bool memcmp(u8* ptr1 , u8* ptr2 , u32 lon){
    bool trh = true;
    for(int i = 0 ; i<lon ; i++){
        if(ptr1[i]!=ptr2[i]){
            trh=false;
        }
    }
    return trh;
}

 void* memfind(u8* start/*first pointer of find area*/ ,
                     u8* find /*find word*/,
                     u32 findlon/*long of find word*/ ,
                     u32 end/*the longth of find area*/)
{
    for(int i = 0 ; i< end ; i++){
        if(memcmp(&start[i],find,findlon)){
            return (void*)((u32)start+i);
        }
    }
    return (void*)find;
}


void outb(u32 ad, u8 v){
	asm("outb %%al, %%dx" :: "d" (ad), "a" (v));;
}
	// output word
void outw(u32 ad, u16 v){
	asm("outw %%ax, %%dx" :: "d" (ad), "a" (v));
}
	// output word
void outl(u32 ad, u32 v){
	asm("outl %%eax, %%dx" : : "d" (ad), "a" (v));
}
	// input byte
u8 inb(u32 ad){
	u8 _v;       \
	asm("inb %%dx, %%al" : "=a" (_v) : "d" (ad)); \
	return _v;
}
	// input word
u16 inw(u32 ad){
	u16 _v;			\
	asm("inw %%dx, %%ax" : "=a" (_v) : "d" (ad));	\
	return _v;
}
	// input word
u32 inl(u32 ad){
	u32 _v;			\
	asm("inl %%dx, %%eax" : "=a" (_v) : "d" (ad));	\
	return _v;
}


#define IDE_DATA 0x1F0       // منفذ البيانات
#define IDE_SECTOR_COUNT 0x1F2 // عدد القطاعات
#define IDE_LBA_LOW 0x1F3      // LBA Low Byte
#define IDE_LBA_MID 0x1F4      // LBA Mid Byte
#define IDE_LBA_HIGH 0x1F5     // LBA High Byte
#define IDE_DRIVE_SELECT 0x1F6 // اختيار القرص
#define IDE_COMMAND 0x1F7      // منفذ الأوامر
#define IDE_STATUS 0x1F7       // منفذ الحالة

#define ATA_READ 0x20          // أمر القراءة
#define ATA_WRITE 0x30
#define SECTOR_SIZE 512        // حجم القطاع (512 بايت)


u32 DiskErrSector = 0;
u32 start_GetDiskErr(){
    if(DiskErr){
        return DiskErrSector;
    }
    return 0;
}

void start_wait_for_drive() {
   // تحقق من جاهزية القرص قبل الإرسال
   while (inb(IDE_STATUS) & 0x80); // انتظر حتى ينتهي القرص من العمل (BSY = 0)
while ((inb(IDE_STATUS) & 0x08)); // انتظر حتى يصبح DRDY = 1

}

int start_GetDiskErorr(int lba){
    if (inb(IDE_STATUS) & 0x01) {  // إذا كانت هناك مشكلة في العملية (مثل الخطأ)
        printc("Disk ERORR",12);
        //printc((int)DiskErrSector,12);
        return 1;
    }
    return 0;
}

void start_read_sector8(u32 lba,u32 count, u8* buffer) {
    for(int ii = 0 ; ii < count ; ii++){
        start_wait_for_drive();

        outb(IDE_SECTOR_COUNT, 1);     
        outb(IDE_LBA_LOW, lba & 0xFF);
        outb(IDE_LBA_MID, (lba >> 8) & 0xFF); 
        outb(IDE_LBA_HIGH, (lba >> 16) & 0xFF);
        outb(IDE_DRIVE_SELECT, 0xE0 | ((lba >> 24) & 0x0F));

        outb(IDE_COMMAND, ATA_READ);
        if(start_GetDiskErorr(lba)){DiskErr=true;DiskErrSector=lba;return;};
        start_wait_for_drive();

        for (int i = 0; i < SECTOR_SIZE/2; i++) {
            ((u16*)buffer)[i+ii*(SECTOR_SIZE/2)] = inw(IDE_DATA);
        }
        lba++;
    }
}
int disk_info(HARD_DISK_INFO* hhd_info){
    start_wait_for_drive();

    
    
    outb(0x3f6,0x4);
    outb(IDE_DRIVE_SELECT,0xA0);
    outb(IDE_SECTOR_COUNT, 0);     
    outb(IDE_LBA_LOW, 0);
    outb(IDE_LBA_MID, 0); 
    outb(IDE_LBA_HIGH,0);
    outb(IDE_COMMAND,0xEC);
    
    /*if(inb(IDE_LBA_LOW)||inb(IDE_LBA_HIGH)){
        return 0;
    }*/
    u16 Buff[256];
    start_wait_for_drive();
    for(int i = 0 ; i < 256 ; i++){
        ((u16*)hhd_info)[i]=inw(IDE_DATA);
    }
    for (int i = 0; i < 20; i ++) {
       // u8 temp = hhd_info[0].Model_name[i];
        ((u16*)hhd_info[0].Model_Name)[i] =switche_byte16(((u16*)hhd_info[0].Model_Name)[i]);
        //hhd_info[0].Model_name[i + 1] = temp;
    }
    for (int i = 1; i <= 10; i ++) {
        ((u16*)hhd_info[0].Serial_Number)[i] = switche_byte16(((u16*)hhd_info[0].Serial_Number)[i]);
    }
    //hhd_info=(HARD_DISK_INFO*)Buff;

    return 1;
    
}





void* malloc(u32 size){
    u32 Block_take=(int)((size/BLOCK_SIZE)+1.0);
    bool isit = true;//you dont undrstant? i do this for if somthing is wrong yhis will be fals
    bool no_mem = false;
    u32 tourns_cont=0;
    //printc(Int_To_STR(Map_Long,10),10);
    while(1){
        for(; get_bit(MEM_map[Map_Pointer/8],Map_Pointer%8);
            Map_Pointer>=(mem_info.Extend2-Start_Free_Mem)?Map_Pointer=0:Map_Pointer++
            ,tourns_cont++){}
        
        for(int i = 0 ; i<Block_take ; i++){
            if(get_bit(MEM_map[(Map_Pointer+i)/8],(Map_Pointer+i)%8)){
		isit=false;
            }
        
	    if(Map_Pointer+i>=Map_Long/*(Map_Pointer+i)*BLOCK_SIZE>=(mem_info.Extend2-Start_Free_Mem)*/){
                isit=false;
            }
            tourns_cont++;
        }
        if(isit){
            for(int i= 0 ; i<Block_take;i++){
	        MEM_map[(Map_Pointer+i)/8]|=1<<((Map_Pointer+i)/8);
	    }
	    void* array = (void*)(Map_Pointer*BLOCK_SIZE+Start_Free_Mem);
            Map_Pointer+=Block_take;
            return array;
        }
        if((tourns_cont>=Map_Long)){
            return (void*)0;
        }
        
        isit=true;

    }

}




void mfree(void* adress,u32 size){
    u32 Block_take=(int)((size/BLOCK_SIZE)+1.0);
    u32 freeptr=((u32)adress)/BLOCK_SIZE;
    for(int i = freeptr ; i < freeptr+Block_take; i++){
        MEM_map[freeptr/8]=MEM_map[i/8]&(0xff&(0xfe<<(i%8)));
    }
}
