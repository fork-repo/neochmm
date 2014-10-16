ifeq ($(OS),Windows_NT)
   RM = del /Q
   FixPath = $(subst /,\,$1)
   SHELL=cmd.exe
else
   ifeq ($(shell uname), Linux)
      RM = rm -f
      FixPath = $1
   endif
endif

CC = gcc
CPPC = g++
INCLUDE = .
#CFLAGS = -mwindows
OBJ = main.o
LDFLAG = -lm -lgmm -lKMeans -lchmm -lwin -lgdi32 -lwinmm -laudiotool
#LDFLAG = -lm -lgmm -lKMeans -lchmm -lwin
LIB = libKMeans.a libgmm.a libchmm.a libwin.a libaudiotool.a
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

libchmm.a: libchmm.o libchmm.o libgmm.o libKMeans.o
	ar rcs $@ $?
	$(RM) $?

.PHONY: clean
clean:
	$(RM) $(TARGET)
	$(RM) $(LIB)
	$(RM) $(OBJ)
run:
	./$(TARGET)