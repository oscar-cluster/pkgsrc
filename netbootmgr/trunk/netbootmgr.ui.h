/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/




void netBootMgr::init()
{
    print "Starting netbootmgr...\n";
    readNBAConfig();
    buildHostDB();
    readHostAction();
    fillHostTab();
    processTimer = Qt::Timer(this, "processTimer");
    Qt::Object::connect(processTimer, SIGNAL 'timeout()', this, SLOT 'refresh()');
    # auto-refresh
    processTimer->start($netbootlib::config{refresh} * 1000, 0);
    if (!hasCPower()) {
	powerTextLabe->setHidden(1);
	powerComboBox->setHidden(1);
    }
}

void netBootMgr::readNBAConfig()
{
    this->actionComboBox->clear();
    netbootlib::readConfig();

    # add labels to action menu
    my %nba = %{$netbootlib::config{nba}};
    for my $label (sort keys(%nba)) {
	next if $label eq "default_action";
	this->actionComboBox->insertItem( trUtf8($label) );
    }
}

void netBootMgr::selectAll()
{
    tabqListView->selectAll(1);
}

void netBootMgr::clearSelected()
{
    tabqListView->selectAll(1);
    tabqListView->invertSelection();
}

void netBootMgr::setAction()
{
    my $action = actionComboBox->currentText();
    my @sel = @{listSelected()};
    if (@sel) {
	netbootlib::setAction($action, \@sel);

	for my $h (@sel) {
	    # update listview with nba of host
	    my $qli = tabqListView->findItem($h, 0, Qt::ExactMatch());
	    if ($qli) {
		$qli->setText(3, $netbootlib::hosts{$h}->{nba});
	    }
	}
    }
}

void netBootMgr::buildHostDB()
{
    netbootlib::getHostDB();
}

void netBootMgr::fillHostTab()
{
    tabqListView->clear();
    my $item = undef;
    for my $h (sort keys(%netbootlib::hosts)) {
	$item = Qt::ListViewItem(tabqListView, $item);
	$item->setText(0, trUtf8($h));
	$item->setText(1, trUtf8($netbootlib::hosts{$h}->{arch}));
	$item->setText(2, trUtf8($netbootlib::hosts{$h}->{ip}));
	$item->setText(3, trUtf8($netbootlib::hosts{$h}->{nba}));
	$item->setText(4, trUtf8($netbootlib::hosts{$h}->{stat}));
	$item->setText(5, trUtf8($netbootlib::hosts{$h}->{power}));
    }
}

void netBootMgr::updateHostTab()
{
    my (@list) = @_;
    for my $h (@list) {
	my $qli = tabqListView->findItem($h, 0, Qt::ExactMatch());
	if ($qli) {
	    $qli->setText(3, trUtf8($netbootlib::hosts{$h}->{nba}));
	    $qli->setText(4, trUtf8($netbootlib::hosts{$h}->{stat}));
	    $qli->setText(5, trUtf8($netbootlib::hosts{$h}->{power}));
	}
    }
}

void netBootMgr::readHostAction()
{
    return netbootlib::read_host_action();
}

void netBootMgr::refresh()
{
    #print "refresh\n";
    my @changed = netbootlib::read_host_action();
    my @statuschanged = netbootlib::read_host_status();
    my %nodes;
    map { $nodes{$_}=1; } @changed;
    map { $nodes{$_}=1; } @statuschanged;
    updateHostTab(keys %nodes);
}


void netBootMgr::powerAction()
{
    our $power_action = powerComboBox->currentText();
    return if ($power_action eq "no action");
    our @power_sel = @{listSelected()};
    return if (!@power_sel);
    our $sure_text = "Are you sure you want to $power_action the selected nodes?";
    my $sd = sureDialog( this,  "sd");   
}


void netBootMgr::applyPower()
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


QString * netBootMgr::listSelected()
{
    # find selected hosts
    my @sel;
    my $qlc = tabqListView->firstChild();
    while ($qlc) {
	if ($qlc->isSelected()) {
	    push @sel, $qlc->text(0);
	}
	$qlc = $qlc->nextSibling();
    }
    return \@sel;
}


QInt netBootMgr::hasCPower()
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
