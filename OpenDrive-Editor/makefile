OBJS	= main.o serialib.o func.o
SOURCE	= main.cpp include/serialib.cpp include/func.cpp
HEADER	= include/serialib.h include/func.h
OUT		= od_editor.exe
CC		= g++
FLAGS	= -g -c
LFLAGS	=

all: $(OBJS)
	$(CC) -g $(OBJS) -o $(OUT) $(LFLAGS)

main.o: main.cpp
	$(CC) $(FLAGS) main.cpp

serialib.o: include/serialib.cpp
	$(CC) $(FLAGS) include/serialib.cpp 
	
func.o: include/func.cpp
	$(CC) $(FLAGS) include/func.cpp 

clean:
	rm -f $(OBJS) $(OUT)