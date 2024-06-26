# Compiler for C++
CXX = g++
CXXFLAGS = -Wall -pthread -std=c++11  # Add -std=c++11 to enable C++11 features
EXEC = simulator

# Object files
OBJS = simulator.o WriteOutput.o

# Default target
all: $(EXEC)

# Build the simulator executable
$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(OBJS)

# Compile source files
simulator.o: simulator.cpp
	$(CXX) $(CXXFLAGS) -c simulator.cpp

WriteOutput.o: WriteOutput.c WriteOutput.h
	$(CXX) $(CXXFLAGS) -c WriteOutput.c

# Clean up
clean:
	rm -f $(EXEC) $(OBJS)
