#include <stabs.h>
#include <malloc.h>
#include <printf.h>
#include <timer.h>

extern int __bss_start__;
extern int __bss_end__;
extern int __stab_start__;

/*static unsigned char *mem_ptr_start = (unsigned char *)&__bss_start__; */
static unsigned char *mem_ptr       = (unsigned char *)&__bss_end__;

#define HEADERSIZE            sizeof(struct rz_header) //bytes
#define STABSTRUCTSIZE        sizeof(struct Stab)
#define INTBYTESSHIFTFAC      2                        
#define BYTESFROMPCTOFP       12                       //12 bytes from pc to fp
#define MAXSYMBOLLENGTH       20

//Simple struct to hold all register address
typedef struct Address{
    unsigned pc_address;
    unsigned lr_address;
    unsigned fp_address;
    unsigned sp_address;
} Address;

#if 0
//Maps index(n_value) to FUNC symbol
void findFuncSymbolAndPrintOut(unsigned int funcAddress, int framesNum){
    
    //Read the size of stab section
    unsigned char *stab_ptr = (unsigned char *)&__stab_start__;
    size_t stabSize = *((size_t *)stab_ptr);
    stab_ptr += (1 << INTBYTESSHIFTFAC);
    unsigned char *stabstr_ptr = stab_ptr + stabSize + (1 << INTBYTESSHIFTFAC);
    
    
    size_t startAddressEachSection = 0;
    struct Stab* s = 0;
    char symbol[MAXSYMBOLLENGTH];
    //Clear symbol str
    for (int i = 0; i < MAXSYMBOLLENGTH; i++){
        symbol[i] = '\0';
    }
    int  symbolInd = 0;
    //Scan all stab sections
    while (startAddressEachSection < stabSize){ 
        s = (struct Stab*)stab_ptr; 
        //Check n_type (function address)
        if (s->n_type == N_FUN && s->n_value == funcAddress){
            /*printf("0x%08x\n", s->n_value);*/
            while (*((char *)(stabstr_ptr + s->n_strx + symbolInd)) && 
                   *((char *)(stabstr_ptr + s->n_strx + symbolInd)) != ':' &&
                   symbolInd < MAXSYMBOLLENGTH){
                symbol[symbolInd] = *((char *)(stabstr_ptr + s->n_strx + symbolInd));
                symbolInd ++;
            }
            //Print out traceback results
            printf("#%d, 0x%08x at %s+\n", framesNum, funcAddress, symbol);
            return;
        }
        //Update pointers
        stab_ptr += STABSTRUCTSIZE;
        startAddressEachSection += STABSTRUCTSIZE;   
    }
}

void bt() {
  // FIXME: Write some code here! And maybe elsewhere, too.

    Address A;
    //Get current register address
    asm("mov %0, pc" : "=r" (A.pc_address));
    asm("mov %0, lr" : "=r" (A.lr_address));
    asm("mov %0, fp" : "=r" (A.fp_address));
    asm("mov %0, sp" : "=r" (A.sp_address));
    /*printf("start BT...\n");*/
    /*delaySec(0.5);*/
    

    int framesNum   = 0;
    unsigned fp_cur   = *((unsigned *)A.fp_address - 3); //point to first function executes bt()
    unsigned fp_pre   = 0;
    unsigned pc_saved = 1;
    /*unsigned sp_cur   = 0;*/
    /*unsigned lr       = 0;*/

    while (fp_cur && pc_saved){  
        fp_pre   = *((unsigned *)fp_cur - 3);
        /*lr       = *((unsigned *)fp_cur - 1);*/
        pc_saved = *((unsigned *)fp_cur);
        
        //check
        /*findFuncSymbolAndPrintOut(pc_saved - BYTESFROMPCTOFP, framesNum);    */
        /*printf("#%d, fp= 0x%08x\n", framesNum, fp_cur);*/
        /*printf("#%d, pc= 0x%08x\n", framesNum, pc_saved - BYTESFROMPCTOFP);*/
        /*printf("#%d, lr= 0x%08x\n", framesNum, lr);*/

        /*printf("#%d, 0x%08x\n", framesNum, pc_saved - BYTESFROMPCTOFUNC);*/
        /*printf("#%d, sp= 0x%08x\n", framesNum, sp_cur);*/
        /*printf("#%d 0x%08x\n", framesNum, lr_cur);*/


        //Trace back
        fp_cur = fp_pre;
        framesNum ++;
    }
}
#endif

#ifndef REDZONE_MALLOC
void *malloc(size_t n) {
    n = roundup(n, 8);
    void *alloc = (void *)mem_ptr;
    mem_ptr += n;

    return alloc;
}
#else
unsigned char* get__STABS__end(){
    //Read the size of stab section
    unsigned char *stab_ptr = (unsigned char *)&__stab_start__;
    size_t stabSize = *((size_t *)stab_ptr);
    stab_ptr += ((1 << INTBYTESSHIFTFAC) + stabSize);
    
    //Read the size of stabs section
    size_t stabsSize = *((size_t *)stab_ptr);
    stab_ptr += ((1 << INTBYTESSHIFTFAC) + stabSize);
        
    return stab_ptr;
}
void adjustMallocStartAddress(){
    unsigned char* stab_end = get__STABS__end();
    if (mem_ptr_start <= stab_end){
        mem_ptr_start = stab_end + (1 << INTBYTESSHIFTFAC); //4 bytes gap 
        mem_ptr       = stab_end + (1 << INTBYTESSHIFTFAC);
        /*printf("Stab end: 0x%08x, bss start: 0x%08x\n", stab_end, mem_ptr);*/
    }
}

