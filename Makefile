CC=g++
SDK_PATH=../include
CFLAGS=-Wno-multichar -I $(SDK_PATH) `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv`
LDFLAGS=-lm -ldl -lpthread

DisplayVideo3D: main.cpp DisplayVideo3D.cpp $(SDK_PATH)/DeckLinkAPIDispatch.cpp
#	$(CC) -o DisplayVideo3D main.cpp DisplayVideo3D.cpp $(SDK_PATH)/DeckLinkAPIDispatch.cpp $(CFLAGS) $(LDFLAGS)
	$(CC) -o DisplayVideo3D main.cpp DisplayVideo3D.cpp $(SDK_PATH)/DeckLinkAPIDispatch.cpp $(CFLAGS) $(LIBS) $(LDFLAGS)


clean:
	rm -f DisplayVideo3D