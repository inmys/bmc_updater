#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <linux/i2c.h>
#include <errno.h>

#define FILE_BUF_SIZE (1024*1024)

uint8_t file_buf[FILE_BUF_SIZE];

int i2c_ioctl_smbus_write (int fd, uint8_t dev, uint8_t regaddr, uint8_t data)
{
    int i, j = 0;
    int ret;
    union i2c_smbus_data buf;

    buf.byte = data;
    struct i2c_smbus_ioctl_data payload;
	payload.read_write = I2C_SMBUS_WRITE;
        payload.size = I2C_SMBUS_BYTE_DATA;
        payload.command = regaddr;
        payload.data = &buf;
   /*
    ret = ioctl (fd, I2C_SLAVE_FORCE, dev);
    if (ret < 0)
    {
        ret = -errno;
	printf("%d ",__LINE__);perror("smbus: ");
        goto exit;
    }
*/
    ret = ioctl (fd, I2C_SMBUS, &payload);
    if (ret < 0)
    {
        ret = -errno;
	printf("%d ",__LINE__);perror("smbus: ");
        goto exit;
    }

exit:
    return ret;
}
int i2c_ioctl_smbus_read (int fd, uint8_t dev, uint8_t regaddr, uint8_t *data)
{
    int i, j = 0;
    int ret;
    union i2c_smbus_data buf;


    struct i2c_smbus_ioctl_data payload;
	payload.read_write = I2C_SMBUS_READ;
        payload.size = I2C_SMBUS_BYTE_DATA;
        payload.command = regaddr;
        payload.data = &buf;
                       /*
    ret = ioctl (fd, I2C_SLAVE_FORCE, dev);
    if (ret < 0)
    {
        ret = -errno;
	printf("%d ",__LINE__);perror("smbus: ");
        goto exit;
    }
                         */
    ret = ioctl (fd, I2C_SMBUS, &payload);
    if (ret < 0)
    {
        ret = -errno;
	printf("%d ",__LINE__);perror("smbus: ");
        goto exit;
    }

exit:
	if(ret<0)
		perror("smbus: ");
	*data=buf.byte;
    return ret;
}

     /*

void WriteByte(int addr, int file_i2c,uint8_t reg_addr, uint8_t reg_data) {
	uint8_t command[] = { reg_addr, reg_data };
	struct i2c_msg message = { addr, 0, sizeof(command), command };
	struct i2c_rdwr_ioctl_data ioctl_data = { &message, 1 };
	int result = ioctl(file_i2c, I2C_RDWR, &ioctl_data);
	if (result != 2) {
		perror("failed to get variables");
		printf("line %d\n\r",__LINE__);
		exit(1);
		}
}
*/

unsigned char ReadStatus(int addr, int file_i2c) {
	uint8_t bt;
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE0, 0x00); //# CS LOW

	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, 0x05);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, 0x00);
        i2c_ioctl_smbus_read  (file_i2c, addr, 0xE1, &bt);
	//printf("status 1: 0x%x\n\r",bt);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE0, 0x01); //# CS HIGH
	return bt;

}


unsigned char ReadID(int addr, int file_i2c) {
	uint8_t bt;
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE0, 0x00);

	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, 0x90);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, 0x0);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, 0x0);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, 0x0);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, 0x0);
        i2c_ioctl_smbus_read  (file_i2c, addr, 0xE1, &bt);
	printf("read: 0x%x\n\r",bt);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, 0x0);
        i2c_ioctl_smbus_read  (file_i2c, addr, 0xE1, &bt);
	printf("read: 0x%x\n\r",bt);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE0, 0x01);
	//printf("line: %d\n\r",__LINE__);
	return 0;
}
unsigned char ReadJEDECID(int addr, int file_i2c,uint8_t *jd_id) {
	uint8_t bt;
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE0, 0x00);

	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, 0x9F);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, 0x0);
        i2c_ioctl_smbus_read  (file_i2c, addr, 0xE1, &jd_id[0]);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, 0x0);
        i2c_ioctl_smbus_read  (file_i2c, addr, 0xE1, &jd_id[1]);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, 0x0);
        i2c_ioctl_smbus_read  (file_i2c, addr, 0xE1, &jd_id[2]);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE0, 0x01);
	return 0;
}

