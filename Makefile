CC = gcc
CFLAGS = -Wall -Wextra -lm -lraylib
OUTPUT = plotter
DEVICE = /dev/ttyACM0
EXEC_FLAGS = -D $(DEVICE)

build:
	${CC} plotter.c -o ${OUTPUT} ${CFLAGS}

# Hyperland doesn't allow gui applications root permissions.
# https://stackoverflow.com/questions/48833451/no-protocol-specified-when-running-a-sudo-su-app-on-ubuntu-linux
xhost:
	xhost si:localuser:root


run: build 
	./${OUTPUT} ${EXEC_FLAGS}

clean:
	rm -f ${OUTPUT}
