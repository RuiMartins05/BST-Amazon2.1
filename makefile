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

# INC_DIR = include
# OBJ_DIR = object
# SRC_DIR = source
# BIN_DIR = binary
# CC = gcc
# CFLAGS = -Wall -g -w -I
# OBJCLIENT = tree.o data.o entry.o client_stub.o network_client.o sdmessage.pb-c.o message.o 
# OBJSERVER = tree_server.o network_server.o tree_skel.o message.o sdmessage.pb-c.o data.o entry.o tree.o
# PROTOC= protoc --c_out=.

# run:
# 	make clean
# 	make proto
# 	make tree_client
# 	make tree_server

# %.o: $(SRC_DIR)/%.c $($@)
# 	$(CC) $(CFLAGS) -o $(OBJ_DIR)/$@ -c $< -I $(INC_DIR)


# tree_server: $(OBJSERVER)
# 	$(CC) $(addprefix $(OBJ_DIR)/, $^) $(LDFLAGS) -o $(BIN_DIR)/$@    

# tree_client: $(OBJCLIENT) 
# 	$(CC) $(OBJ_DIR)/tree_client.o -o $(BIN_DIR)/$@

# proto: 
# 	$(PROTOC) sdmessage.proto
# 	# mv source/sdmessage.pb-c.c $(SRC_DIR)/sdmessage.pb-c.c
# 	# mv source/sdmessage.pb-c.h $(INC_DIR)/sdmessage.pb-c.h

# clean:
# 	rm -f $(OBJ_DIR)/*
# 	rm -f $(BIN_DIR)/*
CC = gcc
INC_DIR = include
OBJ_DIR = object
BIN_DIR = binary
SRC_DIR = source
LIB_DIR = lib

OBJETOS = data.o entry.o tree.o sdmessage.pb-c.o message.o network_client.o network_server.o tree_skel.o client_stub.o client-lib.o tree_client.o tree_server.o

data.o = data.h
entry.o = data.h entry.h
tree.o = entry.h data.h tree.h tree-private.h
sdmessage.pb-c.o = sdmessage.pb-c.h
tree_skel.o = tree.h sdmessage.pb-c.h
message.o = message-private.h
network_server.o = tree_skel.h network_server.h sdmessage.pb-c.h
client_stub.o = tree.h data.h entry.h client_stub.h client_stub-private.h network_client.h sdmessage.pb-c.h message.h
network_client.o = sdmessage.pb-c.h client_stub.h
client-lib.o = client_stub.h network-client.h data.h entry.h sdmessage.pb-c.h
tree_client.o = tree_client.h data.h entry.h client_stub.h client_stub-private.h
tree_server.o = tree_server.h data.h entry.h tree.h message.h tree_skel.h network_server.h sdmessage.pb-c.h

CFLAGS = -Wall -g -w -I

PROTOC=protoc --c_out=.

LIBS=-lrt -lpthread
all: $(OBJETOS) tree_server tree_client

proto: 
	$(PROTOC) sdmessage.proto
	mv source/sdmessage.pb-c.h $(INC_DIR)

%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INC_DIR) -o $(OBJ_DIR)/$@ -c $<

client-lib.o: tree.o data.o entry.o client_stub.o client_stub-private.o network_client.o sdmessage.pb-c.o message.o
	ld -r $(addprefix $(OBJ_DIR)/,$^) -o $(LIB_DIR)/$@

tree_client: tree_client.o data.o entry.o message.o client_stub.o network_client.o
	$(CC)  $(addprefix $(OBJ_DIR)/,$^) -I/usr/local/include -L/usr/local/lib -lprotobuf-c -o $(BIN_DIR)/$@

tree_server: tree_server.o data.o entry.o tree.o message.o tree_skel.o network_server.o
	$(CC) $(addprefix $(OBJ_DIR)/,$^) -I/usr/local/include -L/usr/local/lib -lprotobuf-c -o $(BIN_DIR)/$@

clean:
	rm -f $(OBJ_DIR)/*.o
	rm -f $(BIN_DIR)/*
	rm -f $(LIB_DIR)/*
