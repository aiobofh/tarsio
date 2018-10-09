#
# Makefile for Tarsio
#

export VERSION:=0.0.1

all:
	@$(MAKE) --no-print-directory -C src

.PHONY: check
check:
	@$(MAKE) --no-print-directory -C src && \
	$(MAKE) --no-print-directory -C test && \
	$(MAKE) --no-print-directory -C src clean && \
	$(MAKE) --no-print-directory -C test clean && \
	$(MAKE) --no-print-directory -C src SASC=1 && \
	$(MAKE) --no-print-directory -C test SASC=1 && \
	$(MAKE) --no-print-directory -C src clean && \
	$(MAKE) --no-print-directory -C test clean

#	git status | grep 'git addd' >/dev/null && ((echo "WARNING: There are untracked files, investigate with 'git status'." >&2 && false) || (true)) && \
#	git status | grep 'git reset' >/dev/null && ((echo "WARNING: There are uncommitted changes, investigate with 'git status'." >&2 && false) || (true)) && \

tarsio-${VERSION}.tar.gz: check
	@${MAKE} --no-print-directory clean && \
	mkdir -p tarsio-${VERSION} && \
	cp -r README Makefile src include doc test tarsio-${VERSION}/. && \
	tar -c tarsio-${VERSION} | gzip > $@ && \
	rm -rf tarsio-${VERSION}

tarsio-src.lha: check
	@${MAKE} --no-print-directory clean && \
	mkdir -p tarsio-${VERSION}/src && \
	cp -r README Makefile src include doc test tarsio-${VERSION}/src/. && \
	vamos -c ~/.vamosrc lha $@ tarsio-${VERSION} && \
	rm -rf tarsio-${VERSION}

tarsio-sasc-68000-bin.lha: check
	@${MAKE} --no-print-directory clean && \
	mkdir -p tarsio-${VERSION}/c && \
	mkdir -p tarsio-${VERSION}/libs && \
	cp -r README Makefile include doc tarsio-${VERSION}/. && \
	${MAKE} --no-print-directory -C src SASC=1 && \
	cp src/tcg src/tsg src/tmg src/tam src/ttg tarsio-${VERSION}/c/. && \
	cp src/tarsio.o tarsio-${VERSION}/libs/. && \
	vamos -c ~/.vamosrc lha $@ tarsio-${VERSION} && \
	rm -rf tarsio-${VERSION}

source-dist: tarsio-${VERSION}.tar.gz
	tar xzf $< && cd tarsio-${VERSION} && ${MAKE} check && rm -rf tarsio-${VERSION}

.PHONY: dist
dist: source-dist

clean:
	@$(MAKE) --no-print-directory -C src clean && \
	$(MAKE) --no-print-directory -C test clean && \
	$(RM) -rf *~ include/*~ *.uaem tarsio-${VERSION}* tarsio.lha # && \
#	git status | grep 'git addd' >/dev/null && (echo "WARNING: There are untracked files, investigate with 'git status'." >&2 && false) || (true)
