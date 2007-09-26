###################################################################
# Copyright (c) 2007 INRIA-IRISA,
#                    Jean Parpaillon <jean.parpaillon@irisa.fr>
#                    All rights reserved
# For license information, see the COPYING file in the top level
# directory of the source
###################################################################

import os
import bsddb
from Oreposd.Config import *
from Oreposd.Logger import *

class Db(object):
    """ Db
    """
    __instance = None

    __tables__ = {"pkg_status": None}
    
    def __new__ (cls):
        if cls.__instance is None:
            cls.__instance = object.__new__(cls)
        return cls.__instance

    def init(self):
        dbdir = os.path.join(Config().get("DEFAULT", "basedir"), "db")
        if not os.path.isdir(dbdir):
            Logger().info("Create db dir: %s" % dbdir)
            os.makedirs(dbdir)
        for t in self.__tables__.keys():
            bt = bsddb.btopen(os.path.join(dbdir, "%s.db" % t), 'c')
            self.__tables__[t] = bt
            
    def stop(self):
        Logger().debug("Close db files")
        for t in self.__tables__.keys():
            self.__tables__[t].close()
        
