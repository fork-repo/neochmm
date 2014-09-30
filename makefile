CC = gcc
CPPC = g++
INCLUDE = .
CFLAGS =
LDFLAG = -lm
OBJ = main.o gmm.o win.o kmeans.o
TARGET = main

all: $(OBJ)
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
run:
	./$(TARGET)
