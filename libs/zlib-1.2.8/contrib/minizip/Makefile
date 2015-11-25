#
# Makefile 
#

CC=cc
CFLAGS += -O -I../.. -I../../.. ../../libz.a -fPIC

UNZ_OBJS = miniunz.o unzip.o ioapi.o	../../libz.so
ZIP_OBJS = minizip.o zip.o   ioapi.o	../../libz.so
RDR_OBJS = reader.o unzip.o ioapi.o		../../libz.so
WTR_OBJS = writer.o zip.o ioapi.o		../../libz.so
SHRD_OBJ = libjar.o zip.o unzip.o ioapi.o	../../libz.so 

.c.o:
	$(CC) -c $(CFLAGS) $*.c

all: miniunz miniz reader writer libjar.a

miniunz:  $(UNZ_OBJS)
	$(CC) $(CFLAGS) -o $@ $(UNZ_OBJS)

miniz:  $(ZIP_OBJS)
	$(CC) $(CFLAGS) -o $@ $(ZIP_OBJS)

test:	miniunz minizip
	./minizip test readme.txt
	./miniunz -l test.zip
	mv readme.txt readme.old
	./miniunz test.zip

reader: $(RDR_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

writer: $(WTR_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ 

libjar.a : $(SHRD_OBJ)
	@ar -r $@ $^

clean:
	/bin/rm -f *.o *~ miniz miniunz reader writer libjar.a
