COMMON        = ../../../
COMMON_TYPES  = $(COMMON)/types
SIGNAL_API    = $(COMMON)/signal_api

INTERFACE       = ../interface

CC            = gcc
CFLAGS        = -pipe -g -Wall -W
CXXFLAGS      = -pipe -g -Wall -W
INCPATH       = -I$(COMMON_TYPES)/inc -I$(SIGNAL_API)/inc -I$(INTERFACE)/inc -I.
LINK          = gcc
LFLAGS        = 
LIBS          = -lpthread 
DEL_FILE      = rm -f
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = .

####### Files

OBJECTS       = $(OBJECTS_DIR)/main.o \
				$(OBJECTS_DIR)/interface.o \
				$(OBJECTS_DIR)/interface_general.o

TARGET        = $(OBJECTS_DIR)/server

first: all
####### Implicit rules

.SUFFIXES: .o .c

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET):  $(OBJECTS)  
	$(CC) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)

clean:
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) $(OBJECTS_DIR)/*~ $(OBJECTS_DIR)/core $(OBJECTS_DIR)/*.core
	-$(DEL_FILE) $(TARGET)

####### Compile

$(OBJECTS_DIR)/main.o: main.c
	$(CC) -c $(CFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/main.o main.c

$(OBJECTS_DIR)/interface.o: $(SIGNAL_API)/inc/interface_general.h \
				 $(SIGNAL_API)/src/interface_general.c \
				 $(INTERFACE)/inc/interface.h \
				 $(COMMON_TYPES)/inc/types_general.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/interface.o $(INTERFACE)/src/interface.c

$(OBJECTS_DIR)/interface_general.o: $(SIGNAL_API)/src/interface_general.c \
					 				$(SIGNAL_API)/inc/interface_general.h \
			  					    $(COMMON_TYPES)/inc/types_general.h
	$(CC) -c $(CFLAGS) $(INCPATH) -o $(OBJECTS_DIR)/interface_general.o $(SIGNAL_API)/src/interface_general.c

