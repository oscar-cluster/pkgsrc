DESTDIR=/tmp
PKGDEST=/tmp

deb: clean
	/usr/bin/build_package --type deb --output $(PKGDEST) --url http://www.csm.ornl.gov/srt/downloads/oscar/torque-4.1.4.tar.gz --package-name torque-oscar --verbose
	/usr/bin/build_package --type deb --output $(PKGDEST) --url http://www.csm.ornl.gov/srt/downloads/oscar/drmaa-0.5.tar.gz --package-name drmaa-python --verbose

rpm: clean
	/usr/bin/build_package --type rpm --output $(PKGDEST) --url http://www.csm.ornl.gov/srt/downloads/oscar/torque-4.1.4.tar.gz --package-name torque-oscar --verbose
	/usr/bin/build_package --type rpm --output $(PKGDEST) --url http://www.csm.ornl.gov/srt/downloads/oscar/drmaa-0.5.tar.gz --package-name drmaa-python --verbose

clean:
