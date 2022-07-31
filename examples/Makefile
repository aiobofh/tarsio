#
# Makefile for Tarsio example projects
#
#               _______          _____ ___        ______
#                  |      ||    |         |    | |      |
#                  |      ||    |         |    | |      |
#                  |   ___||___ |         |___ | |______|
#
#               Copyright (C) 2020 AiO Secure Teletronics
#
# This Makefile will compile (and run) the examples projects for Tarsio.
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

Q=@
PREFIX=$(realpath ..)

export Q
export PATH:=../../src:${PATH}

#
# Just build the write_file program in the various examples (not very useful)
#
all:
	${Q}${MAKE} --no-print-directory -C 0_making_your_own_build_system_using_tarsio && \
	${MAKE} --no-print-directory -C 1_using_the_stock_build_system && \
	${MAKE} --no-print-directory -C 2_out_of_source_checks && \
	${MAKE} --no-print-directory -C 3_line_and_branch_coverage

#
# Perform the unit-checks in the various examples
#
check: compile_tarsio
	${Q}${MAKE} --no-print-directory -C 0_making_your_own_build_system_using_tarsio check && \
	${MAKE} --no-print-directory -C 1_using_the_stock_build_system check && \
	${MAKE} --no-print-directory -C 2_out_of_source_checks check && \
	${MAKE} --no-print-directory -C 3_line_and_branch_coverage check

#
# Produce JUnit XML output (and Cobertura XML output, if coverage enabled)
#
xml: compile_tarsio
	${MAKE} --no-print-directory -C 1_using_the_stock_build_system xml && \
	${MAKE} --no-print-directory -C 2_out_of_source_checks xml && \
	${MAKE} --no-print-directory -C 3_line_and_branch_coverage xml

#
# Helper target to show information about the Tarsio set-up for the examples
# using the tarsio.mk file.
#
tarsio_info:
	${Q}${MAKE} --no-print-directory -C 1_using_the_stock_build_system tarsio_info && \
	${MAKE} --no-print-directory -C 2_out_of_source_checks tarsio_info && \
	${MAKE} --no-print-directory -C 3_line_and_branch_coverage tarsio_info

.PHONY: compile_tarsio
compile_tarsio:
	${Q}${MAKE} --no-print-directory -C ..
#
# Cleanup all folders
#
clean:
	${Q}${MAKE} --no-print-directory -C 0_making_your_own_build_system_using_tarsio clean && \
	${MAKE} --no-print-directory -C 1_using_the_stock_build_system clean && \
	${MAKE} --no-print-directory -C 2_out_of_source_checks clean && \
	${MAKE} --no-print-directory -C 3_line_and_branch_coverage clean && \
	${RM} tarsio.pc
