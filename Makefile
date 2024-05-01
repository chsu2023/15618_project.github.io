# Compiler
CC = g++
# Compiler flags
CFLAGS = -std=c++17 -Wall -Iinclude/

# Source files directory
SRCDIR = src/
# Object files directory
OBJDIR = obj
# Include files directory
INCDIR = include/

# List all source files
SRCS = $(wildcard $(SRCDIR)/*.cpp)
# Generate corresponding object file names
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))

# Target executable name
TARGET = cachesim

# Default rule to build the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# Rule to compile each source file into object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule to remove object files and the executable
clean:
	rm -f $(OBJDIR)/*.o $(TARGET)

# Phony target to prevent conflicts with file names
.PHONY: clean
