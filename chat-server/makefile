CC = g++
BUILD_DIR = build
SRC_DIR = src
INCLUDE_DIR = include
CFLAGS = -std=c++11 -Wall -I$(INCLUDE_DIR)

CHAT_SERVER_EXECUTABLE_FILE = server.out
CHAT_CLIENT_EXECUTABLE_FILE = client.out

UtilitiesSensitivityList = \
	$(SRC_DIR)/utilities.cpp \
	$(INCLUDE_DIR)/utilities.hpp \

UserSensitivityList = \
	$(SRC_DIR)/user.cpp \
	$(INCLUDE_DIR)/user.hpp \

MessageSensitivityList = \
	$(SRC_DIR)/message.cpp \
	$(INCLUDE_DIR)/message.hpp \

MessageHandlerSensitiveList = \
	$(SRC_DIR)/messagehandler.cpp \
	$(INCLUDE_DIR)/messagehandler.hpp \


IOHandlerSensitivityList = \
	$(SRC_DIR)/iohandler.cpp \
	$(INCLUDE_DIR)/iohandler.hpp \


ClientSensitivityList = \
	$(SRC_DIR)/client.cpp \
	$(INCLUDE_DIR)/client.hpp \


ServerSensitivityList = \
	$(SRC_DIR)/server.cpp \
	$(INCLUDE_DIR)/server.hpp \

CHAT_SERVER_OBJECTS = \
	$(BUILD_DIR)/server.o \
	$(BUILD_DIR)/utilities.o \
	$(BUILD_DIR)/user.o \
	$(BUILD_DIR)/message.o \
	$(BUILD_DIR)/messagehandler.o \

CHAT_CLIENT_OBJECTS = \
	$(BUILD_DIR)/client.o \
	$(BUILD_DIR)/utilities.o \
	$(BUILD_DIR)/user.o \
	$(BUILD_DIR)/message.o \
	$(BUILD_DIR)/iohandler.o \
	$(BUILD_DIR)/messagehandler.o \

all: $(BUILD_DIR) $(CHAT_SERVER_EXECUTABLE_FILE) $(CHAT_CLIENT_EXECUTABLE_FILE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(CHAT_SERVER_EXECUTABLE_FILE): $(CHAT_SERVER_OBJECTS)
	$(CC) $(CFLAGS) $(CHAT_SERVER_OBJECTS) -o $(CHAT_SERVER_EXECUTABLE_FILE)

$(CHAT_CLIENT_EXECUTABLE_FILE): $(CHAT_CLIENT_OBJECTS)
	$(CC) $(CFLAGS) $(CHAT_CLIENT_OBJECTS) -o $(CHAT_CLIENT_EXECUTABLE_FILE)

$(BUILD_DIR)/messagehandler.o: $(MessageHandlerSensitiveList)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/messagehandler.cpp -o $(BUILD_DIR)/messagehandler.o

$(BUILD_DIR)/iohandler.o: $(IOHandlerSensitivityList)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/iohandler.cpp -o $(BUILD_DIR)/iohandler.o

$(BUILD_DIR)/message.o: $(MessageSensitivityList)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/message.cpp -o $(BUILD_DIR)/message.o

$(BUILD_DIR)/user.o: $(UserSensitivityList)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/user.cpp -o $(BUILD_DIR)/user.o

$(BUILD_DIR)/utilities.o: $(UtilitiesSensitivityList)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/utilities.cpp -o $(BUILD_DIR)/utilities.o

$(BUILD_DIR)/server.o: $(ServerSensitivityList)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/server.cpp -o $(BUILD_DIR)/server.o

$(BUILD_DIR)/client.o: $(ClientSensitivityList)
	$(CC) $(CFLAGS) -c $(SRC_DIR)/client.cpp -o $(BUILD_DIR)/client.o

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) *.o *.out &> /dev/null

