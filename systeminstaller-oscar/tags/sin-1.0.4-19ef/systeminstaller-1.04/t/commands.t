use Test;
use Carp;

BEGIN {
	plan tests => 2;
}
$PERLC = "perl -cw -Iblib/lib";

# Check commands syntax 
ok(system("$PERLC blib/script/mksidisk > /dev/null 2>&1"),0);
ok(system("$PERLC blib/script/buildimage > /dev/null 2>&1"),0);

