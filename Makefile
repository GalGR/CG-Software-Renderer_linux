SCREEN_PIXELS_IMPLEMENTATION = SCREEN_PIXELS_BUFFER_IMPLEMENT

MODE ?= release

CC = g++
CXXFLAGS = -std=c++14 -Wall -Isrc -D$(SCREEN_PIXELS_IMPLEMENTATION) -include cmath -include algorithm
# LDFLAGS = -Wl,-rpath,$$ORIGIN -lX11 -Lbin -lfreeglut -lAntTweakBar -lGLEW -lGLU -lGL
LDFLAGS = -Wl,-rpath,$$ORIGIN:$$PWD:. -lX11

ifeq ($(MODE),debug)
CXXFLAGS += -g
else
CXXFLAGS += -O3
endif

APPNAME = cg
EXT = .cpp
# SRCDIR = src
SRCDIR = src
OBJDIR = obj
DEPDIR = dep
BINDIR = bin

LDFLAGS += -L$(BINDIR)

# osdialog
ARCH=gtk3
OSDIALOGDIR = $(SRCDIR)/osdialog
OSDIALOGNAME = $(shell make -s C $(OSDIALOGDIR) ARCH=$(ARCH) name)
OSDIALOGLIB = $(shell make -s -C $(OSDIALOGDIR) ARCH=$(ARCH) libname)
OSDIALOG = $(OSDIALOGDIR)/$(OSDIALOGLIB)
LDFLAGS += $(shell make -s -C $(OSDIALOGDIR) ARCH=$(ARCH) ldflags)
# LDFLAGS += -l$(OSDIALOGNAME)
# LDFLAGS += -L$(OSDIALOGDIR) -l$(shell make -s -C $(OSDIALOGDIR) ARCH=$(ARCH) name)

# Freeglut
FREEGLUTNAME = freeglut
FREEGLUTDIR = $(SRCDIR)/freeglut
FREEGLUTLIB = lib$(FREEGLUTNAME).so
FREEGLUTLIBVER = $(FREEGLUTLIB).3
FREEGLUT = $(FREEGLUTDIR)/lib/lib$(FREEGLUTNAME).so
FREEGLUTBIN = $(BINDIR)/$(FREEGLUTLIB)
FREEGLUTBINVER = $(BINDIR)/$(FREEGLUTLIBVER)
LDFLAGS += -l$(FREEGLUTNAME)

# AntTweakBar
ANTTWEAKBARNAME = AntTweakBar
ANTTWEAKBARDIR = $(SRCDIR)/AntTweakBar
ANTTWEAKBARLIB = lib$(ANTTWEAKBARNAME).so
ANTTWEAKBARLIBVER = $(ANTTWEAKBARLIB).1
ANTTWEAKBAR = $(ANTTWEAKBARDIR)/lib/$(ANTTWEAKBARLIB)
ANTTWEAKBARBIN = $(BINDIR)/$(ANTTWEAKBARLIB)
ANTTWEAKBARBINVER = $(BINDIR)/$(ANTTWEAKBARLIBVER)
LDFLAGS += -l$(ANTTWEAKBARNAME)

# Glew
GLEWNAME = GLEW
GLEWDIR = $(SRCDIR)/Glew
GLEWLIB = lib$(GLEWNAME).so
GLEWLIBVER = $(GLEWLIB).2.1
# GLEW = $(GLEWLIB:%.so=$(GLEWDIR)/lib/%.so)
GLEW = $(GLEWDIR)/lib/$(GLEWLIB)
GLEWBIN = $(BINDIR)/$(GLEWLIB)
GLEWBINVER = $(BINDIR)/$(GLEWLIBVER)
LDFLAGS += -l$(GLEWNAME) -lGLU -lGL

