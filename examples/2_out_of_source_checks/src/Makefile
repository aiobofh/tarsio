#
# Makefile for Tarsio example project
#
#               _______          _____ ___        ______
#                  |      ||    |         |    | |      |
#                  |      ||    |         |    | |      |
#                  |   ___||___ |         |___ | |______|
#
#             Copyright (C) 2020-2022 AiO Secure Teletronics
#
# This Makefile will compile (and run) the example project for Tarsio.
#
#  This file is part of Tarsio.
#
#  Tarsio is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  Tarsio is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with Tarsio.  If not, see <https://www.gnu.org/licenses/>.
#

all: write_file

#
# Build the write_file program
#
.PHONY: all
all: write_file
	${Q}echo "Built the write_file program, if you intended to run the" && \
	echo "checks just type 'make check' instead."

write_file: write_file.c
	${Q}gcc -O2 -Wall -std=c11 -pedantic -o $@ $^

clean::
	${Q}${RM} write_file *~
