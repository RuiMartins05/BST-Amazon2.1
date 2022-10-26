OBJ_dir = object
BIN_dir = binary
OBJECTS = client_stub-private.o client_stub.o data.o entry.o message.o network_client.o network_server.o tree_client.o tree_server.o tree_skel.o tree.o

CLIENT = data.o entry.o client_stub.o client_stub-private.o network_client.o sdmessage.pb-c.o tree_client.o
SERVER = data.o entry.o  tree.o tree_skel.o network_server.o sdmessage.pb-c.o tree_server.o

all: tree_client tree_server

tree_client: $(CLIENT)
	gcc -Wall -g -o $(BIN_dir)/tree_client $(addprefix $(OBJ_dir)/,$(CLIENT))

tree_server: $(SERVER)
	gcc -Wall -g -o $(BIN_dir)/tree_server $(addprefix $(OBJ_dir)/,$(SERVER))

%.o: source/%.c $($@)
	gcc -Wall -g -I include -o $(OBJ_dir)/$@ -c $<

clean:
	rm $(addprefix $(OBJ_dir)/,$(OBJECTS))
	rm $(BIN_dir)/tree_client
	rm $(BIN_dir)/tree_server