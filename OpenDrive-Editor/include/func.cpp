#include "serialib.h"
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

using namespace std;
extern serialib serial;
extern unsigned char buffer[512];
extern string port;
extern const char* filePath;
extern int size;
extern int memory_mode;

//---------------------------------------------------------
//SERIAL OPERATIONS
//---------------------------------------------------------
/* void serial_init()	//Serial initialization
{
    // Connection to serial port
    char errorOpening = serial.openDevice(port, 115200);

    // If connection fails, return the error code otherwise, display a success message
    //if (errorOpening!=1) return errorOpening;
    printf ("Conectado \n\n");
}  */

//---------------------------------------------------------
//FILE OPERATIONS
//---------------------------------------------------------
long getFileSize(FILE *file)	// Get the size of a file
{
	long lCurPos, lEndPos;
	lCurPos = ftell(file);
	fseek(file, 0, 2);
	lEndPos = ftell(file);
	fseek(file, lCurPos, 0);
	return lEndPos;
}

//---------------------------------------------------------
//READ OPERATIONS
//---------------------------------------------------------
void is_erased()				//Verify Erasure
{
	//Function local variables
	int block_n = 0;	//Block counter
	int addr = 0; 		//Memory address counter
	int autom;			//Automatic/Manual verification switch
	long int blocos = 0x2000;	//Number of blocks to be verified
		
	//Request MCU Erasure verification mode
	unsigned char send_addr[3] = {'r', 's', 'f'};
	serial.writeBytes(send_addr, 3);
	
	char ready_wait[1];
	while (ready_wait[0] != 'r')
	{
		char test = serial.readBytes(ready_wait,1,10000);
	}
	
	//Start clock for transfer rate determination
	auto start = chrono::steady_clock::now();
	
	//Read loop
	while (block_n < blocos)
	{
		//Wait for MCU next block request
		serial.writeChar('n');
		
		//Receive MCU block
		char test = serial.readBytes(buffer,512,5000);
		
		//Block verification loop
		int loop = 0;
		while (loop < 512)
		{
			//Check word for 0xFF (Erased state), branch and display address+data if not erased
			if (buffer[loop] != 0xFF || buffer[loop+1] != 0xFF)
			{
				cout << "\nByte nao apagado em: " << addr;
				cout << "\nConteudo: ";
				cout << "0x";
				printf("%.2X", (int)buffer[loop]);
				cout << " 0x";
				printf("%.2X", (int)buffer[loop+1]);
				cout << "\n";
				
				//Request MCU to end verification routine, return to Flash menu
				serial.writeChar('e');
				return;
			}
			loop=loop+2;
			addr=addr+2;
		}
		block_n=block_n+1;
	}
	
	//Request MCU to end verification routine
	serial.writeChar('e');
	
	//Stop clock and calculate transfer rate
	auto end = chrono::steady_clock::now();
	cout << "\nApagado!";
	float tempo = chrono::duration_cast<chrono::microseconds>(end-start).count();
	cout << "\nTempo(s): " << chrono::duration_cast<chrono::milliseconds>(end-start).count() << "ms";
	cout << "\nTaxa de transferencia(kB/s): " << (blocos*512)/(tempo/1000) << "kB/s";
}

void read_addr()	//Read arbitrary address
{
	// Input Address to be read
	uint32_t addr;
	cout << "Qual endereco ler?: ";
	cin >> addr;
						
	// Send Address to be read
	unsigned char send_addr[5] = {'r', 'a', ((addr & 0xFF)), ((addr & 0xFF00)>>8), ((addr & 0xFF0000)>>16)};
	serial.writeBytes(send_addr, 5);
	
	// Wait for answer
	char test = serial.readBytes(send_addr,2,10000);

	// Display address and data
	cout << "\nConteudo: ";
	cout << "0x";
	printf("%.2X", (int)send_addr[0]);
	cout << " 0x";
	printf("%.2X", (int)send_addr[1]);
}

