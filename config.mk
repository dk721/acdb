# program and version
PROGRAM = sqlnet
VERSION = 0.1

# includes and libraries
INCS = -Iinclude
LIBS = -lpq -lconfig -lssl -lcrypto

# flags
CPPFLAGS = -DPROGRAM=\"${PROGRAM}\" -DVERSION=\"${VERSION}\"
CFLAGS = -Wall -Os ${INCS} ${CPPFLAGS}
LDFLAGS = ${LIBS}

# compiler
CC = gcc
