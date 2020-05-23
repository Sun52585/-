/*
	NAME
       open and possibly create a file or device

SYNOPSIS
       #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>   //for open

       int open(const char *pathname, int flags);
       int open(const char *pathname, int flags, mode_t mode);

	   file  descriptor:�ļ�������
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>  //for close
#include <sys/mman.h>
#include <linux/input.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <pthread.h>
int *plcd = NULL;

char wo[] = {
	0x04,0x40,0x0E,0x50,0x78,0x48,0x08,0x48,0x08,0x40,0xFF,0xFE,0x08,0x40,0x08,0x44,
0x0A,0x44,0x0C,0x48,0x18,0x30,0x68,0x22,0x08,0x52,0x08,0x8A,0x2B,0x06,0x10,0x02
};

//��ʼ������
int init_serial(const char *file, int baudrate)
{ 
	/*���ô���
	  ������:19200
	  ����λ:8
	  У��λ:��Ҫ
	  ֹͣλ:1
	  ����������:��
	  */
#if 0
	struct termios
	{
		tcflag_t c_iflag;       /* input mode flags */
		tcflag_t c_oflag;       /* output mode flags */
		tcflag_t c_cflag;       /* control mode flags */
		tcflag_t c_lflag;       /* local mode flags */
		cc_t c_line;            /* line discipline */
		cc_t c_cc[NCCS];        /* control characters */
		speed_t c_ispeed;       /* input speed */
		speed_t c_ospeed;       /* output speed */
#define _HAVE_STRUCT_TERMIOS_C_ISPEED 1
#define _HAVE_STRUCT_TERMIOS_C_OSPEED 1
	};
#endif
	
	int fd;
	
	fd = open(file, O_RDWR);
	if (fd == -1)
	{
		perror("open device error:");
		return -1;
	}

	struct termios myserial;
	//��սṹ��
	memset(&myserial, 0, sizeof (myserial));
	//O_RDWR               
	myserial.c_cflag |= (CLOCAL | CREAD);
	//���ÿ���ģʽ״̬���������ӣ�����ʹ��
	//���� ����λ
	myserial.c_cflag &= ~CSIZE;   //�������λ
	myserial.c_cflag &= ~CRTSCTS; //��Ӳ��������
	myserial.c_cflag |= CS8;      //����λ:8

	myserial.c_cflag &= ~CSTOPB;//   //1λֹͣλ
	myserial.c_cflag &= ~PARENB;  //��ҪУ��
	//myserial.c_iflag |= IGNPAR;   //��ҪУ��
	//myserial.c_oflag = 0;  //����ģʽ
	//myserial.c_lflag = 0;  //�������ն�ģʽ

	switch (baudrate)
	{
		case 9600:
			cfsetospeed(&myserial, B9600);  //���ò�����
			cfsetispeed(&myserial, B9600);
			break;
		case 115200:
			cfsetospeed(&myserial, B115200);  //���ò�����
			cfsetispeed(&myserial, B115200);
			break;
		case 19200:
			cfsetospeed(&myserial, B19200);  //���ò�����
			cfsetispeed(&myserial, B19200);
			break;
	}
		/* ˢ���������,��������ܵ����� */
	tcflush(fd, TCIFLUSH);

	/* �ı����� */
	tcsetattr(fd, TCSANOW, &myserial);

	return fd;
}

void lcd_draw_point(int x0, int y0, int color)
{
	*(plcd + y0*800 + x0) = color;
}

//�ڶ���Ϊx0,y0��λ�ÿ�ʼ��ʾһ����ɫ���Σ����εĿ�//��Ϊw, �߶�Ϊh,��ɫΪcolor
void lcd_draw_rect(int x0, int y0, int w, int h, int color)
{
	int x, y;
	for (y = y0; y < y0+h; y++)
	{
		for (x = x0; x < x0+w; x++)
			lcd_draw_point(x, y, color);
	}
}

//��lcd���color��ɫ
void lcd_clean_screen(int color)
{
	int x, y;
	for (y = 0; y < 480; y++)
	{
		for (x = 0; x < 800; x++)
			lcd_draw_point(x, y, color);
	}
}

//��ʾһ��24λ��bmpͼƬ��ͼƬ�Ŀ�ȱ���Ϊ4�ı��� 
void lcd_draw_bmp(char *name, int x0, int y0, int w, int h)
{
	int fd;
	fd = open(name, O_RDWR);
	if (-1 == fd)
	{
		printf("open %s error!\n", name);
		
		return ;
	}
	
	//1������ǰ54��byte
	lseek(fd, 54, SEEK_SET);
	char ch[w*h*3];
	read(fd, ch, sizeof(ch));
	close(fd);
	
	//lcd �����ص�ռ4byte�� 24λbmp 3byte
	int color;
	char r, g, b;
	//λ����
	int x, y, i = 0;
	for (y = 0; y < h; y++)
	{
		for (x = 0; x < w; x++)
		{
			b = ch[i];
			g = ch[i+1];
			r = ch[i+2];
			i += 3;
			color = (r << 16) | (g << 8) | b;
			lcd_draw_point(x+x0, h-y+y0, color);
		}
	}
}

//��������������ȡ������x�����y����
void monitor_touch()
{
	int fd;
	fd = open("/dev/event0", O_RDWR);
	if (-1 == fd)
		return ;
	
	struct input_event ev;
	int x, y;
	while (1)
	{
		read(fd, &ev, sizeof(ev));
		//����жϸ��¼���һ�������¼�
		if ((ev.type == EV_ABS) && (ev.code == ABS_X))
		{
			x = ev.value;
		}
		else if ((ev.type == EV_ABS) && (ev.code == ABS_Y))
		{
			y = ev.value;
		}
		
		//��ָ����
		if ((ev.type == EV_ABS) && (ev.code == ABS_PRESSURE) && (ev.value == 0))
		{
			printf("x: %d, y: %d\n", x, y);
			
		}
	}
	
	close(fd);
}

int main(int argc, char *argv[])
{
	int fd;
	//O_RDWR: �ɶ���д
	fd = open("/dev/fb0", O_RDWR);
	// if(fd == -1)
	//if (fd = -1)
	if (-1 == fd)
	{
		printf("open dev/fb0 error !\n");
		
		return 0;
	}
	
	//�ڴ�ӳ�䣬��һ���ļ������豸ӳ�䵽һ�����̵�///��ַ�ռ��У��ڴ棩���ڽ����в������ӳ�����///����൱�ڲ��� ��ӳ����ļ�
	//plcd �洢��ӳ��ĵ�ַ�ռ���׵�ַ
	plcd = mmap(NULL, 800*480*4, 
			PROT_READ | PROT_WRITE,
			MAP_SHARED,
			fd,
			0);
	
	//��ΰ�lcd�ϵĵ�n�����ص� ��ʾ�ɺ�ɫ��
	//*(plcd + n) = 0x00ff0000;
	
	//��ΰ�����Ϊ(x, y)�����ص㣬��ʾ�ɺ�ɫ��
	//*(plcd + y*800+x) = 0x00ff0000;
	
	//��ΰ�������Ļȫ����ʾ��ɫ��
	//ѭ��
	lcd_clean_screen(0xFFFFFFFF);
	
	//�����ʾһ������
	lcd_draw_rect(50, 50, 100, 100, 0x00ff0000);
	
	//��ʾBMPͼƬ
	lcd_draw_bmp("music.bmp", 100, 100, 240, 240);
	
	//��ʼ������
	int com_fd = init_serial("/dev/s3c2410_serial3", 9600);
	
	monitor_touch();
	
	close(fd);
	
	return 0;
}