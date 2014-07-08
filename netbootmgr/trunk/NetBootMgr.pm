package NetBootMgr;

use strict;
use warnings;
use QtCore4;
use QtGui4;
use QtCore4::isa qw( Qt::Dialog );

use Ui_NetBootMgr;
use SureDialog;
use NetBootMgr; # our sure_text;
use netbootlib;

# Our slots.
use QtCore4::slots
    readNBAConfig => [],
    selectAll => [],
    clearSelected => [],
    setAction => [],
    buildHostDB => [],
    fillHostTab => [],
    updateHostTab => [],
    readHostAction => [],
    refresh => [],
    powerAction => [],
    applyPower => [];


sub NEW {
    my ( $class, $parent ) = @_;
    $class->SUPER::NEW($parent);
    this->{ui} = Ui_NetBootMgr->setupUi(this);

    init();


}

sub ui() {
    return this->{ui};
}

sub init
{

    print "Starting netbootmgr...\n";
    readNBAConfig();
    buildHostDB();
    readHostAction();
    fillHostTab();
    #my $processTimer = Qt::Timer(this, "processTimer");
    my $processTimer = Qt::Timer(this);
    Qt::Object::connect($processTimer, SIGNAL 'timeout()', this, SLOT 'refresh()');
    # auto-refresh
    $processTimer->start($netbootlib::config{refresh} * 1000);
    if (!hasCPower()) {
        this->ui()->powerTextLabe()->setHidden(1);
        this->ui()->powerComboBox()->setHidden(1);
    }

}

sub listSelected
{

    # find selected hosts
    my @sel;
    my $qlc = this->tabqListView->firstChild();
    while ($qlc) {
        if ($qlc->isSelected()) {
            push @sel, $qlc->text(0);
        }
        $qlc = $qlc->nextSibling();
    }
    return \@sel;

}

sub hasCPower
{

    # check if CPower is available and configured
    my $cpower = `which cpower 2>/dev/null`;
    return 0 if (!$cpower);
    chomp $cpower;

    my %stat;
    # now check whether status returns anything meaningfull
    local *P;
    my $cmd = "$cpower --status ".join(" ", sort keys %netbootlib::hosts);
    $cmd .= " 2>/dev/null";
    open P, "$cmd |" or do {
        print "Could not run $cmd: $!\n";
        return 0;
    };
    while (<P>) {
        chomp;
        if (/^(\S+) : (.*)$/) {
            my $h = $1;
            my $v = $2;
            if ($v) {
                if ($v !~ /(timeout|unknown)/) {
                    $stat{$h} = $v;
                }
            }
        }
    }
    close P;
    if (scalar(keys(%stat))) {
        return 1;
    }
    return 0;

}

sub readNBAConfig
{

    this->ui()->actionComboBox()->clear();
    netbootlib::readConfig();

    # add labels to action menu
    my %nba = %{$netbootlib::config{nba}};
    for my $label (sort keys(%nba)) {
        next if $label eq "default_action";
        this->ui()->actionComboBox()->insertItem( 0, trUtf8($label) ); # index = 0: prepend
    }

}

sub selectAll
{

    this->ui()->tabqListView()->selectAll(1);

}

sub clearSelected
{

    this->ui()->tabqListView()->selectAll(1);
    this->ui()->tabqListView()->invertSelection();

}

sub buildHostDB
{

    netbootlib::getHostDB();

}

sub fillHostTab
{

    this->ui()->tabqListView()->clear();
    my $item = undef;
    for my $h (sort keys(%netbootlib::hosts)) {
        $item = Qt::ListViewItem(this->ui()->tabqListView(), $item);
        $item->setText(0, trUtf8($h));
        $item->setText(1, trUtf8($netbootlib::hosts{$h}->{arch}));
        $item->setText(2, trUtf8($netbootlib::hosts{$h}->{ip}));
        $item->setText(3, trUtf8($netbootlib::hosts{$h}->{nba}));
        $item->setText(4, trUtf8($netbootlib::hosts{$h}->{stat}));
        $item->setText(5, trUtf8($netbootlib::hosts{$h}->{power}));
    }

}

sub updateHostTab
{

    my (@list) = @_;
    for my $h (@list) {
        my $qli = this->ui()->tabqListView()->findItem($h, 0, Qt::ExactMatch());
        if ($qli) {
            $qli->setText(3, trUtf8($netbootlib::hosts{$h}->{nba}));
            $qli->setText(4, trUtf8($netbootlib::hosts{$h}->{stat}));
            $qli->setText(5, trUtf8($netbootlib::hosts{$h}->{power}));
        }
    }

}

sub readHostAction
{

    return netbootlib::read_host_action();

}

sub refresh
{

    #print "refresh\n";
    my @changed = netbootlib::read_host_action();
    my @statuschanged = netbootlib::read_host_status();
    my %nodes;
    map { $nodes{$_}=1; } @changed;
    map { $nodes{$_}=1; } @statuschanged;
    updateHostTab(keys %nodes);

}

sub powerAction
{

    our $power_action = this->ui()->powerComboBox()->currentText();
    return if ($power_action eq "no action");
    our @power_sel = @{listSelected()};
    return if (!@power_sel);
    our $sure_text = "Are you sure you want to $power_action the selected nodes?";
    my $sd = SureDialog( this,  "sd");
    this->ui()->{sd} = $sd; # Keep track of that.

}

sub applyPower
{

    our ($power_action, @power_sel);
    print "applyPower : $power_action ".join(" ", @power_sel)."\n";
    my $pa = lc($power_action);
    $pa =~ s:\s+::g;
    my $cmd = "cpower";
    if ($pa eq "status") {
        $cmd .= " --status";
    } elsif ($pa eq "poweron") {
        $cmd .= " --on";
    } elsif ($pa eq "poweroff") {
        $cmd .= " --off";
    } elsif ($pa eq "idledon") {
        $cmd .= " --idon";
    } elsif ($pa eq "idledoff") {
        $cmd .= " --idoff";
    }
    return if ($cmd eq "cpower");
    my @c = netbootlib::apply_cpower_cmd($cmd, @power_sel);
    updateHostTab(@c);

}

1;
