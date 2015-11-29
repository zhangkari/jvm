############################
# file name:	Makefile
# author:		kari.zhang
#
############################

JVM = jvm
JVMP = jvmp
JAR = jar
BOOTSTRAP = rt.jar
PROJ = $(JVM) $(JVMP) $(JAR) # $(BOOTSTRAP)
CC = gcc
CFLAGS +=  -Iinclude \
          -Ilibs \
		  -L./libs \
		  -g \
	      -DDEBUG # -m32

LDFLAGS += -pthread
                     
SRC = $(wildcard src/vm/*.c) $(wildcard libs/*.c)
OBJ = $(patsubst %.c, %.o, $(SRC))
DEP = $(patsubst %.c, %.d, $(SRC))

BIN_SRC = $(wildcard src/*.c)
BIN_OBJ = $(patsubst %.c, %.o, $(BIN_SRC))

JAVA_SRC = $(wildcard java/lang/*.java)
CLASSES = $(patsubst %.java, %.class, $(JAVA_SRC))

VPATH = src:include

all : ziplib $(DEP) $(OBJ) $(BIN_OBJ) $(PROJ) $(CLASSES)
	@echo "Build OK"

# Build libz for jar
ziplib:
	@make -s -C libs

# Build jvm
$(JVM) : $(OBJ) src/jvm.o libs/libjar.a
	@$(CC) $(CFLAGS) $^ -o $@
	@echo "Compile $(JVM) OK"

# Build jvmp
$(JVMP) : $(OBJ) src/jvmp.o libs/libjar.a
	@$(CC) $(CFLAGS) $^ -o $@
	@echo "Compile $(JVMP) OK"

# Build jar
$(JAR) : src/jar.o libs/libjar.a
	@$(CC) $(CFLAGS) $^ -o $@
	@echo "Compile $(JAR) OK"

# Build rt.jar
$(BOOTSTRAP) : $(CLASSES)
	@./jar -c $(CLASSES) $(BOOTSTRAP)
	@echo "Build rt.jar OK"

# Build *.c
%.o : %.c %.d
	@echo "Compiling $@ "
	@$(CC) -c  $(CFLAGS) -o $@ $<

%.class : %.java
	@echo "javac $< "
	@javac $< 

%.d : %.c
	@echo "Generating $@"
	@$(CC) -MM  $(CFLAGS) $< > $@

.PHONEY:clean
clean:
	@rm -rf libs/libjar.a $(OBJ) $(BIN_OBJ) $(DEP) $(CLASSES) $(PROJ)
