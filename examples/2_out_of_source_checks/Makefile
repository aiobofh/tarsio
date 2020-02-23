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

all:
	${Q}${MAKE} --no-print-directory -C src

check:
	${Q}${MAKE} --no-print-directory -C test

xml:
	${Q}${MAKE} --no-print-directory -C test xml

tarsio_info:
	${Q}${MAKE} --no-print-directory -C test tarsio_info

clean:
	${Q}${MAKE} --no-print-directory -C test clean && \
	${MAKE} --no-print-directory -C src clean && \
	${RM} *~


