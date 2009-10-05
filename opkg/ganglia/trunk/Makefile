PKGDEST= clean

deb: clean
	/usr/bin/build_package --type deb --output $(PKGDEST) --url http://www.csm.ornl.gov/srt/downloads/oscar/ganglia-3.0.6.tar.gz --package-name ganglia --verbose

rpm: clean
	/usr/bin/build_package --type rpm --output $(PKGDEST) --url http://www.csm.ornl.gov/srt/downloads/oscar/ganglia-3.0.6.tar.gz --package-name ganglia --verbose

clean:
