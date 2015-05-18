CC		=	gcc

ifndef CFLAGS
	CFLAGS = -MMD -Wall -g
endif
CFLAGS	+=	-I. -lcrypt

TARGET	=	zftp
OBJS	=	main.o hash.o utils.o tunables.o parseconf.o str.o prelogin.o connrec.o ftpcode.o\
			ftpcmdio.o privsock.o process.o zsignal.o privops.o transfer.o

TPO		= 	testparseconf.o parseconf.o str.o tunables.o utils.o
THO		= 	testhash.o hash.o

all : $(TARGET)
.PHONY : all

include $(SRCS: .o=.d)

%.d: %.c
	@set -e; rm -f $@;\
	$(CC) $(CFLAGS) $< > $@.$$$$;\
	sed 's,\($*\)\.o[:]*,\1.o $@:,g' < $@.$$$$ > $@;\
	rm -f $@.$$$$

$(TARGET): $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(CFLAGS)

*.o: Makefile

testhash: $(THO)
	$(CC) -o testhash $(THO)

testparseconf: $(TPO)
	$(CC) -o testparseconf $(TPO)


.PHONY : clean cleanall
clean:
	-rm -f *.o $(TARGET) *.d

cleanall:
	-rm -f *.o $(TARGET) *.d testhash testparseconf