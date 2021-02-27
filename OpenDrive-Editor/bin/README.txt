USAGE:
Device connetion:
-p <PORT>		- Serial port to use (ex: COM5)

Main arguments:
-c <OP>		- Flash chip options
			OP="reset"		Reset chip internal controller
			OP="sr"			Show Status Register (SR) contents
			OP="clear"		Clear Status Register contents	
-d <BLOCKS>	- Dump memory to file, needs -m and -f arguments
			BLOCKS=Number of 512 byte blocks to dump
-e			- Erase Flash
-v <OP>		- Verify memory, needs -m argument
			OP="compare"	Compares memory contents to file, needs -f argument
			OP="erased"		Check if memory is erased
-w			- Write file to memory, needs -m and -f argument

Acessory arguments:
-m <MEM>	- Memory to edit
			MEM="flash"		Flash memory is selected
			MEM="feram"		FeRAM (Backup Memory) is selected
-f <PATH>	- File path

Other:
-h			- View this message
