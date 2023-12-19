#---------------------------------------------------------------------------------------------------
# Microtest example and test build file
#
# Requires GNU make version >= v4.0 and basic UNIX tools (for Windows
# simply install GIT globally, so that tools like rm.exe are in the PATH;
# Linux/BSD: no action needed).
#---------------------------------------------------------------------------------------------------
# Optional tool chain prefix path, sometimes also referred to as CROSS_COMPILE, mainly suitable
# to switch between compilers.
TOOLCHAIN=

# Compiler and dependency selection
CXX=$(TOOLCHAIN)g++
LD=$(CXX)
CXX_STD=c++17
FLAGSCXX=-std=$(CXX_STD) -W -Wall -Wextra -pedantic
FLAGSCXX+=-I./include
SCM_COMMIT:=$(shell git log --pretty=format:%h -1 2>/dev/null || echo 0000000)
BUILD_DIRECTORY=build

#---------------------------------------------------------------------------------------------------
ifdef DEBUG
 FLAGSCXX+=-Os -g
 FLAGSLD+=-Os -g
else
 FLAGSCXX+=-Os
 FLAGSLD+=-Os
endif

# make command line overrides from the known teminologies
# (CXXFLAGS, LDFLAGS) without completely replacing the
# previous settings.
FLAGSCXX+=$(FLAGS)
FLAGSCXX+=$(CXXFLAGS)
FLAGSLD+=$(LDFLAGS)

# Pick windows or unix-like
ifeq ($(OS),Windows_NT)
 BUILDDIR:=./$(BUILD_DIRECTORY)
 BINARY_EXTENSION=.exe
 LDSTATIC+=-static
 FLAGSCXX+=-D_WIN32_WINNT=0x0601 -DWINVER=0x0601 -D_WIN32_IE=0x0900
 LIBS+=-ladvapi32 -lshell32
else
 BUILDDIR:=./$(BUILD_DIRECTORY)
 BINARY_EXTENSION=.elf
 LIBS+=-lm -lrt
 ifdef STATIC
  LDSTATIC+=-static
 endif
endif

#---------------------------------------------------------------------------------------------------
# make targets
#---------------------------------------------------------------------------------------------------
.PHONY: default clean all test mrproper help

default:
	@$(MAKE) -j test

clean:
	@$(MAKE) test-clean
	-@rm -rf $(BUILDDIR)

mrproper: clean

all: clean
	@$(MAKE) test-clean
	@mkdir -p $(BUILDDIR)
	@$(MAKE) -j test BUILD_DIRECTORY=$(BUILD_DIRECTORY)/std11 CXX_STD=c++11 TOOLCHAIN=$(TOOLCHAIN)
	@$(MAKE) -j test BUILD_DIRECTORY=$(BUILD_DIRECTORY)/std14 CXX_STD=c++14 TOOLCHAIN=$(TOOLCHAIN)
	@$(MAKE) -j test BUILD_DIRECTORY=$(BUILD_DIRECTORY)/std17 CXX_STD=c++17 TOOLCHAIN=$(TOOLCHAIN)
	@$(MAKE) -j test BUILD_DIRECTORY=$(BUILD_DIRECTORY)/std20 CXX_STD=c++20 TOOLCHAIN=$(TOOLCHAIN)

help:
	@echo "Usage: make [ clean all test ]"
	@echo ""
	@echo " - test:           Build test binaries, run all tests that have changed."
	@echo " - all:            Run tests for standards c++11, c++14, c++17, c++20"
	@echo " - clean:          Clean binaries, temporary files and tests."
	@echo ""


include test/testenv.mk
include test/sanitize.mk

#--
