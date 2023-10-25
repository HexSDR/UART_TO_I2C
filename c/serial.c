////////////////////////////////////////////////////////
// RS-232 example
// Compiles with Microsoft Visual C++ 5.0/6.0
// (c) fpga4fun.com KNJN LLC - 2003, 2004, 2005, 2006

#include <windows.h>
#include <stdio.h>
#include <conio.h>

////////////////////////////////////////////////////////
HANDLE hCom;

void OpenCom(unsigned int port ,unsigned int baud )
{
	static   char b[10];
	DCB dcb;
	COMMTIMEOUTS ct;
	sprintf(b,"COM%d:",port);
	puts(b); 
	hCom = CreateFile(b, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		puts("00000000000000\n");
	if(hCom==INVALID_HANDLE_VALUE) exit(1);
	
		puts("1111111111111111\n");
	if(!SetupComm(hCom, 4096, 4096)) exit(1);
	
		puts("22222222222\n");
	if(!GetCommState(hCom, &dcb)) exit(1);
 
	dcb.BaudRate = baud ;
	((unsigned int *)(&dcb))[2] = 0x1001;  // set port properties for TXDI + no flow-control
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = 2;
	if(!SetCommState(hCom, &dcb)) exit(1);	// set the timeouts to 0
	ct.ReadIntervalTimeout = MAXDWORD ;
	ct.ReadTotalTimeoutMultiplier = 0;
	ct.ReadTotalTimeoutConstant = 0;
	ct.WriteTotalTimeoutMultiplier = 0;
	ct.WriteTotalTimeoutConstant = 0;
	if(!SetCommTimeouts(hCom, &ct)) exit(1);
	printf("ok to open %s with %d \n",b,baud  );
}

void CloseCom()
{
	CloseHandle(hCom);
}
static 
unsigned int  WriteCom(unsigned char* buf, int len)
{
	unsigned int  nSend;
	if(!WriteFile(hCom, buf, len, &nSend, NULL)) exit(1);
	return nSend;
} 

static 
void WriteComChar(unsigned char b)
{
	WriteCom(&b, 1);
}

static 
unsigned char  ReadCom(unsigned char *buf, int len)
{
	unsigned char  nRec;
	if(!ReadFile(hCom, buf, len, &nRec, NULL)) exit(1);
	return (unsigned char )nRec;
}

static 
unsigned char ReadComChar()
{
	unsigned int  nRec;	char c;  
	do {
			if(!ReadFile(hCom, &c, 1, &nRec, NULL)) exit(1);
		 
	}
	while (nRec==0    ) ; 
	return   c  ;
}

////////////////////////////////////////////////////////

 
int do_cmd_wr8(unsigned char off, unsigned int u8 ){       //  20 
	static unsigned char tmp[10] ;
	int i=0 ;
	tmp[i++] = 0x55;
	tmp[i++] = 0xaa ;
	tmp[i++] = 0x5a  ;
	tmp[i++] = off   ;
	tmp[i++] = 20 ;//burst write 
	tmp[i++] = u8 ;
	WriteCom( tmp,i );	
}

unsigned char do_cmd_rd8 (unsigned char  off ){ //  30
	static  unsigned char  tmp[10];
	unsigned  int i=0 ;
	tmp[i++] = 0x55;
	tmp[i++] = 0xaa ;
	tmp[i++] = 0x5a  ;
	tmp[i++] = off   ;
	tmp[i++] = 30  ; 
	WriteCom( tmp,i );
	tmp[0] = ReadComChar(); 
	return tmp[0]  ; 	
}
 
 
 
 
void IOWR  (unsigned int base,unsigned char off ,unsigned char val ){
//	printf("IOWR\n" );
	do_cmd_wr8(off,val);
}

unsigned char IORD(unsigned int base ,unsigned char off){
//	printf("IORD\n" );
	return do_cmd_rd8(off);
}


 int check_io(){
 unsigned char r ;
 IOWR(0,0,0xaa) ;
 r = IORD(0,0) ;
 printf("read back as %x \n",r); 
 }
void IOInit(unsigned int port ,unsigned int baud){
OpenCom(port , baud ) ;
check_io();
}
 
 
