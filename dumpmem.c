#include <stdio.h>
#include <stdlib.h>

#include <sys/ptrace.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define CNORMAL  "\x1B[0m"
#define CRED  "\x1B[31m"

void usage(char** argv);

int main(int argc, char** argv){

int boolPrettyPrint = 1;
char* formatStringData = "%.2X ";
unsigned int WIDTH = 0x20u;

//pid is always the last argument
int pid = atoi(argv[argc-1]);

//check if the user wants raw output for use in like xxd
if(strcmp(argv[1],"-r") == 0 || strcmp(argv[1],"--raw-output") == 0 ){
	boolPrettyPrint=0;
	formatStringData = "";
}

long offsetBegin=strtoul(argv[2], NULL, 16);
long offsetEnd=strtoul(argv[3], NULL, 16);

char mem_file_name[80];
int mem_fd;
unsigned char buf[_SC_PAGE_SIZE];

sprintf(mem_file_name, "/proc/%d/mem", pid);

//read the image now
mem_fd = open(mem_file_name, O_RDONLY);

//pauses the process sends SIG_STOP
ptrace(PTRACE_ATTACH, pid, NULL, NULL);
waitpid(pid, NULL, 0);

int i=0;
long offsetItr;

if(boolPrettyPrint){
	//printing out bytes, page by page
	printf("%s", CRED);
	
	for(i=0; i<WIDTH; i++)
		printf((const char*)formatStringData, i);
	printf("\n\n");
	printf("%s", CNORMAL);
}
//first lets start with a 0x....0 offset
offsetBegin = (offsetBegin - (offsetBegin & 0xFF) );

for(offsetItr=offsetBegin; offsetItr < offsetEnd; offsetItr+=WIDTH ){

	lseek(mem_fd, offsetItr, SEEK_SET);
	read(mem_fd, buf, WIDTH);

	for(i=0; i<WIDTH; i++){
		printf((const char*)formatStringData, buf[i]);
	}

	//I know conditionals inside loops are bad...
	if(boolPrettyPrint){
		printf(" : 0x%X\n", offsetItr);
	}
}

if(boolPrettyPrint){
	printf("\n");
	printf("%s", CRED);
	for(i=0; i<WIDTH; i++)
		printf("%.2X ", i);
	printf("%s", CNORMAL);
	
	printf("\n");
}
//just to be proper
close(mem_fd);

return 0;

}

void usage(char** argv){
	printf("Usage: %s [-r|--raw-output] <Start Address in hex> <End Address in hex> <PID>\n", argv[0]);
}
