###################################################################
# Copyright (c) 2007 INRIA-IRISA,
#                    Jean Parpaillon <jean.parpaillon@irisa.fr>
#                    All rights reserved
# For license information, see the COPYING file in the top level
# directory of the source
###################################################################

import threading, Queue
from Config import *
from Tools import *

class Incoming(threading.Thread):
    __stopEvent__ = threading.Event()
    __queue__ = Queue.Queue()

    def __init__(self):
        threading.Thread.__init__(self, name="Incoming")
        self.__stopEvent__.clear()
        self.setDaemon(False)

    def run(self):
        polltime = int(Config().get("GENERAL", "incoming_polltime"))
        while not self.__stopEvent__.isSet():
            self.syncIncoming()
            self.__stopEvent__.wait(polltime)
        
        # Exit
        Logger().info("%s exit..." % self.getName())
        raise SystemExit(0)

    def syncIncoming(self):
        Logger().info("Pull incoming packages")
        local_dir = "%s/" % Config().get("GENERAL", "local_incoming_dir")
        dist_dir = "%s@%s:%s/" % (Config().get("GENERAL", "dist_incoming_user"),
                                  Config().get("GENERAL", "dist_incoming_host"),
                                  Config().get("GENERAL", "dist_incoming_dir"))
        cmd = "rsync -av %s %s" % (dist_dir, local_dir)
        command(cmd)

    def stop(self):
        self.__stopEvent__.set()

