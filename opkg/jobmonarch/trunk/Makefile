PKGDEST=/tmp

deb: clean
	@echo "Debian package(s) not yet available"

rpm: clean
	/usr/bin/build_package --type rpm --output $(PKGDEST) --url http://svn.oscar.openclustergroup.org/pkgs/downloads/jobmonarch-0.4.tar.gz --package-name jobmonarch --verbose

clean:
