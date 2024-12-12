# ! /bin/sh

# # Set OpenSSL paths
# OPENSSL_INCLUDE=/opt/homebrew/opt/openssl@3/include
# OPENSSL_LIB=/opt/homebrew/opt/openssl@3/lib

# Compile the C and C++ source files with the OpenSSL paths
# gcc -c -I"$OPENSSL_INCLUDE" proxyDriver.c proxy.c cache.c mitm.c tunnel.c
# g++ -c -I"$OPENSSL_INCLUDE" MurmurHash3.cpp
# g++ -o proxy proxyDriver.o proxy.o cache.o MurmurHash3.o mitm.o tunnel.o \
#     -L"$OPENSSL_LIB" -lssl -lcrypto


gcc -DERROR -DDEBUG -DINFO -fsanitize=address -g -c proxyDriver.c proxy.c cache.c mitm.c tunnel.c LLM.c
g++ -DERROR -DDEBUG -DINFO -fsanitize=address -g -c MurmurHash3.cpp
g++ -DERROR -DDEBUG -DINFO -fsanitize=address -g -o proxy proxyDriver.o proxy.o cache.o MurmurHash3.o LLM.o mitm.o tunnel.o -lssl -lcrypto -lcurl

#! /bin/sh

# Set OpenSSL paths
# OPENSSL_INCLUDE=/opt/homebrew/opt/openssl@3/include
# OPENSSL_LIB=/opt/homebrew/opt/openssl@3/lib

# # Compile the C and C++ source files with the OpenSSL paths
# gcc -fsanitize=address -g -c -I"$OPENSSL_INCLUDE" proxyDriver.c proxy.c cache.c mitm.c tunnel.c LLM.c
# g++ -fsanitize=address -g -c -I"$OPENSSL_INCLUDE" MurmurHash3.cpp
# g++ -fsanitize=address -g -o proxy proxyDriver.o proxy.o cache.o MurmurHash3.o mitm.o tunnel.o LLM.o -lcurl \
#     -L"$OPENSSL_LIB" -lssl -lcrypto

