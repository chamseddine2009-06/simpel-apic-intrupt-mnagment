#ifndef Util_h
#define Util_h

#define KERNEL_LONG 128
#define KERNEL_POINTER 0x8200
#define KERNEL_START_LBA 2
#define SECTOR_SIZE 512        // حجم القطاع (512 بايت)



#define get_bit(in,bit) (in&(1<<bit))>0?1:0
#define switche_byte16(x) ((x&0xff)<<8)|((x>>8)&0xff)
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef struct{
    u32 Extend1 = 0;
    u32 Extend2 = 0;
}MEM_INFO;

void printc(char* str, int Clor,...);
bool memcmp(u8* ptr1 , u8* ptr2 , u32 lon);
 void* memfind(u8* start/*first pointer of find area*/ ,
                     u8* find /*find word*/,
                     u32 findlon/*long of find word*/ ,
                     u32 end/*the longth of find area*/);
void start_read_sector8(u32 lba,u32 count, u8* buffer);


void DMem_init();
MEM_INFO get_mem_info();
char* Int_To_STR(int val, int base);
char * Hex(u64 num);
void* malloc(u32 size);
void  mfree(void* adress,u32 size);
u8 inb(u32 ad);
void outb(u32 ad, u8 v);

void idt_init();
void systemcall();

void cpuid(u32& eax, u32& ebx, u32& ecx , u32& edx);
u64 rdmsr(u32 msr);
void wrmsr(u32 msr, u64 val);

struct HARD_DISK_INFO {
    u16 General_Config;            // تكوين الجهاز العام
    u16 Cylinders;                 // عدد الأسطوانات (CHS)
    u16 Reserved1;
    u16 Heads;                     // عدد الرؤوس (CHS)
    u16 Track_Sectors;             // عدد القطاعات لكل مسار (CHS)
    u16 Sector_Bytes;              // عدد البايتات لكل قطاع (غالبًا 512)
    u16 Vendor_Specific1[3];
    u8  Serial_Number[20];         // الرقم التسلسلي (ASCII)
    u16 Buffer_Type;
    u16 Buffer_Size;               // حجم المخزن المؤقت (Cache)
    u16 ECC_Bytes;
    u8  Firmware_Revision[8];      // إصدار الـ Firmware (ASCII)
    u8  Model_Name[40];            // اسم الموديل (ASCII)
    u16 Max_Sectors_Per_Interrupt;
    u16 Capabilities;              // القدرات (LBA، DMA)
    u16 Reserved2;
    u16 PIO_Mode;
    u16 DMA_Mode;
    u16 Validity_Flags;            // صلاحية الحقول
    u32 LBA_28bit_Count;           // عدد القطاعات في وضع LBA-28
    u16 Reserved3[2];
    u64 LBA_48bit_Count;           // عدد القطاعات في وضع LBA-48
    u16 Reserved4[22];
    u16 Security_Status;           // حالة الأمان
    u16 Reserved5[31];
    u16 ATA_Version_Major;         // إصدار ATA المدعوم
    u16 ATA_Version_Minor;
    u16 Supported_Features[2];     // الميزات المدعومة
    u16 Reserved6[48];
} __attribute__((packed));

int disk_info(HARD_DISK_INFO* hhd_info);
#endif