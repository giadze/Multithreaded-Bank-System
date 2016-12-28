COMPILER=gcc
COMPILER_FLAGS= -g -D_GNU_SOURCE -D_POSIX_C_SOURCE -Wall -Werror -std=c99 -pedantic -pthread -Qunused-arguments 
PROGRAM=server
INCLUDES_FOLDER=include
LIBRARY_FOLDER=lib
LIBRARY_SOURCES=
SOURCE_FOLDER=
SOURCES=var.c linkedlist.c hashmap.c tokenizer.c server.c
CLEANUP=*.o server client
EXEC=FALSE
EXEC_ARGS="Color color = new Color(0xFFEEDD, 0xCCBBAA, 0x99);"
VALGRIND=FALSE
VALGRIND_FLAGS=--leak-check=full --show-reachable=yes --read-var-info=yes --track-origins=yes
DEBUG=FALSE

ALL_SOURCES=$(LIBRARY_SOURCES) $(SOURCES)
OBJECTS=$(ALL_SOURCES:.c=.o)

SOURCES_LINKED=$(SOURCES:%.c=%.c)
LIBRARY_SOURCES_LINKED=$(LIBRARY_SOURCES:%.c=$(LIBRARY_FOLDER)/%.c)
ALL_SOURCES_LINKED=$(LIBRARY_SOURCES_LINKED) $(SOURCES_LINKED)

ifeq ($(DEBUG), FALSE)
	SUPPRESS=@
else
	SUPPRESS=
endif

MAKE: SETUP COMPILE $(PROGRAM) EXECUTE


EXECUTE:
ifeq ($(EXEC), TRUE)
	@echo 'Executing <$(PROGRAM) $(EXEC_ARGS)>:'
ifeq ($(VALGRIND), TRUE)
	$(SUPPRESS)valgrind $(VALGRIND_FLAGS) $(PROGRAM) $(EXEC_ARGS)
else
	$(SUPPRESS)./$(PROGRAM) $(EXEC_ARGS)
endif
endif

clean:
	$(SUPPRESS)rm -rf ./$(CLEANUP)


$(PROGRAM): $(OBJECTS)
	$(SUPPRESS)$(COMPILER) $(COMPILER_FLAGS) -I $(INCLUDES_FOLDER) $(OBJECTS) -o $(PROGRAM)
	$(SUPPRESS)gcc -o client -std=gnu99 -pthread client.c

COMPILE: $(SOURCES_LINKED)
	$(SUPPRESS)$(COMPILER) -c $(COMPILER_FLAGS) -I $(INCLUDES_FOLDER) $(ALL_SOURCES_LINKED)

SETUP:
	@
