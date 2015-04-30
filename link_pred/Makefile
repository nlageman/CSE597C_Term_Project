CC = gcc
EXEC = link_pred
GDB_EXEC = $(EXEC:%=g_%)
PROF_EXEC = $(EXEC:%=p_%)
HDIRS = -Ih -I/opt/local/include
CDIR = c/
CFLAGS = -Wall -std=gnu99 $(HDIRS)
LLIBS = -L/opt/local/lib -ligraph
LIBS = $(CDIR)graph.c $(CDIR)community_metrics.c 
RM = -rm -f


all: $(EXEC)

$(EXEC): % : $(CDIR)%.c $(LIBS)
	$(CC) -o $@ $(CFLAGS) -O3 $< $(LIBS) $(LLIBS)

$(GDB_EXEC): g_% : $(CDIR)%.c $(LIBS)
	$(CC) -o $@ $(CFLAGS) -ggdb3 $< $(LIBS) $(LLIBS)

$(PROF_EXEC): p_% : $(CDIR)%.c $(LIBS)
	$(CC) -o $@ $(CFLAGS) -O3 -pg $< $(LIBS) $(LLIBS)

clean:
	$(RM) $(EXEC) $(GDB_EXEC) $(PROF_EXEC)

#Print variable (for debugging Makefile)
#PHONY: all
#all: ; $(info $$GDB_EXEC is [${GDB_EXEC}])
