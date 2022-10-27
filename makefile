OBJ_DIR = object
SRC_DIR = source
BIN_DIR = binary
INC_DIR = include
LIB_DIR = lib
CC = gcc
CFLAGS = -Wall -g -o
CLIENT_LIB = data.o entry.o client_stub.o network_client.o message.o
TREE_CLIENT = tree_client.o sdmessage.pb-c.o
TREE_SERVER = tree_server.o network_server.o tree_skel.o message.o sdmessage.pb-c.o data.o entry.o tree.o
LDFLAGS = /usr/lib/x86_64-linux-gnu/libprotobuf-c.a
PROTOC= protoc --c_out=.

all: proto client_lib tree_client tree_server protoclean

%.o: $(SRC_DIR)/%.c $($@)
	$(CC) -Wall -g -I include -o $(OBJ_DIR)/$@ -c $<

proto: 
	$(PROTOC) sdmessage.proto
	$(CC) -c sdmessage.pb-c.c
	cp sdmessage.pb-c.o $(OBJ_DIR)/sdmessage.pb-c.o
	cp sdmessage.pb-c.c $(SRC_DIR)/sdmessage.pb-c.c
	cp sdmessage.pb-c.h $(INC_DIR)/sdmessage.pb-c.h

client_lib: $(CLIENT_LIB)
	ld -r $(addprefix $(OBJ_DIR)/,$(CLIENT_LIB)) -o $(LIB_DIR)/client_lib.o


tree_client: $(TREE_CLIENT)
	$(CC) $(CFLAGS) $(BIN_DIR)/tree_client $(addprefix $(OBJ_DIR)/,$(TREE_CLIENT)) $(LIB_DIR)/client_lib.o $(LDFLAGS)


tree_server: $(TREE_SERVER)
	$(CC) $(CFLAGS) $(BIN_DIR)/tree_server $(addprefix $(OBJ_DIR)/,$(TREE_SERVER)) $(LDFLAGS)

protoclean:
	rm sdmessage.pb-c.o 
	rm sdmessage.pb-c.c 
	rm sdmessage.pb-c.h 


clean:
	rm -f $(OBJ_DIR)/*
	rm -f $(BIN_DIR)/*
	rm -f $(LIB_DIR)/* 
