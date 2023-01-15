CC = g++
BUILD_DIR = build
SERVER_DIR = server
CLIENT_DIR = client
TOOLS_DIR = tools
INCLUDE_DIR = includes
CFLAGS = -std=c++11 -I$(INCLUDE_DIR)

OBJECTS = \
	$(BUILD_DIR)/command_handler.o \
	$(BUILD_DIR)/user.o \
	$(BUILD_DIR)/connect.o \
	$(BUILD_DIR)/config.o \
	$(BUILD_DIR)/logger.o \
	$(BUILD_DIR)/user_handler.o \
	$(BUILD_DIR)/utilities.o\


all: $(BUILD_DIR) server.out client.out

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

server.out: $(BUILD_DIR)/server.o $(OBJECTS)
	$(CC) $(CFLAGS) -o server.out $(BUILD_DIR)/server.o $(OBJECTS)

client.out: $(BUILD_DIR)/client.o $(OBJECTS)
	$(CC) $(CFLAGS) -o client.out $(BUILD_DIR)/client.o $(OBJECTS)

$(BUILD_DIR)/command_handler.o: $(TOOLS_DIR)/command_handler.hpp $(TOOLS_DIR)/command_handler.cpp
	$(CC) $(CFLAGS) -c $(TOOLS_DIR)/command_handler.cpp -o $(BUILD_DIR)/command_handler.o

$(BUILD_DIR)/config.o: $(TOOLS_DIR)/config.hpp $(TOOLS_DIR)/config.cpp
	$(CC) $(CFLAGS) -c $(TOOLS_DIR)/config.cpp -o $(BUILD_DIR)/config.o

$(BUILD_DIR)/connect.o: $(TOOLS_DIR)/connect.hpp $(TOOLS_DIR)/connect.cpp
	$(CC) $(CFLAGS) -c $(TOOLS_DIR)/connect.cpp -o $(BUILD_DIR)/connect.o

$(BUILD_DIR)/user.o: $(TOOLS_DIR)/user.hpp $(TOOLS_DIR)/user.cpp
	$(CC) $(CFLAGS) -c $(TOOLS_DIR)/user.cpp -o $(BUILD_DIR)/user.o

$(BUILD_DIR)/logger.o: $(TOOLS_DIR)/logger.hpp $(TOOLS_DIR)/logger.cpp
	$(CC) $(CFLAGS) -c $(TOOLS_DIR)/logger.cpp -o $(BUILD_DIR)/logger.o

$(BUILD_DIR)/user_handler.o: $(TOOLS_DIR)/user_handler.hpp $(TOOLS_DIR)/user_handler.cpp
	$(CC) $(CFLAGS) -c $(TOOLS_DIR)/user_handler.cpp -o $(BUILD_DIR)/user_handler.o

$(BUILD_DIR)/utilities.o: $(TOOLS_DIR)/utilities.hpp $(TOOLS_DIR)/utilities.cpp
	$(CC) $(CFLAGS) -c $(TOOLS_DIR)/utilities.cpp -o $(BUILD_DIR)/utilities.o

$(BUILD_DIR)/server.o: $(SERVER_DIR)/server.hpp $(SERVER_DIR)/server.cpp
	$(CC) $(CFLAGS) -c $(SERVER_DIR)/server.cpp -o $(BUILD_DIR)/server.o

$(BUILD_DIR)/client.o: $(CLIENT_DIR)/client.hpp $(CLIENT_DIR)/client.cpp
	$(CC) $(CFLAGS) -c $(CLIENT_DIR)/client.cpp -o $(BUILD_DIR)/client.o

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) *.o *.out log.txt