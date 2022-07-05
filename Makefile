CC = g++ -std=c++11

CFLAGS = -g -Wall

all: client monitor serverM serverA serverB serverC

client: client.cpp
	$(CC) $(CFLAGS) -o client client.cpp

monitor: monitor.cpp
	$(CC) $(CFLAGS) -o monitor monitor.cpp

serverM: serverM.cpp
	$(CC) $(CFLAGS) -o serverM serverM.cpp

serverA: serverA.cpp
	$(CC) $(CFLAGS) -o serverA serverA.cpp

serverB: serverB.cpp
	$(CC) $(CFLAGS) -o serverB serverB.cpp

serverC: serverC.cpp
	$(CC) $(CFLAGS) -o serverC serverC.cpp

clean:
	$(RM) client monitor serverM serverA serverB serverC