void dump()
{
	int block_n = 0;
	ofstream dump_file;
	dump_file.open (filePath, ios::out | ios::binary);
	
	unsigned char send_addr[3] = {'r', 's', 'f'};
	if (memory_mode == 1)
	{
		send_addr[2] = 'f';
	}
	else if (memory_mode == 2)
	{
		send_addr[2] = 's';
	}
	else
	{
		cout << "Memory not selected\n";
		return;
	}
	
	serial.writeBytes(send_addr, 3);
	
	char ready_wait[1];
	while (ready_wait[0] != 'r')
	{
		char test = serial.readBytes(ready_wait,1,10000);
	}
	
	auto start = chrono::steady_clock::now();
	char cart_buffer[512];
	char corrected_end[512];
	while (block_n < size)
	{
		serial.writeChar('n');
		char test = serial.readBytes(cart_buffer,512,10000);
/* 		int loop=0;
		while (loop < 512)
		{
			corrected_end[loop] = cart_buffer[loop+1];
			corrected_end[loop+1] = cart_buffer[loop];
			loop++;
		} */
		dump_file.write (cart_buffer, 512);
		block_n++;
	}
	serial.writeChar('e');
	dump_file.close();
	return;
}

void verify()
{
	int block_n = 0;
	int addr = 0;
	BYTE *fileBuf;			// Pointer to our buffered data
	FILE *file = NULL;		// File pointer

	if ((file = fopen(filePath, "rb")) == NULL)
	{
		cout << "File not found" << endl;
		return;
	}
	else
	{
		cout << "File opened" << endl;
	}
	
	long fileSize = getFileSize(file);
	fileBuf = new BYTE[fileSize];
	fread(fileBuf, fileSize, 1, file);
	
				
	unsigned char send_addr[3] = {'r', 's', 'f'};
	if (memory_mode == 1)
	{
		send_addr[2] = 'f';
	}
	else if (memory_mode == 2)
	{
		send_addr[2] = 's';
	}
	else
	{
		cout << "Memory not selected\n";
		return;
	}
	
	serial.writeBytes(send_addr, 3);
	
	char ready_wait[1];
	while (ready_wait[0] != 'r')
	{
		char test = serial.readBytes(ready_wait,1,10000);
	}
		
	auto start = chrono::steady_clock::now();
	
	while (block_n < fileSize/512)
	{
		serial.writeChar('n');
		char test = serial.readBytes(buffer,512,50);
		
		int loop = 0;
		while (loop < 512)
		{
			if (buffer[loop] != fileBuf[addr] || buffer[loop+1] != fileBuf[addr+1])
			{
				cout << "\nDados divergentes em: " << addr;
				cout << "\nChip:    ";
				cout << "0x";
				printf("%.2X", (int)buffer[loop]);
				cout << " 0x";
				printf("%.2X", (int)buffer[loop+1]);
				cout << "\nArquivo: ";
				cout << "0x";
				printf("%.2X", (int)fileBuf[addr+1]);
				cout << " 0x";
				printf("%.2X", (int)fileBuf[addr]);
				serial.writeChar('e');
				return;
			}			
			
			addr=addr+2;
			loop=loop+2;
		}
		block_n=block_n+1;
	}
	serial.writeChar('e');
	
	auto end = chrono::steady_clock::now();
	cout << "\nDone!";
	float tempo = chrono::duration_cast<chrono::microseconds>(end-start).count();
	cout << "\nTempo(s): " << chrono::duration_cast<chrono::milliseconds>(end-start).count() << "ms";
	cout << "\nTaxa de transferencia(kB/s): " << (fileSize)/(tempo/1000) << "kB/s";
	return;
}

//---------------------------------------------------------
//WRITE OPERATIONS
//---------------------------------------------------------

