SHELL=cmd.exe
CC = gcc
CPPC = g++
INCLUDE = .
CFLAGS =
OBJ = main.o libchmm.o
LDFLAG = -lm -lgmm -lKMeans
LIB = libKMeans.a libgmm.a
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
	del /Q $(LIB)
	del /Q $(OBJ)
run:
	./$(TARGET)