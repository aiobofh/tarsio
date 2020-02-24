#
# Makefile to for building the Tarsio tool-chain man-pages
#
#               _______          _____ ___        ______
#                  |      ||    |         |    | |      |
#                  |      ||    |         |    | |      |
#                  |   ___||___ |         |___ | |______|
#
#               Copyright (C) 2020 AiO Secure Teletronics
#
#

Q?=@

RST2MAN=$(shell which rst2man)

ifeq (,${RST2MAN})
	RST2MAN=$(error rst2man is required to generate man-pages))
endif

all: tcg.3 tsg.3 tam.3 tmg.3 ttg.3

%.3: %.rst
	${Q}${RST2MAN} $< > $@

clean:
	${Q}${RM} *.3 *~
