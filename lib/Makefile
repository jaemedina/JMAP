#Target Library

lib := libJMAP.a
objs := JMAP.o Polygon.o
compiled :=
CC := g++
CFLAGS := -Wall -g -std=c++11 -fPIC -no-pie

all: $(lib)

$(lib): $(objs) $(compiled)
	ar rcs $(lib) $(objs)

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm *.o $(lib)
