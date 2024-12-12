# ! /bin/sh

gcc -c proxyDriver.c proxy.c cache.c mitm.c tunnel.c LLM.c
g++ -c MurmurHash3.cpp
g++ -o proxy proxyDriver.o proxy.o cache.o MurmurHash3.o LLM.o mitm.o tunnel.o -lssl -lcrypto -lcurl