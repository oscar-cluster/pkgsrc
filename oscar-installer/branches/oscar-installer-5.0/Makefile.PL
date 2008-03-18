use ExtUtils::MakeMaker;

my $version = `cat VERSION`;

sub MY::postamble {
    my $string = <<EOF;

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
	'NAME' => 'oscar-installer',
	'INSTALLBIN' => "/usr/bin",
#	'INSTALLSITELIB' => "/usr/lib/oscar",
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
	'realclean' => {FILES => "*.old *.bak *~ *.tar.gz Makefile MANIFEST"},
);
