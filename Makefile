CC = gcc
RM = /bin/rm
CPPFLAGS =
#CFLAGS = -O2 -g -Wall -pedantic 
CFLAGS = -O2
LDFLAGS =
HEADERS =
SRCS = iwu.c
OBJS = iwu.o
LIBS = -lnet

all: iwu

iwu:	$(OBJS)
	$(CC) $(LDFLAGS) -o $@ $? $(LIBS) ; \

iwu.o: $(SRCS) $(HEADERS)
#	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c $(SRCS)

dist:   all
	mkdir IDSwakeup-1.0
	cp IDSwakeup iwu.c README LICENSE Makefile IDSwakeup-1.0/
	tar cvzf IDSwakeup-1.0.tgz IDSwakeup-1.0/
	rm -fr ./IDSwakeup-1.0/

clean:
	$(RM) -f $(OBJS) iwu *~
