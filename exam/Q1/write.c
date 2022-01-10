#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int8_t write_buf[1024];

int main()
{
        int fd;
        fd = open("/dev/Mydriver1", O_RDWR);
        if(fd < 0) 
        {
                printf("Cannot open device file...\n");
                return 0;
        }
        
printf("Enter the string to write into driver :");
scanf("  %[^\t\n]s", write_buf);
printf("Data Writing ...");
write(fd, write_buf, strlen(write_buf)+1);
printf("Done!\n");

close(fd);


}
