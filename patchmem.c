#define _GNU_SOURCE
#include <string.h>

#include <stdio.h>
#include <stdlib.h>

#include <math.h>
#include <sys/ptrace.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

void usage(char** argv);
unsigned char* hex2bytes( const unsigned char *hexstring, unsigned int len);

int main(int argc, char** argv){

int i=0;

if(argc !=4){
	usage(argv);
	return -1;
}

int pid = atoi(argv[1]);
unsigned long startAddress=strtoul(argv[2], NULL, 16);
unsigned char* inputBytes=NULL;

if( (inputBytes = hex2bytes( (const unsigned char*)argv[3], strlen(argv[3]) ) ) == NULL ){
	usage(argv);
	return -2;
}
int numInputBytes = ( strlen( argv[3] ) / 2 );

printf("input bytes =\n");
for(i=0; i<numInputBytes; i++)
	printf("%02X\n", inputBytes[i]);

int mem_fd;
unsigned char buf[_SC_PAGE_SIZE];

//20 should be biggest ever
char mem_file_name[20];
sprintf(mem_file_name, "/proc/%d/mem", pid);

//open the memory of the image and pause it's execution
mem_fd = open(mem_file_name, O_RDONLY);
ptrace(PTRACE_ATTACH, pid, NULL, NULL);
waitpid(pid, NULL, 0);

//unsigned long offsetEditStart = 0x8048474u+0x14u;
//long type should be the word size
int numWordsToWrite = ceil( (double)numInputBytes/(double)sizeof(long) );
printf("num words to write %d\n", numWordsToWrite);
int numTotalBytesToWrite = numWordsToWrite * sizeof(long);
unsigned char writeByteBuf[ numTotalBytesToWrite ];

//ptrace writes 1 word per call, so on x64 means 8 bytes
//Read the extra bytes
int deltaBytes = numTotalBytesToWrite-numInputBytes;
lseek(mem_fd, startAddress+deltaBytes, SEEK_SET);

//I don't think read can write in reverse
unsigned char* endBytesStart=(writeByteBuf+numInputBytes);
read(mem_fd, endBytesStart, deltaBytes );

memcpy(writeByteBuf, inputBytes, numInputBytes);

printf("total bytes to write %d  @ %X - %X\n", numTotalBytesToWrite, startAddress, startAddress+numTotalBytesToWrite);

for(i=0; i<numTotalBytesToWrite; i++)
	printf("%02X ", writeByteBuf[i]);
printf("\n");

//long* writeBufPtr = (*((long*)writeByteBuf));
long* writeBufPtr = (long*)writeByteBuf;
long ptraceRet;

for(i=0; i<numWordsToWrite; i++){
	printf("poking value %X @ %X\n", writeBufPtr[i], startAddress);
	ptraceRet = ptrace (PTRACE_POKEDATA, pid, startAddress, writeBufPtr[i]);
	startAddress += sizeof(long);

	if( ptraceRet < 0 ){
		printf("fatal: ptrace return code %d\n", ptraceRet);
		return -3;
	}
}

printf("bytes written at %X\n", startAddress);

for(i=0; i<numTotalBytesToWrite; i++)
	printf("%02X ", writeByteBuf[i]);

printf("\n");

//just to be proper
close(mem_fd);
ptrace(PTRACE_DETACH, pid, NULL, NULL);

return 0;

}

void usage(char** argv){
	printf("Usage:\n	%s <pid> <Start Address in hex> <Bytes, \"E8C29090\">\n", argv[0]);
	printf("Ex:\n	%s 1234 0xdeadbeef \"90E8C29090\"\n", argv[0]);
	printf(" 	Writes the 5 bytes into 0xdeadbeef to 0xdeadbef4\n");
}

unsigned char* hex2bytes(const unsigned char *hexstring, unsigned int len)
{
	//needs to be even
	if(len % 2)
		return -1;

	//2 ascii hex chars per byte
	unsigned char* bytes = NULL;

	int byteSize = len/2;
	bytes = (unsigned char*)malloc(byteSize);
	if(bytes == NULL)
		printf("malloc error\n");

	int i=0;
	for (;i<byteSize;++i) {
	  int value;
	  sscanf(hexstring+2*i,"%02x",&value);
//	printf("%02X", value);
	  bytes[i] = value;
//	printf("%02X ", *bytes[i]);
	}

	return bytes;
/*
int j;
*/
}
