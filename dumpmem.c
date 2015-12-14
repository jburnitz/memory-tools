#include <stdio.h>
#include <stdlib.h>

#include <sys/ptrace.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define CNORMAL  "\x1B[0m"
#define CRED  "\x1B[31m"

void usage(char **argv);

int main(int argc, char **argv)
{
	int boolPrettyPrint = 1;
	unsigned int WIDTH = 0x20u;

	if (argc < 4) {
		usage(argv);
		return -1;
	}

	//check if the user wants raw output for use in like xxd
	int arg_idx = 1;
	if (strcmp(argv[arg_idx], "-r") == 0 || strcmp(argv[arg_idx], "--raw-output") == 0) {
		boolPrettyPrint = 0;
		arg_idx++;
	}

	//interpret address as hex
	unsigned long offsetBegin = strtoul(argv[arg_idx++], NULL, 16);
	unsigned long offsetEnd = strtoul(argv[arg_idx++], NULL, 16);

	//pid is always the last argument
	unsigned int pid = atoi(argv[arg_idx]);

	char mem_file_name[80];
	int mem_fd;

	//most efficient to read page by page
	unsigned char buf[_SC_PAGE_SIZE];

	sprintf(mem_file_name, "/proc/%d/mem", pid);

	//read the memory image now
	mem_fd = open(mem_file_name, O_RDONLY);

	//pauses the process sends SIG_STOP
	ptrace(PTRACE_ATTACH, pid, NULL, NULL);
	waitpid(pid, NULL, 0);

	int i = 0;
	unsigned int offsetItr;

	//first lets start with a 0x....0 offset
	offsetBegin = (offsetBegin - (offsetBegin & 0xFF));

	//gives contexual information to the output
	if (boolPrettyPrint) {
		//header
		printf("%15s", CRED);

		for (i = 0; i < WIDTH; i++)
			printf("%.2X ", i);

		printf("%s", CNORMAL);

		for (offsetItr = offsetBegin; offsetItr < offsetEnd; offsetItr += WIDTH) {
			lseek(mem_fd, offsetItr, SEEK_SET);
			read(mem_fd, buf, WIDTH);

			printf("\n%#x: ", offsetItr);

			for (i = 0; i < WIDTH; i++)
				printf("%.2X ", buf[i]);

			 for (i = 0; i < WIDTH; i++)
				if(isprint((char) buf[i]))
	                                printf("%c", buf[i]);
				else
					printf(".");
		}
		//footer
		printf("\n%15s", CRED);
		for (i = 0; i < WIDTH; i++)
			printf("%.2X ", i);
		printf("%s\n", CNORMAL);
	} else {
		//if not pretty its ugly
		for (offsetItr = offsetBegin; offsetItr < offsetEnd; offsetItr += WIDTH) {
			lseek(mem_fd, offsetItr, SEEK_SET);
			read(mem_fd, buf, WIDTH);

			fwrite(buf, 1, WIDTH, stdout);
		}
	}

	//just to be proper
	close(mem_fd);

	return 0;
}

void usage(char **argv)
{
	printf("Usage: %s [-r|--raw-output] <Start Address in hex> <End Address in hex> <PID>\n", argv[0]);
}
