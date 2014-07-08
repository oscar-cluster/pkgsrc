package SureDialog;

use strict;
use warnings;
use QtCore4;
use QtGui4;
use QtCore4::isa qw( Qt::Dialog );

use Ui_SureDialog;
use NetBootMgr;

sub NEW {
    my ( $class, $parent ) = @_;
    $class->SUPER::NEW($parent); # Create a Qt::Dialog
    this->{ui} = Ui_SureDialog->setupUi(this); # Fill the above.

    init();

}

sub init
{
    my $parent = Qt::Object::parent(this);
    my $a = $NetBootMgr::sure_text;
    this->textLabel1()->setText($a);
    Qt::Object::connect(this->sureYesButton(), SIGNAL 'clicked()', $parent, SLOT 'applyPower()');
    this->show();
}

1;
