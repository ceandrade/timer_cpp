###############################################################################
# SPDX-FileCopyrightText: 2015-2026 Carlos E. Andrade <ce.andrade@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause.
#
# Created on : 2015-06-17 by ceandrade.
# Last update: 2026-07-26 by ceandrade.
###############################################################################

###############################################################################
# User Defines
###############################################################################

# Optimization switch
OPT = opt

# Set debug mode. Either...
#USER_DEFINES += -DDEBUG

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

# Default compiler per platform. Override from the command line for CI,
# e.g. `make CXX=g++-15` or `make CXX=clang++-20`.
ifeq ($(SYSTEM), macosx)
	CXX = clang++
else
	CXX = g++
endif

USER_FLAGS += -std=c++23

# Compiler flags for debugging
ifeq ($(OPT), opt)
	USER_FLAGS += -O3 -fomit-frame-pointer -funroll-loops
	ifeq ($(CXX), g++)
		USER_FLAGS += -ftracer -fpeel-loops -fprefetch-loop-arrays
	endif
else
	USER_FLAGS += -ggdb2 -fexceptions -fno-omit-frame-pointer \
		-fno-optimize-sibling-calls -fno-inline
endif
# Include parallel stuff
USER_FLAGS += -pthread

# Flags
USER_FLAGS += -Wall -Wextra -Wcast-align -Wcast-qual -Wdisabled-optimization \
	-Wformat=2 -Winit-self -Wmissing-format-attribute -Wshadow \
	-Wpointer-arith -Wredundant-decls -Wstrict-aliasing=2 \
	-Wfloat-equal -Weffc++

# Used when we get memory errors.
# USER_FLAGS += -fsanitize=address

# Paranoid mode!!!
# Used to analyze format of structures
# USER_FLAGS += -Wpacked -Wpadded
# USER_FLAGS += -Wconversion
# USER_FLAGS += -Wunsafe-loop-optimizations

###############################################################################
# Compiler and linker defs
###############################################################################

# C Compiler command and flags. EXTRA_FLAGS is an empty hook meant to be
# set from the command line (e.g. for sanitizers in CI):
#     make EXTRA_FLAGS="-fsanitize=address,undefined"
CXXFLAGS += $(USER_FLAGS) $(EXTRA_FLAGS)

SHELL = /bin/bash

# Necessary Include dirs
# Put -I in front of dirs
INCLUDES = `for i in $(USER_INCDIRS); do echo $$i | sed -e 's/^/-I/'; done`

# List all libs
LIBS = -lm

ifneq ($(findstring DEBUG,$(USER_DEFINES)), DEBUG)
    USER_DEFINES += -DNDEBUG
endif

###############################################################################
# Build Rules
###############################################################################

.PHONY: all clean
.SUFFIXES: .cpp .o

all: test_timer test_execution_stopper

test_timer: $(TEST_TIMER_OBJ)
	@echo "--> Linking objects... "
	$(CXX) $(CXXFLAGS) $(TEST_TIMER_OBJ) -o $(TEST_TIMER_EXE)

	@echo
	@echo "--> Running tests..."
	$(TEST_TIMER_EXE)
	@echo

test_execution_stopper: $(TEST_EXECUTION_STOPPER_OBJ)
	@echo "--> Linking objects... "
	$(CXX) $(CXXFLAGS) $(TEST_EXECUTION_STOPPER_OBJ) \
	-o $(TEST_EXECUTION_STOPPER_EXE)

	@echo
	@echo "--> Running tests..."
	$(TEST_EXECUTION_STOPPER_EXE)
	@echo

.cpp.o:
	@echo "--> Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(USER_DEFINES) -c $< -o $@
	@echo

clean:
	@echo "--> Cleaning compiled..."
	rm -rf $(TEST_TIMER_OBJ) $(TEST_EXECUTION_STOPPER_OBJ)
	rm -rf $(TEST_TIMER_EXE) $(TEST_EXECUTION_STOPPER_EXE)
	rm -rf *o
	rm -rf Debug
	rm -rf *.dSYM
