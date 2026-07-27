###############################################################################
# @file Makefile
# @brief: Build the single translation unit usage example.
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

TARGET = single_tu
SRC = main.cpp

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
