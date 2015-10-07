OBJDIR = target
JVM = jvm
JVMP = jvmp
PROJ = $(JVM) $(JVMP) 
CC = gcc
CFLAGS =  -Iinclude \
          -Ilibs \
		  -g \
	      -DDEBUG #-m32

LDFLAGS = -pthread
                     
SRC = $(wildcard src/vm/*.c) $(wildcard libs/*.c)
OBJ = $(patsubst %.c,$(OBJDIR)/%.o,$(SRC))
DEP = $(patsubst %.c,$(OBJDIR)/%.d,$(SRC))

MAIN_SRC= $(wildcard src/*.c)
MAIN_OBJ = $(patsubst %.c,$(OBJDIR)/%.o,$(MAIN_SRC))

VPATH = src:include

all : $(OBJDIR) $(DEP) ziplib $(OBJ) $(MAIN_OBJ) $(PROJ)
	@echo "Build OK"

ziplib:
	@make -s -C libs

$(JVM) : $(OBJ) $(OBJDIR)/src/jvm.o
	@$(CC) $(CFLAGS) $^ -o $@
	@echo "Compile $(JVM) OK"

$(JVMP) : $(OBJ) $(OBJDIR)/src/jvmp.o
	@$(CC) $(CFLAGS) $^ -o $@
	@echo "Compile $(JVMP) OK"

ifeq ($(wildcard target/src), )
	@mkdir -p target/src
endif

$(OBJDIR)/%.o:%.c $(OBJDIR)/%.d
	@echo "Compiling $@ "
	@$(CC) -c  $(CFLAGS) -o $@ $<

$(OBJDIR)/%.d:%.c
	@echo "Generating $@ "
	@$(CC) -MM  $(CFLAGS) $< > $@ 
	@sed -i 's/$(patsubst %.d,%.o,$(notdir $@))/$(patsubst %.d,%.o,$(notdir $@)) $(notdir $@)/' $@

$(OBJDIR):
	@mkdir -p $(OBJDIR)/libs
	@mkdir -p $(OBJDIR)/src/vm

.PHONEY:clean
clean:
	@rm -rf $(OBJDIR) $(PROJ)
