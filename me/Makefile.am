ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src
if ENABLE_ETC
SUBDIRS += etc
endif
ME = $(PACKAGE_NAME)-$(PACKAGE_VERSION)
ME_TARBALL = $(ME).tar.gz

.PHONY: rpm
rpm: dist-gzip
	$(MKDIR_P) rpm/{BUILD,RPMS,SOURCES,SPECS,SRPMS}
	cp $(ME_TARBALL) rpm/SOURCES
	rpmbuild -ba me.spec

rpm-clean:
	rm -rf rpm/
	rm -f $(ME_TARBALL)

PEDIGREE = $(DESTDIR)/$(bindir)/me-pedigree
include $(top_srcdir)/../automake/pedigree.am

clean-local: rpm-clean
