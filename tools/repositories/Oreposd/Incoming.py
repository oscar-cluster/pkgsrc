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
    __timer = None

    def __init__(self):
        threading.Thread.__init__(self, name="Incoming")
        self.__stopEvent__.clear()
        self.setDaemon(False)

    def run(self):
        self.syncIncoming()
        while True:
            if self.__stopEvent__.isSet():
                break
            
            self.__timer = threading.Timer(int(Config().get("GENERAL", "incoming_polltime")), self.syncIncoming)
            self.__timer.start()
            self.__timer.join()
        
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
        if self.__timer:
            self.__timer.cancel()
        self.__stopEvent__.set()

