SHELL=cmd.exe
CC = gcc
CPPC = g++
INCLUDE = .
CFLAGS =
LDFLAG = -lm -lKMeans
OBJ = main.o gmm.o win.o 
LIB = libKMeans.a
TARGET = main.exe

all: $(OBJ) $(LIB)
	$(CPPC) -o $(TARGET) $(OBJ) -L. $(LDFLAG) 

%.o: %.c
	echo *** [GCC] $@ : $<
	$(CC) -c -o $@ $< $(CFLAGS) -I $(INCLUDE) 

%.o: %.cpp
	echo *** [G++] $@ : $<
	$(CPPC) -c -o $@ $< $(CFLAGS) -I $(INCLUDE) 

%.a: %.o
	echo *** [AR] $@ : $<
	ar rcs $@ $<

.PHONY: clean
clean:
	del /Q $(TARGET)
	del /Q $(OBJ)
	del /Q $(LIB)
run:
	./$(TARGET)
