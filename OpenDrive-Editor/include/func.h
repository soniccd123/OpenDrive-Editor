#include "serialib.h"
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>

//Serial operations
void serial_init();	//Serial initialization

//Read operations
void is_erased();	//Verify Flash erasure
void read_addr();	//Read arbitrary address
void dump();
void verify();

//Write operations
void write();


//Erase operations
void erase_flash();

//Debug operations
void reset_flash();		//Return Flash to reset state
void read_sr();		//Read Status Register
void clear_sr();		//Clear Status Register
