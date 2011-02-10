DESTDIR=/tmp
PKGDEST=/tmp

deb: clean
	/usr/bin/build_package --type deb --output $(PKGDEST) --url http://www.csm.ornl.gov/srt/downloads/oscar/torque-2.1.10.tar.gz --package-name torque --verbose

rpm: clean
	/usr/bin/build_package --type rpm --output $(PKGDEST) --url http://www.csm.ornl.gov/srt/downloads/oscar/maui-3.2.6p19.tar.gz --package-name maui-oscar --verbose

clean:
