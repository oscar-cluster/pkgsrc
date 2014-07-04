# 
all: netbootmgr netBootMgr.pm sureDialog.pm

netBootMgr.pm: netbootmgr.ui.h
	puic -o netBootMgr.pm netbootmgr.ui

sureDialog.pm: suredialog.ui.h
	puic -o sureDialog.pm suredialog.ui

clean:
	rm -f sureDialog.pm netBootMgr.pm
