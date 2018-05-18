CC = gcc
CFLAGS = -Wall 
LFLAGS =
OBJECTS = $(patsubst %.c,%.o,$(wildcard *.c))
FICHIERS = disque.dat inodes.dat blocs_libres.dat repertoires.dat
EXEC = fs

$(EXEC): $(OBJECTS)
	$(CC) $(LFLAGS) -o $(EXEC) $(OBJECTS)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean test

clean:
	rm -f $(OBJECTS) $(EXEC) $(FICHIERS)

test:
	make;
	rm -f $(FICHIERS);
	bats test/testArguments.bats;
	rm -f $(FICHIERS);
	bats test/testCommandesSimples.bats;
	rm -f $(FICHIERS);
	bats test/testCommandesIndirection.bats;
	rm -f $(FICHIERS);
	bats test/testCommandesComplexes.bats;
	rm -f $(FICHIERS);
	bats test/testCommandesErreurs.bats;