SRC = $(wildcard $(SRCDIR)/*$(EXT))
OBJ = $(SRC:$(SRCDIR)/%$(EXT)=$(OBJDIR)/%.o)
DEP = $(OBJ:$(OBJDIR)/%.o=$(DEPDIR)/%.d)
# DEP = $(OBJ:$(OBJDIR)/%.o=%.d)
BIN = $(BINDIR)/$(APPNAME)
# UNIX-based OS variables & settings
RM = rm
DELOBJ = $(OBJ)
# Windows OS variables & settings
DEL = del
EXE = .exe
WDELOBJ = $(SRC:$(SRCDIR)/%$(EXT)=$(OBJDIR)\\%.o)

########################################################################
####################### Targets beginning here #########################
########################################################################

all: $(BIN) $(SRCDIR) $(OBJDIR) $(DEPDIR) $(BINDIR)

$(OBJDIR):
	mkdir --parents $(OBJDIR)

$(DEPDIR):
	mkdir --parents $(DEPDIR)

$(BINDIR):
	mkdir --parents $(BINDIR)

# Builds the app
$(BIN): $(OBJ) $(OSDIALOG) $(OBJDIR)/Obj_Parser/wavefront_obj.o $(FREEGLUTBIN) $(ANTTWEAKBARBIN) $(GLEWBIN)
	$(CC) $(CXXFLAGS) -o $@ $(OBJ) $(OSDIALOG) $(OBJDIR)/Obj_Parser/wavefront_obj.o $(LDFLAGS)

$(OSDIALOG):
	$(MAKE) -C $(OSDIALOGDIR) ARCH=$(ARCH)

$(OBJDIR)/Obj_Parser/wavefront_obj.o: $(SRCDIR)/Obj_Parser/wavefront_obj$(EXT)
	mkdir --parents obj/Obj_Parser
	$(CC) $(CXXFLAGS) -o $(OBJDIR)/Obj_Parser/wavefront_obj.o -c $(SRCDIR)/Obj_Parser/wavefront_obj$(EXT) $(LDFLAGS)

$(FREEGLUTBIN):
	mkdir --parents $(FREEGLUTDIR)/build
	cd $(FREEGLUTDIR)/build && cmake -D CMAKE_BUILD_TYPE=Release -D FREEGLUT_BUILD_DEMOS=OFF -D FREEGLUT_BUILD_SHARED_LIBS=ON -D FREEGLUT_REPLACE_GLUT=OFF ..
	$(MAKE) -C $(FREEGLUTDIR)/build all
	cp -fL $(FREEGLUT) $(FREEGLUTBIN)
	ln -frs $(FREEGLUTBIN) $(FREEGLUTBINVER)

$(ANTTWEAKBARBIN):
	$(MAKE) -C $(ANTTWEAKBARDIR)/src
	cp -fL $(ANTTWEAKBAR) $(ANTTWEAKBARBIN)
	ln -frs $(ANTTWEAKBARBIN) $(ANTTWEAKBARBINVER)

$(GLEWBIN):
	$(MAKE) -C $(GLEWDIR)
	cp -fL $(GLEW) $(GLEWBIN)
	ln -frs $(GLEWBIN) $(GLEWBINVER)

# Creates the dependecy rules
$(DEPDIR)/%.d: $(SRCDIR)/%$(EXT)
	@$(CPP) $(CXXFLAGS) $< -MM -MT $(@:$(DEPDIR)/%.d=$(OBJDIR)/%.o) >$@

# Includes all .d files (execute all the dependencies)
-include $(DEP)

# Building rule for .o files and its .c/.cpp in combination with all .h
$(OBJDIR)/%.o: $(SRCDIR)/%$(EXT)
	$(CC) $(CXXFLAGS) -o $@ -c $<

################### Cleaning rules for Unix-based OS ###################
# Cleans complete project
.PHONY: clean
clean:
	-$(RM) -f $(DELOBJ) $(DEP) $(APPNAME)
	-$(MAKE) -C $(OSDIALOGDIR) ARCH=$(ARCH) clean
	-$(RM) -rf $(OBJDIR)/Obj_Parser
	-$(RM) -f $(BINDIR)/*
	-$(MAKE) -C $(FREEGLUTDIR) -f MakefileCMake clean
	-$(MAKE) -C $(ANTTWEAKBARDIR)/src clean
	-$(MAKE) -C $(GLEWDIR) clean

# Cleans only all files with the extension .d
.PHONY: cleandep
cleandep:
	-$(RM) -f $(DEP)

#################### Cleaning rules for Windows OS #####################
# Cleans complete project
.PHONY: cleanw
cleanw:
	-$(DEL) $(WDELOBJ) $(DEP) $(APPNAME)$(EXE)
	-$(MAKE) -C osdialog clean

# Cleans only all files with the extension .d
.PHONY: cleandepw
cleandepw:
	-$(DEL) $(DEP)