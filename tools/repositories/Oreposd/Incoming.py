###################################################################
# Copyright (c) 2007 INRIA-IRISA,
#                    Jean Parpaillon <jean.parpaillon@irisa.fr>
#                    All rights reserved
# For license information, see the COPYING file in the top level
# directory of the source
###################################################################

import threading
from Config import *
from Tools import *
from Queues import *
from Packages import *
from Db import *

class Incoming(threading.Thread):
    __stopEvent__ = threading.Event()
    __localIncomingDir__ = None

    def __init__(self):
        threading.Thread.__init__(self, name="Incoming")
        self.__stopEvent__.clear()
        self.setDaemon(False)
        self.__localIncomingDir__ = Config().get("incoming", "localdir")

        if not os.path.isdir(self.__localIncomingDir__):
            cmd = "install -m 775 -d %s" % self.__localIncomingDir__
            Logger().info("Creating directory %s" % self.__localIncomingDir__)
            ret = command(cmd)
            if ret != 0:
                Logger().error("Can not create directory %s (return code: %d)" % (self.__localIncomingDir__, ret))
                raise SystemExit(0)

    def run(self):
        polltime = int(Config().get("incoming", "polltime"))
        while not self.__stopEvent__.isSet():
            self.syncIncoming()
            self.fillQueues()
            self.__stopEvent__.wait(polltime)
        
        # Exit
        Logger().info("%s exit..." % self.getName())
        raise SystemExit(0)

    def syncIncoming(self):
        Logger().info("Pull incoming packages")
        user = ""
        if Config().isDefined("incoming", "distuser"):
            user = Config().get("incoming", "distuser")
        else:
            user = Config().getUser()
            
        disthost = Config().get("incoming", "disthost")
        distdir = Config().get("incoming", "distdir")
        dist_dir = "%s@%s:%s/" % (user, disthost, distdir)
        cmd = "rsync -av %s %s" % (dist_dir, "%s/" % self.__localIncomingDir__)
        command(cmd)

    def fillQueues(self):
        Logger().info("Getting list of incoming packages")
        qList = QueueManager().getQueues("source")
        for basename in os.listdir(self.__localIncomingDir__):
            for q in qList:
                if q.accept(basename):
                    # TODO: handle full queue (if maxsize have been set)
                    if not Db().isIn(Db.T_PKG_STATUS, basename):
                        Db().set(Db.T_PKG_STATUS, basename, "%d" % Package.ST_NEW)
                        q.put(basename)
                        Logger().info("Added %s in %s source queue" % (basename, q.getName()))
                    
    def stop(self):
        self.__stopEvent__.set()
