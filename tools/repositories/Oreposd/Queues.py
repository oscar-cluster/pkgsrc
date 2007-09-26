###################################################################
# Copyright (c) 2007 INRIA-IRISA,
#                    Jean Parpaillon <jean.parpaillon@irisa.fr>
#                    All rights reserved
# For license information, see the COPYING file in the top level
# directory of the source
###################################################################

import Queue
import re
from Config import *
from Tools import *

class QueueManager(object):
    __queues__ = {}

    __instance = None
    
    def __new__ (cls):
        if cls.__instance is None:
            cls.__instance = object.__new__(cls)
        return cls.__instance

    def init(self):
        for t in ["source", "build"]:
            qList = []
            for sec in Config().getSections(type=t):
                maxsize = 0
                if Config().isDefined(sec, "maxsize"):
                    maxsize = int(Config().get(sec, "maxsize"))
                q = QueueFactory().createQueue(sec,
                                             Config().get(sec, "type"),
                                             maxsize)
                qList.append(q)
            self.__queues__[t] = qList

    def getQueueList(self, type):
        return self.__queues__[type]

class OQueue(Queue.Queue):
    __name__ = None
    __type__ = None

    def __init__(self, name, type, maxsize=0):
        Queue.Queue.__init__(self, maxsize)
        self.__name__ = name
        self.__type__ = type

    def getName(self):
        return self.__name__

    def getType(self):
        return self.__type__

class SourceQueue(OQueue):
    
    def accept(self, sourcename):
        extRe = glob2pat("*%s" % Config().get(self.getName(), "ext"))
        return re.match(extRe, sourcename)

class QueueFactory(object):
    __instance = None

    def __new__ (cls):
        if cls.__instance is None:
            cls.__instance = object.__new__(cls)
        return cls.__instance

    def createQueue(self, name, type, maxsize):
        if type == "source":
            return SourceQueue(name, type, maxsize)
        else:
            return OQueue(name, type, maxsize)
        
