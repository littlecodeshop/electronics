
#include <stdio.h>

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <fcntl.h>

#define BCM2708_PERI_BASE       0x20000000
#define GPIO_BASE               (BCM2708_PERI_BASE + 0x200000)  // GPIO controller 

#define BLOCK_SIZE      (4*1024)

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x)
#define INP_GPIO(g)   *(gpio.addr + ((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g)   *(gpio.addr + ((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio.addr + (((g)/10))) |= (((a)<=3?(a) + 4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET  *(gpio.addr + 7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR  *(gpio.addr + 10) // clears bits which are 1 ignores bits which are 0

#define GPIO_READ(g)  *(gpio.addr + 13) &= (1<<(g))

#define SPICS  18
#define SPICLK 21
#define SPIRES 23
#define SPIDIO 24

// IO Acces
struct bcm2835_peripheral {
    unsigned long addr_p;
    int mem_fd;
    void *map;
    volatile unsigned int *addr;
};


struct bcm2835_peripheral gpio = {GPIO_BASE};

// Exposes the physical address defined in the passed structure using mmap on /dev/mem
int map_peripheral(struct bcm2835_peripheral *p)
{
    // Open /dev/mem
    if ((p->mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
        printf("Failed to open /dev/mem, try checking permissions.\n");
        return -1;
    }

    p->map = mmap(
            NULL,
            BLOCK_SIZE,
            PROT_READ|PROT_WRITE,
            MAP_SHARED,
            p->mem_fd,      // File descriptor to physical memory virtual file '/dev/mem'
            p->addr_p       // Address in physical map that we want this memory block to expose
            );

    if (p->map == MAP_FAILED) {
        perror("mmap");
        return -1;
    }

    p->addr = (volatile unsigned int *)p->map;

    return 0;
}

void unmap_peripheral(struct bcm2835_peripheral *p) {

    munmap(p->map, BLOCK_SIZE);
    close(p->mem_fd);
}

void send_command(unsigned char cmd){
    GPIO_CLR = 1 << SPIDIO;
    GPIO_SET = 1 << SPICLK;
    usleep(150);
    GPIO_CLR = 1 << SPICLK;
    for(int i=0;i<8;i++){
        if((cmd>>(7-i))&0x1){
            GPIO_SET = 1 << SPIDIO;
        }
        else{
            GPIO_CLR = 1 << SPIDIO;
        }
        GPIO_SET = 1 << SPICLK;
        usleep(150);
        GPIO_CLR = 1 << SPICLK;
    }
}

void send_data(unsigned char data){
    GPIO_SET = 1 << SPIDIO;
    GPIO_SET = 1 << SPICLK;
    usleep(150);
    GPIO_CLR = 1 << SPICLK;
    for(int i=0;i<8;i++){
        if((data>>(7-i))&0x1){
            GPIO_SET = 1 << SPIDIO;
        }
        else{
            GPIO_CLR = 1 << SPIDIO;
        }
        GPIO_SET = 1 << SPICLK;
        usleep(150);
        GPIO_CLR = 1 << SPICLK;
    }
}

void reset_lcd(){
    GPIO_CLR = 1 << SPIRES;
    usleep(2000);
    GPIO_SET = 1 << SPIRES;
    usleep(2000);
    send_command(0xCA);
    send_data(0x00);
    send_data(0x20);
    send_data(0x00);
    send_command(0xBB);
    send_data(0x01);
    send_command(0xD1);
    send_command(0x94);
    send_command(0x20);
    send_data(0x0f);
    send_command(0xA7);
    send_command(0xBC);
    send_data(0x01);
    send_data(0x00);
    send_data(0x02);
    send_command(0x81);
    send_data(32);
    send_data(3);
    usleep(2000);
    send_command(0xAF);
}

void fill_rect(unsigned char x1,unsigned char x2,unsigned char y1,unsigned char y2,char r,char g, char b){
    send_command(0x75);
    send_data(x1);
    send_data(x2);
    send_command(0x15);
    send_data(y1);
    send_data(y2);
    int l = (x2-x1)*(y2-y1);
    send_command(0x5C);
    unsigned char byte1 = r<<4|g;
    unsigned char byte2 = b<<4|r;
    unsigned char byte3 = g<<4|b;
    for (int i=0;i<(l/2);i++){ 
        send_data(byte1);
        send_data(byte2);
        send_data(byte3);
    }
}

int main(){
    
    if(map_peripheral(&gpio) == -1){
        printf("INIT FAILED !!\n");
        return -1;
    }

    //setup outputs
    INP_GPIO(21); //SPICLK
    OUT_GPIO(21); //SPICLK
    INP_GPIO(24); //SPIDIO
    OUT_GPIO(24); //SPIDIO
    INP_GPIO(18); //SPICS
    OUT_GPIO(18); //SPICS
    INP_GPIO(23); //SPIRES
    OUT_GPIO(23); //SPIRES

    INP_GPIO(25); //SPICLK
    OUT_GPIO(25); //SPICLK
    
    reset_lcd();
    fill_rect(0,80,0,80,0xFF,0x00,0x00);


    while(1) {
        GPIO_SET = 1<<25;
        sleep(1);
        GPIO_CLR = 1<<25;
        sleep(1);
    }


}

