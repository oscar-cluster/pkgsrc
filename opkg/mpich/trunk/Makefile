PKGDEST=/tmp

deb: clean
	@echo "Debian package(s) not yet available"

rpm: clean
	/usr/bin/build_package --type rpm --output $(PKGDEST) --url http://www.csm.ornl.gov/srt/downloads/oscar/mpich-1.2.7.tar.bz2 --package-name mpich --verbose

clean:
