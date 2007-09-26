#
# Copyright 2007 IRISA-INRIA
#                Jean Parpaillon <jean.parpaillon@irisa.fr>
#
# Config reader for oreposd
#
import ConfigParser
import os,sys
from Logger import *

class Config(object):
    __required_configs__ = {"DEFAULT": ["basedir",
                                        "pidfile"],
                            "incoming": ["polltime",
                                          "localdir",
                                          "distdir",
                                          "disthost"]}

    __instance__ = None
    __config__ = None

    def __new__ (cls):
        if cls.__instance__ is None:
            cls.__instance__ = object.__new__(cls)
            cls.__instance__.readConfig()
        return cls.__instance__

    #
    # Read config file ./oreposd.conf
    #
    def readConfig(self):
        self.__config__ = ConfigParser.ConfigParser()

        config_files = [ os.path.join(os.path.dirname(sys.argv[0]), "oreposd.conf") ]
        fd = None
        for config_file in config_files:
            try:
                fd = open(config_file)
            except IOError, e:
                continue
            try:
                self.__config__.readfp(fd)
                Logger().debug("Read config in %s" % config_file)
            except ConfigParser.ParsingError, e:
                Logger().error("Error parsing config file:\n%s" % str(e))
                raise SystemExit(1)
            fd.close()

        if fd == None:
            error("No config file")
            raise SystemExit(1)

        # Just to test mandatory options (raise NoOptionError if not found)
        for section in self.__required_configs__.keys():
            for opt in self.__required_configs__[section]:
                self.__config__.get(section, opt)

    def get(self, section, opt):
        return self.__config__.get(section, opt)
    
    def set(self, section, opt, val):
        return self.__config__.set(section, opt, val)
    
    def isDefined(self, section, opt):
        return self.__config__.has_option(section, opt)

    def getSections(self, type=None):
        if type:
            return [ section
                     for section in self.__config__.sections()
                     if self.__config__.has_option(section, "type") and self.__config__.get(section, "type") == type ]
        else:
             return self.__config__.sections()

    def getUser(self):
        return os.environ['USER']
