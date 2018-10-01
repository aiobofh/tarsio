#
# Makefile for Tarsio
#

export VERSION:=0.0.1

all:
	$(MAKE) -C src

.PHONY: check
check:
	$(MAKE) -C src
	$(MAKE) -C test

tarsio-${VERSION}.tar.gz: check
	${MAKE} clean && \
	git status | grep 'git add' >/dev/null && ((echo "WARNING: There are untracked files, investigate with 'git status'." >&2 && false) || (true)) && \
	git status | grep 'git reset' >/dev/null && ((echo "WARNING: There are uncommitted changes, investigate with 'git status'." >&2 && exit 1) || (true)) && \
	mkdir -p tarsio-${VERSION} && \
	cp -r README Makefile src include doc test tarsio-${VERSION}/. && \
	tar -c tarsio-${VERSION} | gzip > $@ && \
	rm -rf tarsio-${VERSION}

source-dist: tarsio-${VERSION}.tar.gz
	tar xzf $< && cd tarsio-${VERSION} && ${MAKE} check && rm -rf tarsio-${VERSION}

.PHONY: dist
dist: source-dist

clean:
	@$(MAKE) -C src clean && \
	$(MAKE) -C test clean && \
	$(RM) -rf *~ include/*~ *.uaem tarsio-${VERSION}* && \
	git status | grep 'git add' >/dev/null && (echo "WARNING: There are untracked files, investigate with 'git status'." >&2 && false) || (true)
