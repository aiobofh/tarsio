#
# Makefile for Tarsio example project
#
#               _______          _____ ___        ______
#                  |      ||    |         |    | |      |
#                  |      ||    |         |    | |      |
#                  |   ___||___ |         |___ | |______|
#
#               Copyright (C) 2020 AiO Secure Teletronics
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

#
# Speedup GNU Make a bit
#
MAKEFLAGS += --no-builtin-rules
.SUFFIXES:

#
# Configure Tarsio. Usually on an installed system pkg-config or TARSIOHOME
# is enough. But here, since it should be possible to run the examples
# without installing Tarsio anywhere a more specific configuration is needed
#
TARSIOHOME=../../..
TARSIOBINDIR?=${TARSIOHOME}/bin
TARSIOINCDIR?=${TARSIOHOME}/inc
TARSIOSRCDIR?=${TARSIOHOME}/src
TARSIOCFLAGS?=-I${TARSIOHOME}/inc

#
# Default clean builds
#
CFLAGS=-Wall -pedantic -std=c99 -I.

#
# Configure Tarsio to find the implementation source
# The trailing slash / is important!!!
#
TSRCROOT=../src/

#
# Depend on the Tarsio make system
#
include ${TARSIOINCDIR}/tarsio.mk
include ${TARSIOINCDIR}/coverage.mk
