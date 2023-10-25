
#include "stdio.h"
#include "stdlib.h"

#define BUFF_SIZE 1024*1024*1
static unsigned char file_buff[ BUFF_SIZE ] ; 
static unsigned char rcv_buff[ BUFF_SIZE ] ; 
  int file2buff(unsigned char *file_buff,  char *fn){
	int r,i ;
	FILE *fp ; 
	fp = fopen( fn , "rb" );	
	if (fp==NULL) printf("can not open file (%s)\n",fn);
	if (fp==NULL) {return 0;exit(1); } 
	r = fread( file_buff , 1, BUFF_SIZE ,fp ); 
	fclose (fp); 
	printf("%s get file size is %d\r",fn, r ); 
	return r ;	
}
 






///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


 



int  if_dev_valid (unsigned char dev_addr ){ //   for 24C02 a2-0=0 ,dev_addr = 0x50 ;
	int r = I2C_start(0,dev_addr,1) ;
	printf(" \n if_dev_valid 0X%02x => %s \n",dev_addr,(r==0)?"OK":"NV");
//	getchar();
	if (r==0)  return 0;
	else return 1 ;
}

 
int burst_write02 (unsigned int base ,unsigned char dev_addr,unsigned char  start_address ,unsigned char *buff,unsigned char size ){
unsigned char r;
unsigned char i;
	while(I2C_start(base,dev_addr,0)) ;
	I2C_write(base ,start_address,0) ;
	for(i=0;i<size;++i){
		if ((size-i)==1)I2C_write(base,buff[i],1) ;else I2C_write(base ,buff[i],0) ;
	}
}


unsigned char  randm_read02 (unsigned int base ,unsigned char dev_addr,unsigned char  byte_addr  ){
unsigned char r;
while(I2C_start(base,dev_addr,0)) ;
I2C_write(base,byte_addr,0);
while(I2C_start(base,dev_addr,1)) ;
r=	I2C_read(base,1);
return r;
} 






int  burn_24c02 (unsigned char dev_addr ,unsigned  char * p , unsigned char len ){
unsigned char off = 0  ;
while(len>=8){
burst_write02(0,dev_addr,off,p+off,8);  
off+=8;
len-=8;
}
if (len) burst_write02(0,0x50,off,p+off,len);  
}



int readback_24c02 (unsigned char dev_addr,unsigned char len  , unsigned char *p ){
int i ;for(i=0;i<len;++i)p[i] = randm_read02(0,dev_addr,i );
}






//write  24c02 eeprom 
int prog_one_byte_02 (unsigned char  dev_addr , unsigned char off , unsigned char v ){ //test ok 
static unsigned char t[2] ; t[0]=v;
 burst_write02 (0 ,dev_addr,off ,t ,1 );
t[1] = randm_read02(0,dev_addr,off);
printf(" prog_one_byte_02 :dev_addr = 0x%02x , offsite = 0x%02x , data = 0x%02x  =>  %s\n", dev_addr,off,v,(t[0]==t[1]) ? "OK" :"ERROR");
if (t[0] == t[1]) return 1; else return 0;
}
 



 





int burst_write256 (unsigned int base ,unsigned char dev_addr,unsigned short   start_address ,unsigned char *buff,unsigned char size ){
unsigned char r;
unsigned char i;
unsigned short  t = start_address ;
unsigned char  hi,lo;lo = t & 0xff ; t>>=8;hi = t & 0xff ;
	while(I2C_start(base,dev_addr,0)) ;
	I2C_write(base ,hi,0) ;
	I2C_write(base ,lo,0) ;
	for(i=0;i<size;++i){
		if ((size-i)==1)I2C_write(base,buff[i],1) ;else I2C_write(base ,buff[i],0) ;
	}
}


unsigned char  randm_read256 (unsigned int base ,unsigned char dev_addr,unsigned short   byte_addr  ){
unsigned char r;
unsigned short  t = byte_addr ;
unsigned char  hi,lo;lo = t & 0xff ; t>>=8;hi = t & 0xff ;
while(I2C_start(base,dev_addr,0)) ;
I2C_write(base,hi,0);
I2C_write(base,lo,0);
while(I2C_start(base,dev_addr,1)) ;
r =	I2C_read(base,1);
return r;
} 




int  burn_24c256 (unsigned char dev_addr ,unsigned  char * p , unsigned short  len ){
unsigned short  off = 0  ;
while(len>=8){
burst_write256(0,dev_addr,off,p+off,8);  
off+=8;
len-=8;
}
if (len) burst_write256(0,0x50,off,p+off,len);  
}



int readback_24c256 (unsigned char dev_addr,unsigned short  len  , unsigned char *p ){
int i ;for(i=0;i<len;++i)p[i] = randm_read256(0,dev_addr,i );
}


//write  24c256 eeprom 
int prog_one_byte_256 (unsigned short  dev_addr , unsigned short  off , unsigned char v ){ //tested ok 
static unsigned char t[2] ; 
t[0]=v; 
burst_write256 (0 ,dev_addr,off ,t ,1 ) ;
t[1] = randm_read256(0,dev_addr,off);
printf(" prog_one_byte_256 :dev_addr = 0x%02x , offsite = 0x%04x , data = 0x%02x  =>  %s\n", dev_addr,off,v,(t[0]==t[1]) ? "OK" :"ERROR");
if (t[0] == t[1]) return 1; else return 0;
}
 




 #define I2C_DEV_ADDR 0X50
 #define FILE_NAME "aa.bin"

int run_main (){
	int len;
	int i ;
	int  err = 0 ; 
	IOInit(2,115200);
	
	check_io();
	
//	getchar();
	I2C_init(0,100*1000*1000,10*1000);
  	len = file2buff(file_buff , FILE_NAME);
  	if_dev_valid(0x50);
  ///	if_dev_valid(0x51);
	  
	//for(i=0;i<len;++i)   prog_one_byte_02( 0x50, i,file_buff[i]) ;
	for(i=0;i<len;++i)   prog_one_byte_256( 0x50, i,file_buff[i]) ;
//	getchar(); 
	exit(1);
 
  	printf("Program done ...\r\n");
  	CloseCom();
  	
}


