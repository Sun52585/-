/*

1. �򿪻�ر�һ���ļ�
		
	NAME
       open,  creat   open and possibly create a file or  device

	SYNOPSIS
       #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>

		open������һ���ļ�(�򴴽�һ���ļ���������)
       int open(const char *pathname, int flags);
       int open(const char *pathname, int flags, mode_t mode);
		pathname: Ҫ�򿪻򴴽����ļ���(��·����)
		flags: �򿪱�־������ϵͳ�����Ժ��ַ�ʽ������ļ�
				O_RDONLY: read only
				O_WRONLY: write only
				O_RDWR: read/write
					������������־ѡ��һ��
				O_APPEND: append ׷�ӷ�ʽ
				O_CREAT: create ������־(����ļ��������򴴽�)
       			O_EXCL: �ñ�־һ���O_CREAT���ʹ�ã����������ļ��Ƿ���ڡ�
						ָ��O_EXCL | O_CREAT ������ļ����ڣ���openʧ�ܣ�����
						errno == EEXIST
       			O_NONBLOCK: non block() ��������ʽ���ļ���
       					�ñ�־������ã���Ϊ��������ʽ
       					�ñ�־��������ã���Ϊ������ʽ
       			O_TRUNC: truncate ,�ض̱�־��
       					�����ļ����ڣ�������һ����ͨ�ļ������Ҵ򿪷�ʽ
       					ΪO_RDWR/O_WRONLY,���ļ����ݻᱻ��ա�
       			....
       			���ϱ�־����"��"�����

       	���ڶ�������ָ����O_CREAT(������־),
		��ô����������modeָ���������ļ���Ȩ��
		mode: ����ָ���´������ļ���Ȩ�ޡ������ַ�ʽָ��:
			(1) user: S_IRUSR S_IWUSR S_IXUSR  -> S_IRWXU == S_IRUSR  | S_IWUSR  |S_IXUSR
			      group: S_IRGRP S_IWGRP S_IXGRP -> S_IRWXG == S_IRGRP | S_IWGRP | S_IXGRP
			      other: S_IROTH S_IWOTH S_IXOTH -> S_IRWXO == S_IROTH | S_IWOTH | S_IXOTH
			(2) 0660

			����ֵ:
				����ɹ������ļ�������(>0,�������ж��ļ��Ĳ���������
				ͨ��������Ϊ����������ļ���)
				ʧ�ܷ���-1,  ����errno������


       int creat(const char *pathname, mode_t mode);
       <=> open(pathname, O_CREAT | O_WRONLY | O_TRUNC, mode);


       =====
	NAME
       close - close a file descriptor

	SYNOPSIS
       #include <unistd.h>

		�ر�һ���ļ�������
       int close(int fd);
		
2. ��/дһ���ļ�
	NAME
     read - read from a file

SYNOPSIS
       #include <unistd.h>

       		read������fdָ�����ļ��ж�ȡnbyte���ֽ����ݵ�bufָ��
       		�Ŀռ���ȥ��
       ssize_t read(int fd, void *buf, size_t nbyte);
       	fd:�ļ�������(open����ֵ),��ʾ���ĸ��ļ������
       	buf:һ��ָ�룬ָ��һ���ڴ棬����������ļ��������������
       	nbyte:�������ֽ�����.
       	����ֵ:
       		> 0 : �ɹ���ȡ�����ֽ������ļ�ƫ����(���)��֮����
       		= 0: �����ļ�β��(�ļ�������)
       		< 0(-1) :�����ˡ�ͬʱerrno������
	===
	NAME  b 
    write - write on a file

	SYNOPSIS
       #include <unistd.h>

		write������bufָ����ڴ�ռ�������nbyte���ֽ�����д��
		fdָ�����ļ���ȥ��
       ssize_t write(int fd, const void *buf, size_t nbyte);
       		fd:�ļ�������(open����ֵ),��ʾд���ĸ��ļ���ȥ
       		buf:ָ�룬ָ��Ҫд���ļ���ȥ������
       		nbyte:д�����ֽ�
       	����ֵ:
       		> 0 :�ɹ�д���ļ���ȥ���ֽ������ļ�ƫ������֮����
       		=0: ʲôҲûд
       		-1:����ͬʱerrno������

3. ��λ
	lseek
	
NAME
       lseek - reposition read/write file offset

SYNOPSIS
       #include <sys/types.h>
       #include <unistd.h>

		lseek������ָ���ļ���λ���.(����ƫ����)
       off_t lseek(int fd, off_t offset, int whence);
       	fd:�ļ���������Ҫ��λ���ļ�
       	offset:Ҫ���õ�ƫ����
       	whence: ��λ��ʽ,������:
       		SEEK_SET: �����ļ���ͷ��λ
       				�µĹ��λ��Ϊ = �ļ���ͷ��+��offset(����< 0)
       		SEEK_CUR:���ڵ�ǰ���λ�ö�λ
       				�µĹ��λ��Ϊ= ��ǰλ�á�+ offset(�����ɸ�)
       		SEEK_END:�����ļ�ĩβ��λ
       				�µĹ��λ��Ϊ=�ļ�ĩβ��+ offset(�����ɸ�)
       	����ֵ:
       		�ɹ������µĹ��λ��������ļ���ͷ��ƫ����(���ֽ�Ϊ��λ)
       		ʧ�ܷ���-1
 4. mmap/munmap

 NAME
       mmap, munmap - map or unmap files or devices into memory
       		mmamp��һ���ļ����豸ӳ�䵽�ڴ�
       		munmap��ӳ��

SYNOPSIS
       #include <sys/mman.h>

       void *mmap(void *addr, size_t length, int prot, int flags,  int fd, off_t offset);
       	addr:���ļ�����ӳ�䵽�ڴ��ĸ���ַ��һ��ΪNULL,�ò���ϵͳ���з��䡣
       	length:Ҫӳ����ļ����ݵĳ��ȡ�����ȡPAGE_SIZE(4K)��������
       	prot:ӳ����ڴ������Ȩ�ޡ�(Ӧ���ļ��򿪵�Ȩ��һ�£���Ϊ����
       		�������ڴ�ʵ���Ͼ��ǲ����ļ�����)
       		PROT_EXEC:��ִ��
       		PROT_READ: �ɶ�
       		PROT_WRITE:��д
       		PROT_NONE:û����Ȩ��
       	 flags:ӳ���־��������ӳ�䲿�ֵĲ����Ƿ���������̿ɼ���
       	 	MAP_SHARED: �������������̿ɼ����ڴ����ֱ��Ӧ�õ��ļ���ȥ��
       	 	MAP_PRIVATE:˽�С����������̲��ɼ����ڴ������Ӧ�õ��ļ���ȥ��
       	 fd:�ļ���������Ҫӳ����ļ�
       	 offset:ƫ��������ʾ���ļ����ĸ�λ�ÿ�ʼӳ�䡣
       	 			offset����ΪPAGE_SIZE(4k)����������
		����ֵ:
			�ɹ�����ӳ���ڴ�������׵�ַ��
			���ʧ�ܷ���MAP_FAILED,ͬʱerrno�����á�


       
       int munmap(void *addr, size_t length);
       		��ӳ�䡣��mmap�෴�Ĳ���
       	�����˳������Զ�unmap
       	���ǹر��ļ�ʱ�������Զ�unmap
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <sys/mman.h>
#include <string.h>

int main()
{  
	int fd;
	int r;

	fd = open("/home/csgec/20151217.txt", O_RDWR | O_CREAT, 0660);
	if (fd < 0)
	{
		//if (errno == EEXIST)
		//{
		//	printf("file exists\n");
		//}
		perror("open failed:");
		return -1;
	}

	#if 1
	char str[10] ;

	r = read(fd, str, 10);
	if (r > 0)
	{
		str[r] = '\0';
		printf("r = %d :%s\n",r, str);
	}


	r = read(fd, str, 10);
	printf("r = %d\n", r);

	#endif

	//r = write(fd, "hello", 5);
	//printf("r = %d\n", r);


	//r = lseek(fd, 0, SEEK_END);
	//printf("r = %d\n" ,r);
	close(fd);

	return 0;
}
