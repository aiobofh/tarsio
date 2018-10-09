Release Process for Tarsio
==========================

The process is currently quite simple and requires write access to the
project's git repository.

Prepare the sources Version and Tag the release
-----------------------------------------------

* Run all builds and regression tests::

    make check

* Change in Makefile with the new version number (ex. 0.1.0)

* Create a doc/releases.rst page with highlights and a full changelog with
  the command::

    git shortlog v0.0.9

* Commit Makefile and the new releases.rst
* Test locally with "make release" that everything builds fine
* Tag the release in master like this::

    git tag v0.1.0

  This will apply the tag to the last commit
* Push the tag::

    git push origin v0.1.0

Create the release archives and SHA hash
----------------------------------------

* Run the following commands (on a git clean tree, please)::

    make dist

  This will generate all supperted archives named like: tarsio-0.1.0.tar.gz
  and generate checksum text file (sha512sum tarsio-0.1.0.tar.gz > tarsio.0.1.0.tar.gz.sha512sum.txt)

Publish the release
-------------------

* Upload tarball and checksum to Tarsio release page
* Copy the content of the new-release page into a mail and announce on the
  Tarsio mailinglist
