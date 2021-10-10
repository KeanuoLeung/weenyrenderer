SYSCONF_LINK = g++
CPPFLAGS     =
LDFLAGS      =
LIBS         = -lm

DESTDIR = ./
TARGET  = main

OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: $(DESTDIR)$(TARGET)

$(DESTDIR)$(TARGET): $(OBJECTS)
	$(SYSCONF_LINK) -I. $(LDFLAGS) -o $(DESTDIR)$(TARGET) $(OBJECTS) $(LIBS) -Wall -g

$(OBJECTS): %.o: %.cpp
	$(SYSCONF_LINK) -I. $(CPPFLAGS) -c $(CFLAGS) $< -o $@ -Wall -g

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)
	-rm -f *.tga