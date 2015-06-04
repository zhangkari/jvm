OBJDIR = target
PROJ = jvm
CC = gcc
CFLAGS =  -Iinclude \
		  -g \
	      -DDEBUG #-m32

LDFLAGS = -pthread
SRC = $(wildcard *.c) $(wildcard src/*.c)
OBJ = $(patsubst %.c,$(OBJDIR)/%.o,$(SRC))
DEP = $(patsubst %.c,$(OBJDIR)/%.d,$(SRC))

VPATH = src:include

$(PROJ) : $(OBJDIR) $(DEP) $(OBJ)
	@make -s -C libs
	@$(CC) $(OBJ) $(CFLAGS) $(LDFLAGS) -o $@
	@echo "OK"

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
	@mkdir -p $(OBJDIR)/src

.PHONEY:clean
clean:
	@make clean -s -C libs
	@rm -rf $(OBJDIR) $(PROJ)
