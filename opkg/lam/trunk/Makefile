PKGDEST=/tmp

deb: clean
	@echo "Debian package(s) not yet available"

rpm: clean
	/usr/bin/build_package --type rpm --output $(PKGDEST) --url http://www.csm.ornl.gov/srt/downloads/oscar/lam-7.1.4.tar.gz --package-name lam --verbose

clean:
