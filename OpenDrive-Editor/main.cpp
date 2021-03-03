// Serial library
#include "include/func.h"
#include "include/serialib.h"
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <cstring>

using namespace std;

const char *help_message = 
"OpenDrive Editor v0.5\n"
"Soniccd123 - 2021\n"
"\n"
"USAGE:\n"
"Device connetion:\n"
"-p <PORT>	- Serial port to use (ex: COM5)\n"
"\n"
"Main arguments:\n"
"-c <OP>		- Flash chip options\n"
"		OP=reset	Reset chip internal controller\n"
"		OP=sr		Show Status Register (SR) contents\n"
"		OP=clear	Clear Status Register contents\n"
"-d <BLOCKS>	- Dump memory to file, needs -m and -f arguments\n"
"		BLOCKS=Number of 512 byte blocks to dump"
"-e		- Erase Flash\n"
"-v <OP>		- Verify memory, needs -m argument\n"
"		OP=compare	Compares memory contents to file, needs -f argument\n"
"		OP=erased	Check if memory is erased\n"
"-w		- Write file to memory, needs -m and -f argument\n"
"\n"
"Acessory arguments:\n"
"-m <MEM>	- Memory to edit\n"
"		MEM=flash	Flash memory is selected\n"
"		MEM=feram	FeRAM (Backup Memory) is selected\n"
"-f <PATH>	- File path\n"
"\n"
"Other:\n"
"-h	- View this message\n";
 
typedef unsigned char BYTE;
char test;
unsigned char buffer[512];

int mode;			//1- Chip options; 2- Dump; 3- Erase chip; 4- Verification; 5- Write file
int memory_mode;	//1- Flash; 2-FeRAM	
int option;
int size;

bool serial_open;	//1- Connected; 0- Not found

serialib serial;
char errorOpening;

char* port;
const char* filePath;

int main(int argc, char *argv[])
{
	int selection;
	char *mem_mode;
	while ((selection = getopt (argc, argv, "p:c:d:ev:wm:f:h")) != -1)
	{
		switch (selection)
		{
		case 'p': //Get Port
			port = optarg;
			errorOpening = serial.openDevice(port, 115200);
			if (errorOpening!=1)
			{
			serial_open = 0;
			printf ("Device not found\n");
			return errorOpening;
			}
			printf ("Serial connected\n");
			serial_open = 1;
			break;
		case 'c':	//Chip options
			mode = 1;
			mem_mode = optarg;
			if (strcmp(mem_mode, "reset") == 0)
			{
				option = 1; //Reset chip controller
			}
			if (strcmp(mem_mode, "sr") == 0)
			{
				option = 2;	//View Status Register
			}
			if (strcmp(mem_mode, "clear") == 0)
			{
				option = 3;	//Clear Status Register
			}
			break;
		case 'd':	//Dump Chip
			mode = 2;
			size = stoi(optarg);
			break;
		case 'e':	//Erase Flash
			mode = 3;
			break;
		case 'v':	//Verification options
			mode = 4;
			mem_mode = optarg;
			if (strcmp(mem_mode, "compare") == 0)
			{
				option = 1;
			}
			if (strcmp(mem_mode, "erased") == 0)
			{
				option = 2;
			}
			break;
		case 'w':
			mode = 5;
			break;

		case 'f':
			filePath = optarg;	
			break;
		case 'm':	//Verification options
			mem_mode = optarg;
			if (strcmp(mem_mode, "flash") == 0)
			{
				memory_mode = 1;
			}
			if (strcmp(mem_mode, "feram") == 0)
			{
				memory_mode = 2;
			}
			break;
		case 'h':
			printf(help_message);
			break;
		default :
			printf(help_message);
			break;

		case -1:
			printf(help_message);
			break;
		}
    }
	
	if (mode == 1 && serial_open == 1)	//Chip options (1)
	{
		switch (option) {
			case 1:	reset_flash();
			break;
			case 2: read_sr();
			break;
			case 3: clear_sr();
			break;
		}
	}
	
	if (mode == 2 && serial_open == 1)	//Erase Flash (2)
	{
		dump();
	}
	
	if (mode == 3 && serial_open == 1)	//Erase Flash (3)
	{
		erase_flash();
	}
	
	if (mode == 4 && option == 1 && serial_open == 1)	//Verify (4) Option (1)
	{
		verify();
	}
	
	if (mode == 4 && option == 2 && serial_open == 1)	//Verify (4) Option (2)
	{
		is_erased();
	}

	if (mode == 5 && serial_open == 1)	//Write (5)
	{
		write();
	}
	
	string arquivo;
    return 0 ;
}
