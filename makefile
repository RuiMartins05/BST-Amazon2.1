# SRC_dir = source
# INC_DIR = include
# OBJ_dir = object
# BIN_dir = binary
# OBJECTS = client_stub-private.o client_stub.o data.o entry.o message.o sdmessage.pb-c.o network_client.o network_server.o tree_client.o tree_server.o tree_skel.o tree.o

# CLIENT = data.o entry.o client_stub.o client_stub-private.o network_client.o message.o sdmessage.pb-c.o tree_client.o
# SERVER = data.o entry.o  tree.o tree_skel.o network_server.o sdmessage.pb-c.o tree_server.o
# PROTOBUFF= protoc --c_out=.

# all: tree_client tree_server

# tree_server: $(SERVER)
# 	gcc -Wall -g -o $(BIN_dir)/tree_server $(addprefix $(OBJ_dir)/,$(SERVER))

# tree_client: $(CLIENT)
# 	gcc -Wall -g -o $(BIN_dir)/tree_client $(addprefix $(OBJ_dir)/,$(CLIENT))

# proto: 
# 	$(PROTOBUFF) sdmessage.proto
# 	mv sdmessage.pb-c.c $(SRC_dir)/sdmessage.pb-c.c
# 	mv sdmessage.pb-c.h $(INC_DIR)/sdmessage.pb-c.h 

# %.o: source/%.c $($@)
# 	gcc -Wall -g -I include -o $(OBJ_dir)/$@ -c $<

# clean:
# 	rm $(addprefix $(OBJ_dir)/,$(OBJECTS))
# 	rm $(BIN_dir)/tree_client
# 	rm $(BIN_dir)/tree_server




INC_DIR = include
OBJ_DIR = object
SRC_DIR = source
BIN_DIR = binary
CC = gcc
CFLAGS = -g -Wall -O2 -iquote $(INC_DIR)
OBJCLIENT = tree.o data.o entry.o client_stub.o network_client.o sdmessage.pb-c.o message.o 
OBJSERVER = tree_server.o network_server.o tree_skel.o message.o sdmessage.pb-c.o data.o entry.o tree.o
PROTOC= protoc --c_out=.

run:
	make clean
	make proto
	make tree-client
	make tree-server

%.o: $(SRC_DIR)/%.c $($@)
	$(CC) $(CFLAGS) -o $(OBJ_DIR)/$@ -c $< -I $(INC_DIR)


tree-server: $(OBJSERVER)
	$(CC) $(addprefix $(OBJ_DIR)/, $^) $(LDFLAGS) -o $(BIN_DIR)/$@    

tree-client: $(OBJCLIENT) 
	$(CC) $(OBJ_DIR)/tree_client.o -o $(BIN_DIR)/$@

proto: 
	$(PROTOC) source/sdmessage.proto
	# mv source/sdmessage.pb-c.c $(SRC_DIR)/sdmessage.pb-c.c
	# mv source/sdmessage.pb-c.h $(INC_DIR)/sdmessage.pb-c.h

clean:
	rm -f $(OBJ_DIR)/*
	rm -f $(BIN_DIR)/*