void write()
{
	//Function variables
	string arquivo;			//File name string
	uint32_t blocos;		//Number of 256byte blocks to write
	int running = 1;		//Write loop active
	int address = 0;		//File address to be written
	int block_count = 0;	//Block counter for debug purpose
	
	BYTE *fileBuf;			// Pointer to our buffered data
	FILE *file = NULL;		// File pointer

	// Open the file in binary mode using the "rb" format string and checks if file is readable
	if ((file = fopen(filePath, "rb")) == NULL)
	{
		cout << "File not found" << endl;
		return;
	}
	else
		cout << "File opened" << endl;
	// Get the size of the file in bytes
	long fileSize = getFileSize(file);
	// Allocate space in the buffer for the whole file
	fileBuf = new BYTE[fileSize];
	// Read the file in to the buffer
	fread(fileBuf, fileSize, 1, file);
	//Calculate number of blocks
	blocos = fileSize/256;
	
	
	unsigned char send_addr[5] = {'w', 'n', ((blocos & 0xFF)), ((blocos & 0xFF00)>>8), ((blocos & 0xFF0000)>>16)};
	if (memory_mode == 1)
	{
		send_addr[1] = 'n';
	}
	else if (memory_mode == 2)
	{
		send_addr[1] = 'f';
	}
	else
	{
		cout << "Memory not selected\n";
		return;
	}
	serial.writeBytes(send_addr, 5);
	
	//Start transfer rate clock
	auto start = chrono::steady_clock::now();
	while (running == 1)
	{
		char test = serial.readBytes(send_addr,1,10000);
		if(send_addr[0] == 'n')
		{
			cout << "\n";
			cout << block_count;
			int loop = 0;
			while (loop < 64)
			{
				buffer[loop] = fileBuf[address];
				address=address+1;
				loop=loop+1;
			}
			serial.writeBytes(buffer, 64);
			send_addr[0] = 0;
			block_count=block_count+1;
		}
		else if(send_addr[0] == 'e')
		{
			running = 0;
			auto end = chrono::steady_clock::now();
			cout << "\nDone!";
			float tempo = chrono::duration_cast<chrono::microseconds>(end-start).count();
			cout << "\nTempo(s): " << chrono::duration_cast<chrono::milliseconds>(end-start).count() << "ms";
			cout << "\nTaxa de transferencia(kB/s): " << (blocos*256)/(tempo/1000) << "kB/s";
			return;
		}
		else
		{
			cout << "\nFalha";
			return;
		}
	}
}

//---------------------------------------------------------
//ERASE OPERATIONS
//---------------------------------------------------------
void erase_flash()
{
	// Request MCU Erase mode
	unsigned char send_addr[2] = {'b', 0};
	serial.writeBytes(send_addr, 2);
	
	// Wait for MCU SR read to
	char test = serial.readBytes(send_addr,2,10000);
	
	// Check for erase success (SR = 0x80)
	if (send_addr[0] == 0x80)
	{
		cout << "\nErase successful";
		return;
	}
	
	//Display error if SR != 0x80
	else
	{
		cout << "\nError";
		cout << "\nSR: ";
		cout << "0x";
		printf("%.2X", (int)send_addr[0]);
		cout << " 0x";
		printf("%.2X", (int)send_addr[1]);
		cout << " (Expected 0x80 0x00)";
		return;
	}
}

//---------------------------------------------------------
//DEBUG OPERATIONS
//---------------------------------------------------------
void reset_flash()	//Return Flash to reset state
{
	// Request MCU Flash Reset State
	unsigned char send_addr[2] = {'d', 'i'};
	serial.writeBytes(send_addr, 2);

	// Wait for answer
	char test = serial.readBytes(send_addr,2,10000);
	
	// Display SR (Should not appear)
	cout << "\nSR: ";
	cout << "0x";
	printf("%.2X", (int)send_addr[0]);
	cout << " 0x";
	printf("%.2X", (int)send_addr[1]);
}

void read_sr()	//Read Status Register
{
	// Request SR Read
	unsigned char send_addr[2] = {'d', 'r'};
	serial.writeBytes(send_addr, 2);

	// Wait for answer
	char test = serial.readBytes(send_addr,2,10000);
	
	// Display SR
	cout << "\nSR: ";
	cout << "0x";
	printf("%.2X", (int)send_addr[0]);
	cout << " 0x";
	printf("%.2X", (int)send_addr[1]);
}

void clear_sr()	//Debug operations
{
	// Request SR Clear
	unsigned char send_addr[7] = {'w', 'a', 0x00, 0x00, 0x00, 0x00, 0x00};
	serial.writeBytes(send_addr, 7);
	
	// Wait for answer
	char test = serial.readBytes(send_addr,2,10000);
	
	// Display SR
	cout << "\nSR: ";
	cout << "0x";
	printf("%.2X", (int)send_addr[0]);
	cout << " 0x";
	printf("%.2X", (int)send_addr[1]);
}
