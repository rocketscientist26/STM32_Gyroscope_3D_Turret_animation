//gyroscope.cpp
#include "gyroscope.h"
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

void Gyroscope::gyroscope_init(){
	strcpy(write_buffer,"R\n");
	strcpy(read_buffer,"EMPTY\n");

	fd = open("/dev/FTDI0",O_RDWR | O_NOCTTY);
	if(fd == -1){/*ERROR*/}
	struct termios SerialPortSettings;
	tcgetattr(fd, &SerialPortSettings);
	cfsetispeed(&SerialPortSettings,B230400);
	cfsetospeed(&SerialPortSettings,B230400);
	SerialPortSettings.c_cflag &= ~PARENB;
	SerialPortSettings.c_cflag &= ~CSTOPB;
	SerialPortSettings.c_cflag &= ~CSIZE;
	SerialPortSettings.c_cflag |=  CS8;
	SerialPortSettings.c_cflag &= ~CRTSCTS;
	SerialPortSettings.c_cflag |= CREAD | CLOCAL;
	SerialPortSettings.c_iflag &= ~(IXON | IXOFF | IXANY);
	SerialPortSettings.c_iflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	SerialPortSettings.c_oflag &= ~OPOST;
	if((tcsetattr(fd,TCSANOW,&SerialPortSettings)) != 0){/*ERROR*/}

	SerialPortSettings.c_cc[VMIN] = 22;
	SerialPortSettings.c_cc[VTIME] = 0;
}
void copy_array_an_to_bn(char *source, char *destination, int count, int aposition, int bposition){
    int i = 0;
    while (i != count){
        destination[bposition+i] = source[aposition+i];
        i++;
    }
}
char tmp2[40];
int Gyroscope::gyroscope_get_X(){
	write(fd,write_buffer,2);
	tcflush(fd, TCIFLUSH);
	read(fd,&read_buffer,22);
	read_buffer[22] = 0;

	copy_array_an_to_bn(read_buffer,tmp2,3,4,0);
	tmp2[3] = 0x00;
	X = (int)strtol(tmp2, NULL, 10);
	copy_array_an_to_bn(read_buffer,tmp2,3,10,0);
	tmp2[3] = 0x00;
	Z = (int)strtol(tmp2, NULL, 10);
	copy_array_an_to_bn(read_buffer,tmp2,3,16,0);
	tmp2[3] = 0x00;
	Y = (int)strtol(tmp2, NULL, 10);

	return X;
}
int Gyroscope::gyroscope_get_z(){
	return Z;
}
int Gyroscope::gyroscope_get_y(){
	return Y;
}
void Gyroscope::gyroscope_reset(){
	write_buffer[0]= 'T';
	tcdrain(fd);
	write(fd,write_buffer,2);
	tcflush(fd, TCOFLUSH);
	tcdrain(fd);
	write_buffer[0]= 'R';
}
void Gyroscope::_bind_methods(){
	ObjectTypeDB::bind_method("gyroscope_init",&Gyroscope::gyroscope_init);
	ObjectTypeDB::bind_method("gyroscope_get_X",&Gyroscope::gyroscope_get_X);
	ObjectTypeDB::bind_method("gyroscope_get_z",&Gyroscope::gyroscope_get_z);
	ObjectTypeDB::bind_method("gyroscope_get_y",&Gyroscope::gyroscope_get_y);
	ObjectTypeDB::bind_method("gyroscope_reset",&Gyroscope::gyroscope_reset);
}
Gyroscope::Gyroscope(){
}