void *_rzmalloc(size_t n, const char *file, int line, const char *func) {
    n = roundup(n, 8);
    /*printf("Cast size: %d\n", n);*/
    //Special case
    if (n == 0) return NULL;

    //Check STABS section address
    /*adjustMallocStartAddress();*/

    unsigned char* alloc = (unsigned char*)mem_ptr;
    struct rz_header header = {n, file, line, func};
    size_t allocSize = HEADERSIZE + n + 2 * REDZONE_SIZE; //bytes
    mem_ptr += allocSize;
    //Write header
    *((struct rz_header*)alloc) = header;
    alloc += HEADERSIZE;
    //Write redzone value
    int redZoneInd = 0;
    while (redZoneInd ++ < (REDZONE_SIZE >> INTBYTESSHIFTFAC)){
        *((unsigned int *)alloc) = REDZONE_VALUE;
        *((unsigned int *)(alloc + REDZONE_SIZE + n)) = REDZONE_VALUE;
        /*printf("Address: %08x, value: %x\n", alloc, *((unsigned int*)alloc));*/
        /*printf("Address: %08x, value: %x\n", alloc  + REDZONE_SIZE + n, */
                /**((unsigned int*)(alloc + REDZONE_SIZE + n)));*/
        alloc += (1 << INTBYTESSHIFTFAC);
    }
    return (void *)alloc;
}


int rz_check() {
    // FIXME: Check the integrity of all redzones.
    unsigned char* ptr = mem_ptr_start; //(unsigned char *)&__bss_start__;    
    struct rz_header* currHeader = 0;
    int redZoneInd = 0;
    while(ptr <= mem_ptr){
        //Get header
        currHeader = (struct rz_header*)ptr;
        //The idea here is to check if the alloc_size is multiple of 8 or equal to 0
        if (!(currHeader->alloc_size) || (currHeader->alloc_size % 8)){
            printf("REDZONE ERROR: %08x from [BAD HEADER]\n", ptr);
            return 0;
        }        
        ptr += HEADERSIZE;

        //Check REDZONE
        redZoneInd = 0;
        while (redZoneInd ++ < (REDZONE_SIZE >> INTBYTESSHIFTFAC)){
            /*printf("Address: %08x, value: %x\n", ptr, *((unsigned int*)ptr));*/
            if (*((unsigned int *)ptr) != REDZONE_VALUE){
                printf("REDZONE ERROR: %08x from [%s() at %s:%d]\n",
                        ptr + redZoneInd, currHeader->func, currHeader->file, currHeader->line);
                return 0;
            }
            if (*((unsigned int *)(ptr + currHeader->alloc_size + REDZONE_SIZE)) != REDZONE_VALUE){
                printf("REDZONE ERROR: %08x from [%s() at %s:%d]\n",
                        ptr + currHeader->alloc_size + REDZONE_SIZE + redZoneInd, currHeader->func, currHeader->file, currHeader->line);
                return 0;
            }

            ptr += (1 << INTBYTESSHIFTFAC);
        }
        //Move to next frame
        ptr += (currHeader->alloc_size + REDZONE_SIZE);
    }
    return 1;
}


void memset(void *dest, char c, size_t n ){
    char *char_dest = (char *)dest;
    for(; n > 0; n--) *char_dest++ = c;
}

void test1(){
    char *buffer = (char *)malloc(1 << 5);
    memset(buffer, 0, 1 << 5);
    assert(rz_check());
}

void test2(){
    char *buffer = (char *)malloc(1 << 6);
    memset(buffer - 1, 0, 1 << 6);
    assert(!rz_check());
}

void test3(){
    char *buffer = (char *)malloc(1 << 6);
    memset(buffer + 1, 0, 1 << 6);
    assert(!rz_check());
}
//Test broken header situation...
void test4(){
    char *buffer = (char *)malloc(1 << 6);
    memset(buffer - (REDZONE_SIZE + sizeof(struct rz_header)), 1, sizeof(struct rz_header));
    assert(!rz_check());
}

void test5(){
    //What if we broke header ??
    char *buffer = (char *)malloc(1 << 6);
    *(buffer - REDZONE_SIZE - HEADERSIZE) = 0;
    assert(!rz_check());
}


#endif


/*void main() {*/
  // TODO: Run some tests here to ensure your code works.
    /*delaySec(2);*/

    /*
     * Test bt()
     */
    /*Address A;*/
    /*Get current register address*/
    /*asm("mov %0, pc" : "=r" (A.pc_address));*/
    /*asm("mov %0, ip" : "=r" (A.ip_address));*/
    /*asm("mov %0, fp" : "=r" (A.fp_address));*/
    /*asm("mov %0, sp" : "=r" (A.sp_address));*/
    /*printf("0x%x\n", A.fp_address);*/
    /*bt();*/
    /*testBT();*/

     /*
     * Test malloc()
     */
    /*test1();*/
    /*test2();*/
    /*test3();*/
    /*test4();*/
    /*test5();*/
/*}*/
