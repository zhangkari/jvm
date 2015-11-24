OBJDIR = target
JVM = jvm
JVMP = jvmp
BOOTSTRAP = classes.zip
PROJ = $(JVM) $(JVMP) $(BOOTSTRAP)
CC = gcc
CFLAGS =  -Iinclude \
          -Ilibs \
		  -g \
	      -DDEBUG #-m32

LDFLAGS = -pthread
                     
SRC = $(wildcard src/vm/*.c) $(wildcard libs/*.c)
OBJ = $(patsubst %.c,$(OBJDIR)/%.o,$(SRC))
DEP = $(patsubst %.c,$(OBJDIR)/%.d,$(SRC))

JAVA_SRC = $(wildcard java/lang/*.java)
CLASSES = $(patsubst %.java, $(OBJDIR)/%.class, $(JAVA_SRC))

MAIN_SRC= $(wildcard src/*.c)
MAIN_OBJ = $(patsubst %.c,$(OBJDIR)/%.o,$(MAIN_SRC))

VPATH = src:include

all : $(OBJDIR) $(DEP) ziplib $(OBJ) $(MAIN_OBJ) \
			$(CLASSES) $(BOOTSTRAP) $(PROJ)
	@echo "Build OK"

# Build libz for jar
ziplib:
	@make -s -C libs

# Build jvm
$(JVM) : $(OBJ) $(OBJDIR)/src/jvm.o
	@$(CC) $(CFLAGS) $^ -o $@
	@echo "Compile $(JVM) OK"

# Build jvmp
$(JVMP) : $(OBJ) $(OBJDIR)/src/jvmp.o
	@$(CC) $(CFLAGS) $^ -o $@
	@echo "Compile $(JVMP) OK"

# Build bootstrap.zip
$(BOOTSTRAP) : $(CLASSES)
	@echo $^
	@echo "Build rt.jar OK"

# Make sure target/src exist
ifeq ($(wildcard target/src), )
	@mkdir -p target/src
endif

# Build *.c
$(OBJDIR)/%.o:%.c $(OBJDIR)/%.d
	@echo "Compiling $@ "
	@$(CC) -c  $(CFLAGS) -o $@ $<

$(OBJDIR)/%.class:%.java
	@echo "javac $< "
	@javac $< -d $(OBJDIR)

$(OBJDIR)/%.d:%.c
	@echo "Generating $@ "
	@$(CC) -MM  $(CFLAGS) $< > $@ 
	@sed -i 's/$(patsubst %.d,%.o,$(notdir $@))/$(patsubst %.d,%.o,$(notdir $@)) $(notdir $@)/' $@

$(OBJDIR):
	@mkdir -p $(OBJDIR)/libs
	@mkdir -p $(OBJDIR)/src/vm
	@mkdir -p $(OBJDIR)/src/java
	@mkdir -p $(OBJDIR)/src/java/lang

.PHONEY:clean
clean:
	@rm -rf $(OBJDIR) $(PROJ)