void WriteEnable(int addr, int file_i2c) {
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE0, 0x00); // # CS LOW
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, 0x06);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE0, 0x01); // # CS HIGH
}

void SectorErase(int addr, int file_i2c,int sector_address) {
	uint8_t bt;
	WriteEnable(addr,file_i2c);

	i2c_ioctl_smbus_write (file_i2c, addr, 0xE0, 0x00);  // # CS LOW
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, 0x20);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, (sector_address>>16)&0xff);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, (sector_address>>8)&0xff);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, (sector_address>>0)&0xff);

	i2c_ioctl_smbus_write (file_i2c, addr, 0xE0, 0x01); // # CS HIGH

	bt =  ReadStatus(addr,file_i2c);
	while(bt&0x01) {
//		if(bt&0x01)printf("line: %d, bt=0x%x\n\r",__LINE__,bt);
		bt =  ReadStatus(addr,file_i2c);
		//if(bt&0x01)printf("line: %d, bt=0x%x\n\r",__LINE__,bt);
//		printf("bt=0x%x\n\r",bt);
	}

}
uint8_t SWAP(uint8_t bt) {
	uint8_t	 result;
	result = 0;
	for(int i=0;i<8;i++)
		if(bt & (1<<i))
			result	|= 1<<(7-i);
	return result;
}

void PageProgram(int addr, int file_i2c,uint8_t *buf,int sector_address,uint8_t swp) {
	int i;
	uint8_t bt;
	WriteEnable(addr,file_i2c);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE0, 0x00);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, 0x02);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, (sector_address>>16)&0xff);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, (sector_address>>8)&0xff);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, (sector_address>>0)&0xff);
	for(i=0;i<256;i++)
		i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, swp ? SWAP(buf[i]) : buf[i]);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE0, 0x01);

	bt =  ReadStatus(addr,file_i2c);
	while(bt&0x01) {
		if(bt&0x01)printf("line: %d, bt=0x%x\n\r",__LINE__,bt);
		bt =  ReadStatus(addr,file_i2c);
	}
}

void ResetDevice(int addr, int file_i2c) {
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE0, 0x00); // # CS LOW
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, 0x66);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE0, 0x01); // # CS HIGH
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE0, 0x00); // # CS LOW
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, 0x90);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE0, 0x01); // # CS HIGH
}

void ResetDeviceFF(int addr, int file_i2c) {
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE0, 0x00); // # CS LOW
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, 0xFF);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE0, 0x01); // # CS HIGH
}

void GlobalUnlock(int addr, int file_i2c) {
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE0, 0x00); // # CS LOW
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, 0x98);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE0, 0x01); // # CS HIGH
}

void EEPROM_ReadStatus(int file_i2c, int addr, uint8_t *bt ) {
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE0, 0x00); //# CS LOW

	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, 0x05);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, 0x00);
        i2c_ioctl_smbus_read  (file_i2c, addr, 0xE1, bt);
	//printf("status 1: 0x%x\n\r",bt);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE0, 0x01); //# CS HIGH
}

void EEPROM_WriteENA(int file_i2c, int addr) {
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE0, 0x00); //# CS LOW
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, 0x06);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE0, 0x01); //# CS HIGH
}
void EEPROM_PageProgram(int file_i2c,int addr,uint8_t *buf,int sector_address) {
	int i;
	uint8_t bt;

	EEPROM_WriteENA(file_i2c,addr);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE0, 0x00);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, 0x02);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, (sector_address>>8)&0xff);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, (sector_address>>0)&0xff);
	for(i=0;i<128;i++)
		i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, buf[i]);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE0, 0x01);

	EEPROM_ReadStatus(file_i2c,addr,&bt);
	while(bt&0x01) {
//		if(bt&0x01)printf("line: %d, bt=0x%x\n\r",__LINE__,bt);
		EEPROM_ReadStatus(file_i2c,addr,&bt);
	}
}

