DESTDIR=/tmp

deb: clean
	/usr/bin/build_package --type deb --output $(PKGDEST) --url http://www.csm.ornl.gov/srt/downloads/oscar/torque_2.1.10.tar.gz --package-name torque --verbose

rpm: clean
	/usr/bin/build_package --type rpm --output $(PKGDEST) --url http://www.csm.ornl.gov/srt/downloads/oscar/torque_2.1.10.tar.gz --package-name torque --verbose

clean:
