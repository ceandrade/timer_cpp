###############################################################################
# @file Makefile
# @brief: Build the multi translation unit usage example.
#
# Each source file is compiled separately into its own object file and then
# linked together, demonstrating that the header-only ExecutionStopper
# singleton is shared across independently compiled translation units.
#
# SPDX-FileCopyrightText: 2015-2026 Carlos E. Andrade <ce.andrade@gmail.com>
# SPDX-License-Identifier: BSD-3-Clause.
#
# Created on : 2026-07-27 by ceandrade.
# Last update: 2026-07-27 by ceandrade.
###############################################################################

# Compiler is overridable, e.g. `make CXX=g++-15` or `make CXX=clang++`.
CXX ?= c++
CXXFLAGS ?= -std=c++23 -O2 -Wall -Wextra -pthread

# The library is header-only; only the repository root is needed on the
# include path so that the "timer/..." includes resolve.
INCLUDES = -I../..

TARGET = multi_tu
OBJS = main.o even_counter.o multiple_of_three_counter.o

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
