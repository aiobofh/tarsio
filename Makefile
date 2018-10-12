#
# Makefile for Tarsio
#

export VERSION:=0.0.1

all:
	@$(MAKE) --no-print-directory -C src

.NOTPARALLEL: check
.PHONY: check
check: clean
	@echo "----------------------------------------------------------" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	$(MAKE) --no-print-directory -C src && \
	$(MAKE) --no-print-directory -C test && \
	$(MAKE) --no-print-directory -C src clean && \
	$(MAKE) --no-print-directory -C test clean && \
	$(MAKE) --no-print-directory -C src SASC=1 && \
	$(MAKE) --no-print-directory -C test SASC=1 && \
	$(MAKE) --no-print-directory -C src clean && \
	$(MAKE) --no-print-directory -C test clean

#	git status | grep 'git addd' >/dev/null && ((echo "WARNING: There are untracked files, investigate with 'git status'." >&2 && false) || (true)) && \
#	git status | grep 'git reset' >/dev/null && ((echo "WARNING: There are uncommitted changes, investigate with 'git status'." >&2 && false) || (true)) && \

.NOTPARALLEL: tarsio-${VERSION}.tar.gz
tarsio-${VERSION}.tar.gz: check
	@echo "----------------------------------------------------------" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	${MAKE} --no-print-directory -C src clean && \
	${MAKE} --no-print-directory -C test clean && \
	mkdir -p tarsio-${VERSION} && \
	cp -r README Makefile src include doc test tarsio-${VERSION}/. && \
	tar -c tarsio-${VERSION} | gzip > $@ && \
	rm -rf tarsio-${VERSION}

.NOTPARALLEL: tarsio-${VERSION}-src.lha
tarsio-${VERSION}-src.lha: check
	@echo "----------------------------------------------------------" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	${MAKE} --no-print-directory -C src clean && \
	${MAKE} --no-print-directory -C test clean && \
	mkdir -p tarsio-${VERSION} && \
	mkdir -p tarsio-${VERSION}/src && \
	cp -r README Makefile src include doc test tarsio-${VERSION}/src/. && \
	jlha -c $@ tarsio-${VERSION} && \
	rm -rf tarsio-${VERSION}

.NOTPARALLEL: tarsio-${VERSION}-sasc-68000-bin.lha
tarsio-${VERSION}-sasc-68000-bin.lha: check
	@echo "----------------------------------------------------------" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	${MAKE} --no-print-directory -C src clean && \
	${MAKE} --no-print-directory -C test clean && \
	mkdir -p tarsio-${VERSION} && \
	mkdir -p tarsio-${VERSION}/c && \
	mkdir -p tarsio-${VERSION}/libs && \
	cp -r README Makefile include doc tarsio-${VERSION}/. && \
	${MAKE} --no-print-directory -C src SASC=1 && \
	cp src/tcg src/tsg src/tmg src/tam src/ttg tarsio-${VERSION}/c/. && \
	jlha -c $@ tarsio-${VERSION} && \
	rm -rf tarsio-${VERSION}

.NOTPARALLEL: source-dist
source-dist: tarsio-${VERSION}.tar.gz tarsio-${VERSION}-src.lha
	@echo "----------------------------------------------------------" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	tar xzf tarsio-${VERSION}.tar.gz && ${MAKE} -C tarsio-${VERSION} && rm -rf tarsio-${VERSION} && \
	jlha -x tarsio-${VERSION}-src.lha && ${MAKE} -C tarsio-${VERSION}/src check SASC=1 && rm -rf tarsio-${VERSION}

.NOTPARALLEL: bin-dist
bin-dist: tarsio-${VERSION}-sasc-68000-bin.lha

.NOTPARALLEL: dist
.PHONY: dist
dist: source-dist bin-dist

.NOTPARALLEL: clean
clean:
	@echo "----------------------------------------------------------" && \
	echo $@ && \
	echo "----------------------------------------------------------" && \
	$(MAKE) --no-print-directory -C src clean && \
	$(MAKE) --no-print-directory -C test clean && \
	$(RM) -rf *~ include/*~ *.uaem tarsio-${VERSION}* tarsio*.lha # && \
#	git status | grep 'git addd' >/dev/null && (echo "WARNING: There are untracked files, investigate with 'git status'." >&2 && false) || (true)
