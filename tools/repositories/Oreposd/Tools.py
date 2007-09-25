###################################################################
# Copyright (c) 2007 INRIA-IRISA,
#                    Jean Parpaillon <jean.parpaillon@irisa.fr>
#                    All rights reserved
# For license information, see the COPYING file in the top level
# directory of the source
###################################################################

import subprocess, os, logging
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
    stdoutLogger = PipeLogger(exe.stdout, logging.DEBUG)
    stdoutLogger.start()
    stderrLogger = PipeLogger(exe.stderr, logging.WARN)
    stderrLogger.start()
    
    ret = exe.wait()
    stdoutLogger.stop()
    stderrLogger.stop()
    return ret
