CC = g++
LD = ld
TARGET = regex_pt
LDFLAGS = -W -Xlinker -Map=$(TARGET).map
CXXFLAGS = -g -Wall

SDIR = src

SRC = $(wildcard $(SDIR)/*.cpp)
OBJ = $(SRC:.cpp=.o)
INCLUDE = -I./include

$(TARGET) : $(OBJ)
	$(CC) $(CXXFLAGS) $(INCLUDE) -o $@ $(OBJ) $(LDFLAGS)

.PHONY: clean

clean:
	rm -f $(OBJ) $(TARGET) $(TARGET).map
