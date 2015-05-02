CC		=	gcc
CFLAGS	=	-I. -g

TARGET	=	zftp
OBJS	=	main.o hash.o utils.o tunables.o parseconf.o str.o prelogin.o connrec.o ftpcode.o\
			ftpcmdio.o privsock.o process.o zsignal.o

DEPS	=	hash.h utils.h tunables.h session.h connrec.h parseconf.h str.h ftpcode.h err.h\
			prelogin.h

TPO		= 	testparseconf.o parseconf.o str.o tunables.o utils.o
THO		= 	testhash.o hash.o

all : testhash testparseconf
.PHONY : all

.c.o:
	$(CC) -c $(CFLAGS) -o $@ $<

$(TARGET): $(OBJS) $(DEPS)
	$(CC) -o $(TARGET) $(OBJS)

testhash: hash.h $(THO)
	$(CC) -o testhash $(THO)

testparseconf: parseconf.h tunables.h $(TPO)
	$(CC) -o testparseconf $(TPO)


.PHONY : clean cleanall
clean:
	-rm -f *.o $(TARGET)

cleanall:
	-rm -f *.o $(TARGET) testhash testparseconf