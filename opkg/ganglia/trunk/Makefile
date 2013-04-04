PKGDEST=/tmp

deb: clean
	/usr/bin/build_package --type deb --output $(PKGDEST) --url http://svn.oscar.openclustergroup.org/pkgs/downloads/ganglia-3.1.7.tar.gz --package-name ganglia --verbose
	@echo "Ganglia is not supported on Debian based systems yet"

rpm: clean
	#/usr/bin/build_package --type rpm --output $(PKGDEST) --url http://svn.oscar.openclustergroup.org/pkgs/downloads/ganglia-3.1.7.tar.gz --package-name ganglia --verbose

clean:
