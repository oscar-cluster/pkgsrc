###################################################################
# Copyright (c) 2007 INRIA-IRISA,
#                    Jean Parpaillon <jean.parpaillon@irisa.fr>
#                    All rights reserved
# For license information, see the COPYING file in the top level
# directory of the source
###################################################################

__all__ = ['Config']

class Config:
    __instance = None

    __datas = {"templateDir":"../templates",
               "xsdPath":"../doc/opkg.xsd"}

    def __new__ (cls):
        if cls.__instance is None:
            cls._instance = object.__new__(cls)
        return cls._instance
    
    def getValue (self, var):
        return self.__datas[var]
