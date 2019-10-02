#Targets
programs := \
	dots.x \
	test.x \
	curves.x \
	lines.x \
	triangles.x \
	circles.x \
	curves2.x \
	field.x \
	field2.x \
	particle.x \
	colorTest.x \
	shapeTest.x \
	texture.x \
	twotone.x \
	sketch1.x \
	sketch2.x \
	sketch3.x

#Libraries
JMAPLIB := libJMAP
JMAPPATH := ../lib/
libJMAP := $(JMAPPATH)/$(JMAPLIB).a

#Default
all: $(libJMAP) $(programs)

#Compiler
CC := g++

#General Flags
CFLAGS := -Wall -g -std=c++11 -fPIC -no-pie

#Linker options
LDFLAGS := -L$(JMAPPATH) -lJMAP

#Include options
INCLUDE := -I$(JMAPPATH)

#Dependencies
DEPFLAGS = -MMD -MF $(@:.o=.d)

#Objects
objs := $(patsubst %.x,%.o,$(programs))

#Include Dependencies
deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)

#libJMAP.a

$(libJMAP):
	$(MAKE) -C $(JMAPPATH)

%.x: %.o $(libJMAP)
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $< $(DEPFLAGS)

clean:
	rm -rf *.o *.a *.x
