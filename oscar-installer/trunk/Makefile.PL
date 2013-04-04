use ExtUtils::MakeMaker;

chomp (my $version = `cat VERSION`);

sub MY::postamble {
    my $string = <<EOF;

PKGDEST=

deb :: manifest rpm_prep dist
\tmkdir -p /tmp/scdeb
\tmv \$(NAME)-$version.tar.gz /tmp/scdeb \\
\t\t&& cd /tmp/scdeb && tar -xvzf \$(NAME)-$version.tar.gz
\t\@if [ -n "\$\$UNSIGNED_OSCAR_PKG" ]; then \\
\t\techo "dpkg-buildpackage -rfakeroot -us -uc"; \\
\t\tcd /tmp/scdeb/\$(NAME)-$version && dpkg-buildpackage -rfakeroot -us -uc; \\
\telse \\
\t\techo "dpkg-buildpackage -rfakeroot";\\
\t\tcd /tmp/scdeb/\$(NAME)-$version && dpkg-buildpackage -rfakeroot; \\
\tfi
\t\@if [ -n "\$(PKGDEST)" ]; then \\
\t\tmv /tmp/scdeb/\$(NAME)*.deb \$(PKGDEST); \\
\t\techo "Debian packages are available in \$(PKGDEST)"; \\
\telse \\
\t\techo "Debian packages are available in /tmp/scdeb"; \\
\tfi
\trm -rf /tmp/scdeb/\$(NAME)-$version

rpm_prep ::
\tsed -e 's/__VERSION__/$version/g' oscar-installer.spec.in > oscar-installer.spec

rpm :: rpm_prep manifest dist
\trpmbuild -tb --target noarch \$(NAME)-\$(VERSION).tar.gz
\t\@if [ -n "\$(PKGDEST)" ]; then \\
\t\tmv \$(shell rpm --eval '%{_rpmdir}')/noarch/\$(NAME)-*.noarch.rpm \$(PKGDEST); \\
\tfi

srpm :: rpm_prep manifest dist
\trpmbuild -ts --target noarch --nodeps \$(NAME)-\$(VERSION).tar.gz

ext_install :: conf

\$(DESTDIR)\$(PREFIX)/../etc/oscar-installer/oscar-installer.conf : conf/oscar-installer.conf \$(DESTDIR)\$(PREFIX)/../etc/oscar-installer
\tinstall conf/oscar-installer.conf \$(DESTDIR)\$(PREFIX)/../etc/oscar-installer/

\$(DESTDIR)\$(PREFIX)/../etc/oscar-installer :
\tinstall -d \$(DESTDIR)\$(PREFIX)/../etc/oscar-installer

conf: \$(DESTDIR)\$(PREFIX)/../etc/oscar-installer/oscar-installer.conf

manifest :
\t if [[ -f MANIFEST ]]; then \\
\t\t mv MANIFEST MANIFEST.bak; \\
\t fi
\t cp MANIFEST.stub MANIFEST

install :: all pure_install ext_install

EOF
}

WriteMakefile(
	'VERSION' => $version,
	'NAME' => "oscar-installer",
	'EXE_FILES' => [qw(
		oscar-installer
	)],
	'MAN1PODS' => {
		'oscar-installer' => '$(INST_MAN1DIR)/oscar-installer.$(MAN1EXT)',
	},
	'macro' => {
		'MAN1EXT' => '1',
		'MAN3EXT' => '3pm',
	},
	'PREREQ_PM' => {
		AppConfig => '1.52',
	},
	'realclean' => {FILES => "*.old *.bak *~ *.tar.gz oscar-installer.spec Makefile MANIFEST"},
);
