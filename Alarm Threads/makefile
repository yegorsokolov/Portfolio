#First use "make" command to get the executable file of code
#Then use "./New_alarm_mutex" to execute the code
# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -pthread

# Target executable name
TARGET = new_alarm_mutex

# Source files
SRCS = new_alarm_mutex.c

# Build target
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

# Clean target to remove the executable
clean:
	rm -f $(TARGET)
	
