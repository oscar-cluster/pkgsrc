###################################################################
# Copyright (c) 2007 INRIA-IRISA,
#                    Jean Parpaillon <jean.parpaillon@irisa.fr>
#                    All rights reserved
# For license information, see the COPYING file in the top level
# directory of the source
###################################################################

import subprocess
from Logger import *

def command(command, cwd=os.getcwd()):
    Logger().debug("Execute: %s" % command)
    exe = subprocess.Popen(command,
                           cwd=cwd,
                           bufsize=0,
                           stdin=None,
                           stdout=subprocess.PIPE,
                           stderr=subprocess.PIPE,
                           shell=True)
    if Logger().isDebug():
        for l in exe.stdout:
            Logger().info(l.strip())
    for l in exe.stderr:
        Logger().error(l.strip())
    return exe.wait()

