CC			:= gcc
CXX			:= g++
LDLIBS = -lpng
MPICC			:= mpicc
MPICXX			:= mpicxx

CCFLAGS			:= -O3 -march=native -Wall -std=gnu11
CXXFLAGS		:= -O3 -march=native -Wall -std=c++0x

PFLAGS			:= -lpthread

all: ../bin/hw5

../bin/hw5: main.cpp
	$(CXX) $(CXXFLAGS) $< -o $@ $(LDLIBS)
clean:
	rm -f ../bin/hw5
veryclean:
	rm -f ../bin/hw5 *.result