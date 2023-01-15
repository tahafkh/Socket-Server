CC = g++
BUILD_DIR = build
SRC_DIR = src
INCLUDE_DIR = include
CFLAGS = -std=c++11 -Wall -I$(INCLUDE_DIR)

ifeq ($(OS),Windows_NT)
	LDLIBS += -l Ws2_32
endif

WEB_SERVER_EXECUTABLE_FILE = server.out

UtilitiesSensitivityList = \
	$(SRC_DIR)/utilities.cpp \
	$(INCLUDE_DIR)/utilities.hpp \
	$(INCLUDE_DIR)/defs.hpp \

ResponseSensitivityList = \
	$(SRC_DIR)/response.cpp \
	$(INCLUDE_DIR)/response.hpp \
	$(INCLUDE_DIR)/defs.hpp \

RequestSensitivityList = \
	$(SRC_DIR)/request.cpp \
	$(INCLUDE_DIR)/request.hpp \

RouteSensitiveList = \
	$(SRC_DIR)/route.cpp \
	$(INCLUDE_DIR)/route.hpp \
	$(INCLUDE_DIR)/server.hpp \

HandlersSensitivityList = \
	$(SRC_DIR)/handlers.cpp \
	$(INCLUDE_DIR)/handlers.hpp \

ServerSensitivityList = \
	$(SRC_DIR)/server.cpp \
	$(INCLUDE_DIR)/server.hpp \
	$(INCLUDE_DIR)/utilities.hpp \

MainSensitivityList = \
	$(SRC_DIR)/main.cpp \
	$(INCLUDE_DIR)/handlers.hpp \

WEB_SERVER_OBJECTS = \
	$(BUILD_DIR)/main.o \
	$(BUILD_DIR)/request.o \
	$(BUILD_DIR)/response.o \
	$(BUILD_DIR)/handlers.o \
	$(BUILD_DIR)/route.o \
	$(BUILD_DIR)/server.o \
	$(BUILD_DIR)/utilities.o 

all: $(BUILD_DIR) $(WEB_SERVER_EXECUTABLE_FILE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/main.o: $(MainSensitivityList)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/main.cpp -o $(BUILD_DIR)/main.o

$(BUILD_DIR)/request.o: $(RequestSensitivityList)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/request.cpp -o $(BUILD_DIR)/request.o

$(BUILD_DIR)/response.o: $(ResponseSensitivityList)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/response.cpp -o $(BUILD_DIR)/response.o

$(BUILD_DIR)/handlers.o: $(HandlersSensitivityList)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/handlers.cpp -o $(BUILD_DIR)/handlers.o

$(BUILD_DIR)/route.o: $(RouteSensitiveList)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/route.cpp -o $(BUILD_DIR)/route.o

$(BUILD_DIR)/server.o: $(ServerSensitivityList)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/server.cpp -o $(BUILD_DIR)/server.o

$(BUILD_DIR)/utilities.o: $(UtilitiesSensitivityList)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/utilities.cpp -o $(BUILD_DIR)/utilities.o

$(WEB_SERVER_EXECUTABLE_FILE): $(WEB_SERVER_OBJECTS)
	$(CC) $(CFLAGS) $(WEB_SERVER_OBJECTS) -o $(WEB_SERVER_EXECUTABLE_FILE) $(LDLIBS)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) *.o *.out &> /dev/null