void EEPROM_PageRead(int file_i2c,int addr,uint8_t *buf,int sector_address) {
	int i;
	uint8_t bt;

	i2c_ioctl_smbus_write (file_i2c, addr, 0xE0, 0x00);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, 0x03);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, (sector_address>>8)&0xff);
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, (sector_address>>0)&0xff);
	for(i=0;i<128;i++) {
		//i2c_ioctl_smbus_write (file_i2c, addr, 0xE1, 0x0);
	        i2c_ioctl_smbus_read  (file_i2c, addr, 0xE1, &buf[i]);	

	}
	i2c_ioctl_smbus_write (file_i2c, addr, 0xE0, 0x01); // # CS HIGH
}

unsigned int crc32b(uint8_t *message,int len) {
   int i, j;
   unsigned int byte, crc, mask;

   i = 0;
   crc = 0xFFFFFFFF;
   while (len-->0) {
      byte = message[i];            // Get next byte.
      crc = crc ^ byte;
      for (j = 7; j >= 0; j--) {    // Do eight times.
         mask = -(crc & 1);
         crc = (crc >> 1) ^ (0xEDB88320 & mask);
      }
      i = i + 1;
   }
   return ~crc;
}

unsigned int EEPROM_GetHash(int file_i2c,int chip_addr){
	int i = 15*128;
	uint8_t t_buf[256];

	EEPROM_PageRead(file_i2c,chip_addr,t_buf,i);
	uint32_t hash = crc32b(t_buf,128);
	return hash;

}

void EEPROM_Program(int chip_addr,int file_i2c, int sz){
	uint8_t bt;
	int i,j;
	uint8_t t_buf[256];

        i2c_ioctl_smbus_read  (file_i2c, chip_addr, 0x0, &bt);	
	bt |= 0x01; // Switch SPI Master to EEPROM mode
	i2c_ioctl_smbus_write (file_i2c, chip_addr, 0x0, bt);
	
	uint32_t hash = EEPROM_GetHash(file_i2c,chip_addr);
	printf("Fingerprint: %X\n",hash);
	switch(hash){
		case 0x35F0AD07:
			printf("Detected rev07\n");
		break;
		case 0x5FB925C9:
			printf("Detected rev06\n");
		break;
		default:
			printf("Unknown firmware\n");
	}
}


int main(int argc, char** argv)
{
    int file_i2c,i;
    int length;
    int bin_file;	
    int chip_addr,sz;
    uint8_t bt;
    int spi_prog;
    int eeprom_prog;

    unsigned char buffer[60] = {0};
    char fname[64];

    if((argc != 3) ||(strlen(argv[1])>32)) {
	    printf("Usage: \n\r");
	    printf("tofino_ber i2c_bus chip_addr\n\r");
	    exit(1);
    }

    //----- OPEN THE I2C BUS -----
    //char *filename = (char*)"/dev/i2c-0";
    strcpy(fname,(char*)"/dev/i2c-");
    strcat(fname,argv[1]);
    printf("I2C DEV: %s\n\r",fname);

    if ((file_i2c = open(fname, O_RDWR)) < 0)
    {
	//ERROR HANDLING: you can check errno to see what went wrong
	printf("Failed to open the i2c bus\n\r");
	return -1;
    }
    sscanf(argv[2],"%x",&chip_addr);
    if(chip_addr<0 || chip_addr > 0x70) {
	printf("Bad chip addr = 0x%x\n\r",chip_addr);
	return -1;
	}
    	else printf("Chip addr: 0x%x\n\r",chip_addr);
    int ret = ioctl (file_i2c, I2C_SLAVE_FORCE, chip_addr);
    if (ret < 0)
    {
        ret = -errno;
	printf("%d ",__LINE__);perror("smbus: ");
        exit(1);
    }

	
   EEPROM_Program(chip_addr,file_i2c,sz);

    	                     /*
    if (ioctl(file_i2c, I2C_SLAVE, chip_addr) < 0)
    {
	printf("Failed to acquire bus access and/or talk to slave.\n");
	//ERROR HANDLING; you can check errno to see what went wrong
	return -1;
    }
                               */

	close(file_i2c);
    return 0;
}
 
