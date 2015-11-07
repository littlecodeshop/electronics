#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>


#define IN  0
#define OUT 1

#define LOW  0
#define HIGH 1

#define POUT 24  //used for the reset  

int uart0_filestream = -1;


static int GPIOExport(int pin)
{
#define BUFFER_MAX 3
    char buffer[BUFFER_MAX];
    ssize_t bytes_written;
    int fd;

    fd = open("/sys/class/gpio/export", O_WRONLY);
    if (-1 == fd) {
        fprintf(stderr, "Failed to open export for writing!\n");
        return(-1);
    }

    bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
    write(fd, buffer, bytes_written);
    close(fd);
    return(0);
}

static int GPIOUnexport(int pin)
{
    char buffer[BUFFER_MAX];
    ssize_t bytes_written;
    int fd;

    fd = open("/sys/class/gpio/unexport", O_WRONLY);
    if (-1 == fd) {
        fprintf(stderr, "Failed to open unexport for writing!\n");
        return(-1);
    }

    bytes_written = snprintf(buffer, BUFFER_MAX, "%d", pin);
    write(fd, buffer, bytes_written);
    close(fd);
    return(0);
}

    static int
GPIODirection(int pin, int dir)
{
    static const char s_directions_str[]  = "in\0out";

#define DIRECTION_MAX 35
    char path[DIRECTION_MAX];
    int fd;

    snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin);
    fd = open(path, O_WRONLY);
    if (-1 == fd) {
        fprintf(stderr, "Failed to open gpio direction for writing!\n");
        return(-1);
    }

    if (-1 == write(fd, &s_directions_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3)) {
        fprintf(stderr, "Failed to set direction!\n");
        return(-1);
    }

    close(fd);
    return(0);
}

    static int
GPIORead(int pin)
{
#define VALUE_MAX 30
    char path[VALUE_MAX];
    char value_str[3];
    int fd;

    snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
    fd = open(path, O_RDONLY);
    if (-1 == fd) {
        fprintf(stderr, "Failed to open gpio value for reading!\n");
        return(-1);
    }

    if (-1 == read(fd, value_str, 3)) {
        fprintf(stderr, "Failed to read value!\n");
        return(-1);
    }

    close(fd);

    return(atoi(value_str));
}

    static int
GPIOWrite(int pin, int value)
{
    static const char s_values_str[] = "01";

    char path[VALUE_MAX];
    int fd;

    snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
    fd = open(path, O_WRONLY);
    if (-1 == fd) {
        fprintf(stderr, "Failed to open gpio value for writing!\n");
        return(-1);
    }

    if (1 != write(fd, &s_values_str[LOW == value ? 0 : 1], 1)) {
        fprintf(stderr, "Failed to write value!\n");
        return(-1);
    }

    close(fd);
    return(0);
}

void SERIALSendByte(unsigned char c){
    int count = write(uart0_filestream, &c, 1);		//Filestream, bytes to write, number of bytes to write
    if(count!=1){
        printf("did not send !\n");
    }
}

unsigned char SERIALReceiveByte(){
    unsigned char rx_buffer;
    int rx_len = 0;
    do{
        rx_len =read(uart0_filestream,(void*)&rx_buffer,1);
    }while(rx_len<=0);
    printf("received %d %d\n",rx_len,rx_buffer);
    return rx_buffer;
}

    int
main(int argc, char *argv[])
{
    int repeat = 10;
    uart0_filestream=open("/dev/ttyAMA0",O_RDWR);
    if(uart0_filestream==-1){
        printf("shoot ! cannot open the freaking serial port !");
    }
    //configure the UART
    struct termios options;
    tcgetattr(uart0_filestream, &options);
    options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;		//<Set baud rate
    options.c_iflag = IGNPAR;
    options.c_oflag = 0;
    options.c_lflag = 0;
    tcflush(uart0_filestream, TCIFLUSH);
    tcsetattr(uart0_filestream, TCSANOW, &options);


    /*
     * Enable GPIO pins
     */
    if (-1 == GPIOExport(POUT) )
        return(1);

    /*
     * Set GPIO directions
     */
    if (-1 == GPIODirection(POUT, OUT) )
        return(2);

    /*
     * Write GPIO value to reset, c'est un toggle de 0 a 1
     */
    if (-1 == GPIOWrite(POUT, 0))
        return(3);
    usleep(1000*500);
    if (-1 == GPIOWrite(POUT, 1))
        return(3);


    usleep(3000 * 1000);


    //send the serial stuff :)

    SERIALSendByte(0x55);
    SERIALReceiveByte();
    //	SERIALSendByte(0x55);
    //	SERIALReceiveByte();
    //	SERIALSendByte(0x55);
    //	SERIALReceiveByte();
    //	SERIALSendByte(0x55);
    //	SERIALReceiveByte();
    //	SERIALSendByte(0x55);
    //	SERIALReceiveByte();
    //	SERIALSendByte(0x45);
    //	SERIALSendByte('V');
    //	SERIALReceiveByte();
    //	SERIALSendByte(0x43);
    //	SERIALSendByte(0x30);
    //	SERIALSendByte(0x20);
    //	SERIALSendByte(0x10);
    //	SERIALSendByte(0xF8);
    //	SERIALSendByte(0x00);
    //	SERIALReceiveByte();
    //	SERIALSendByte(0xFF);
    //	SERIALSendByte(0xE4);
    //	SERIALSendByte(0x00);
    //	SERIALSendByte(0x05);
    //	SERIALSendByte(0x00);
    //	SERIALSendByte(0x03);
    //	SERIALReceiveByte();

    SERIALSendByte(0x73);
    SERIALSendByte(0x2);
    SERIALSendByte(0x2);
    SERIALSendByte(0x0);
    SERIALSendByte(0xF8);
    SERIALSendByte(0x00);
    SERIALSendByte('H');
    SERIALSendByte('E');
    SERIALSendByte('L');
    SERIALSendByte('L');
    SERIALSendByte('O');
    SERIALSendByte(0x00);
    SERIALReceiveByte();

    usleep(5000 * 1000);




    /*
     * Disable GPIO pins
     */
    if (-1 == GPIOUnexport(POUT) )
        return(4);

    return(0);
}
