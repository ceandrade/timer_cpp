###############################################################################
# Author: Carlos Eduardo de Andrade <ce.andrade@gmail.com>
#
# (c) Copyright 2021. All Rights Reserved.
#
#  Created on : Jun 17, 2021 by andrade
#  Last update: Jun 17, 2021 by andrade
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
###############################################################################

###############################################################################
# User Defines
###############################################################################

# Optimization switch
# OPT = opt

# Set debug mode. Either...
USER_DEFINES += -DDEBUG

###############################################################################
# Build options
###############################################################################

#########################
# The user source files
#########################

# Include dir
USER_INCDIRS = . ..

###############################
# Objects and test files
###############################

EXECUTION_STOPPER_OBJ = ../timer/execution_stopper.o

TEST_TIMER_OBJ = ./test_timer.o
TEST_TIMER_EXE = ./test_timer

TEST_EXECUTION_STOPPER_OBJ = ./test_execution_stopper.o
TEST_EXECUTION_STOPPER_EXE = ./test_execution_stopper

###############################################################################
# Compiler flags
###############################################################################

ifeq ($(shell uname -s),Darwin)
    SYSTEM = macosx
else
    SYSTEM = linux
endif

ifeq ($(SYSTEM), macosx)
	CXX = clang++
	CXXFLAGS=-Xpreprocessor -fopenmp
	# CXXFLAGS=-stdlib=libc++ -fopenmp

	# CXX = clang++-mp-10
	# CXXFLAGS=-fopenmp
else
	CXX = g++
endif

# Define the usage of C++17.
USER_FLAGS += -std=c++17

# Compiler flags for debugging
ifneq ($(OPT), opt)
	USER_FLAGS += -ggdb3 -fexceptions -fno-omit-frame-pointer \
		-fno-optimize-sibling-calls -fno-inline
else
	USER_FLAGS += -O3 -fomit-frame-pointer -funroll-loops
	ifeq ($(CXX), g++)
		USER_FLAGS += -ftracer -fpeel-loops -fprefetch-loop-arrays
	endif
endif

# Include parallel stuff
USER_FLAGS += -pthread

# Flags
USER_FLAGS += -Wall -Wextra -Wcast-align -Wcast-qual -Wdisabled-optimization \
	-Wformat=2 -Winit-self -Wmissing-format-attribute -Wshadow \
	-Wpointer-arith -Wredundant-decls -Wstrict-aliasing=2 \
	-Wfloat-equal -Weffc++

# Paranoid mode!!!
# Used to analyze format of structures
#USER_FLAGS += -Wpacked -Wpadded
#USER_FLAGS += -Wconversion
#USER_FLAGS += -Wunsafe-loop-optimizations

###############################################################################
# Compiler and linker defs
###############################################################################

# C Compiler command and flags
CXXFLAGS += $(USER_FLAGS)

SHELL = /bin/bash

# Necessary Include dirs
# Put -I in front of dirs
INCLUDES = `for i in $(USER_INCDIRS); do echo $$i | sed -e 's/^/-I/'; done`

# List all libs
LIBS = -lm
ifeq ($(CXX), clang++)
	LIBS += -lomp
endif

ifneq ($(findstring DEBUG,$(USER_DEFINES)), DEBUG)
    USER_DEFINES += -DNDEBUG
endif

###############################################################################
# Build Rules
###############################################################################

#GIT_VERSION := $(shell git describe --abbrev=4 --always --tags)
#GIT_VERSION := $(shell git describe --always --tags)

###############################################################################
# Build Rules
###############################################################################

.PHONY: all static clean
.SUFFIXES: .cpp .o

all: test_execution_stopper

test_timer: $(TEST_TIMER_OBJ)
	@echo "--> Linking objects... "
	$(CXX) $(CXXFLAGS) $(TEST_TIMER_OBJ) -o $(TEST_TIMER_EXE)

	@echo
	@echo "--> Running tests..."
	$(TEST_TIMER_EXE)
	@echo

test_execution_stopper: $(EXECUTION_STOPPER_OBJ) $(TEST_EXECUTION_STOPPER_OBJ)
	@echo "--> Linking objects... "
	$(CXX) $(CXXFLAGS) $(EXECUTION_STOPPER_OBJ) $(TEST_EXECUTION_STOPPER_OBJ) \
	-o $(TEST_EXECUTION_STOPPER_EXE)

	@echo
	@echo "--> Running tests..."
	$(TEST_EXECUTION_STOPPER_EXE)
	@echo

# static: $(OBJS) $(TEST_MAIN)
# 	@echo "--> Linking objects... "
# 	$(CXX) -static -static-libgcc -static-libstdc++ $(CXXFLAGS) \
# 	$(OBJS) $(TEST_MAIN) -o $(TEST_EXE)-static \
# 	-lm -lbz2 -lz
# 	@echo

.cpp.o:
	@echo "--> Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(USER_DEFINES) -c $< -o $@
	@echo

clean:
	@echo "--> Cleaning compiled..."
	rm -rf $(EXECUTION_STOPPER_OBJ) $(TEST_TIMER_OBJ) $(TEST_EXECUTION_STOPPER_OBJ)
	rm -rf $(TEST_TIMER_EXE) $(TEST_EXECUTION_STOPPER_EXE)
	rm -rf *o
	rm -rf Debug
	rm -rf *.dSYM
