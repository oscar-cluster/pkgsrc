###################################################################
# Copyright (c) 2007 INRIA-IRISA,
#                    Jean Parpaillon <jean.parpaillon@irisa.fr>
#                    All rights reserved
# For license information, see the COPYING file in the top level
# directory of the source
###################################################################

import os
import threading, select, fcntl
import logging, logging.handlers

class Logger(object):
    """ Logger 
    """
    __instance = None
    __logger = None
    __level = None
    
    def __new__ (cls):
        if cls.__instance is None:
            cls.__instance = object.__new__(cls)
        return cls.__instance

    def init(self, lvl, handlers):
        logging.raiseExceptions = 0
        self.__logger = logging.getLogger("oreposd")
        self.__level = lvl
        self.__logger.setLevel(lvl)
        for handler in handlers:
            self.__logger.addHandler(handler)

    def addHandler(self, handler):
        self.__logger.addHandler(handler)

    def rmHandler(self, handler):
        self.__logger.removeHandler(handler)

    def shutdown(self):
        logging.shutdown()

    def isError(self):
        return self.__level >= logging.ERROR
        
    def error(self, msg):
        self.__logger.error(msg)

    def isInfo(self):
        return self.__level >= logging.INFO
        
    def info(self, msg):
        self.__logger.info(msg)

    def isDebug(self):
        return self.__level >= logging.DEBUG
        
    def debug(self, msg):
        self.__logger.debug(msg)

    def log(self, level, msg):
        self.__logger.log(level, msg)

class PipeLogger(threading.Thread):
    stopEvent = threading.Event()
    fd = None
    level = None

    def __init__(self, fd, level):
        threading.Thread.__init__(self)
        self.fd = fd
        self.level = level
        self.stopEvent.clear()
    
    def run(self):
        s = ""
        flags = fcntl.fcntl(self.fd, fcntl.F_GETFL)
        fcntl.fcntl(self.fd, fcntl.F_SETFL, flags | os.O_NONBLOCK)
        while not self.stopEvent.isSet():
            [i, o, e] = select.select([self.fd], [], [], .2)
            if i:
                s = self.fd.read(1000)
            else:
                s = ""

            if s:
                Logger().log(self.level, s.strip())
            
    def stop(self):
        self.stopEvent.set()
