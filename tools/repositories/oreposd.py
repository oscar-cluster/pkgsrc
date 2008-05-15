#!/usr/bin/env python
# coding=utf8

#
# oreposd: manage OSCAR repositories, fetching packages in incoming queue,
# launching package build and updating repositories
#
# Configuration is done through oreposd.conf file
#
# Copyright (C) 2007 INRIA-IRISA
#                    Jean Parpaillon <jean.parpaillon@irisa.fr>
#

import os, sys, getopt
import string, re
import signal
import logging

from Oreposd.Config import *
from Oreposd.Logger import *
from Oreposd.Incoming import *
from Oreposd.Queues import *
from Oreposd.Db import *

VERSION = '0.1'

USAGE = """Usage: oreposd [ -h ] [ -v ] [ -d ] [ -k ]
       -h|--help      print this and exit
       -v|--version   print version and exit
       -d|--debug     add verbosity
       -k|--kill      kill an running oreposd process
       -b|--batch     don't daemonize
"""

stderrLogger = None
fileLogger = None

threads = []

def alreadyRunning():
    """ Return pid of an already running process of oreposd, if any.
    """
    try:
        fd = open(Config().get("DEFAULT", "pidfile"), 'r')
        pid = int(fd.readline().strip())
        return pid
    except IOError, e:
        return None

def savePid():
    fd = open(Config().get("DEFAULT", "pidfile"), 'w')
    fd.write("%d" % os.getpid())
    fd.close()

def rmPid():
    Logger().debug("Removing pid file")
    os.unlink(Config().get("DEFAULT", "pidfile"))

def stop(signum, frame):
    for t in threads:
        Logger().debug("Asking %s to stop" % t.getName())
        t.stop()
    for t in threads:
        t.join()

    Db().stop()

    rmPid()

    Logger().info("oreposd main thread exit...")
    Logger().shutdown()
    raise SystemExit(0)

def kill(pid):
    os.kill(pid, signal.SIGTERM)
    
#
# Main
#
def main():
    # Parse command line options
    try:
        (opts, args) = getopt.getopt(sys.argv[1:],
                                     'vhdkbl:',
                                     ['version', 'help', 'debug', 'kill', 'batch', 'log='])
    except getopt.error, msg:
        error(msg)
        SystemExit(1)

    killMode = False
    batchMode = False
    logLevel = logging.INFO
    for option, arg in opts:
        if option in ('-h', '--help'):
            print USAGE
            return
        elif option in ('-v', '--version'):
            print VERSION
            return
        elif option in ('-d', '--debug'):
            logLevel = logging.DEBUG
        elif option in ('-k', '--kill'):
            killMode = True
        elif option in ('-b', '--batch'):
            batchMode = True

    # Logger initialization
    stderrLogger = logging.StreamHandler(strm=sys.stderr)
    stderrLogger.setLevel(logLevel)
    stderrLogger.setFormatter(logging.Formatter(fmt="%(name)s [%(thread)d] %(levelname)s: %(message)s"))
    Logger().init(logLevel, [stderrLogger])

    # Config initialization
    try:
        Config()
    except ConfigParser.NoOptionError, e:
        Logger().error(e)
        raise SystemExit(1)

    # Create basedir
    basedir = Config().get("DEFAULT", "basedir")
    if not os.path.isdir(basedir):
        Logger().info("Creates base dir: %s" % basedir)
        os.makedirs(basedir)

    # Initialize database
    Db().init()

    # Once config init'd, can set log file handler
    logdir = os.path.join(basedir, "logs")
    if not os.path.isdir(logdir):
        Logger().info("Creates log dir: %s" % logdir)
        os.makedirs(logdir)
    
    logfileName = os.path.join(logdir, "oreposd.log")
    if not killMode:
        fileLogger = logging.FileHandler(logfileName)
        fileLogger.setLevel(logLevel)
        fileLogger.setFormatter(logging.Formatter(fmt="%(asctime)s %(name)s [%(thread)d] %(levelname)s: %(message)s", datefmt="%b %d %H:%M:%S"))
        Logger().addHandler(fileLogger)

    # Deal with kill mode 
    pid = alreadyRunning()
    if killMode:
        if pid:
            Logger().info("Terminate oreposd (pid %d)" % pid)
            kill(pid)
            raise SystemExit(0)
        else:
            Logger().error("No oreposd process already running")
            raise SystemExit(1)
    else:
        if pid:
            Logger().error("An oreposd process is already running (%s exists)" % Config().get("DEFAULT", "pidfile"))
            raise SystemExit(1)

    # Daemonize if not batch mode
    if not batchMode:
        Logger().debug("Daemonizing...")
        if os.fork() == 0:
            os.setsid()
            if os.fork() != 0:
                raise SystemExit(0)
        else:
            raise SystemExit(0)
        Logger().debug("Finished daemonizing (pid %s)" % (os.getpid(),))
        Logger().rmHandler(stderrLogger)
    savePid()

    # QueueManager initialization
    QueueManager().init()

    # Start threads
    try:
        
        incomingThread = Incoming()
        incomingThread.start()
        
        threads.append(incomingThread)
    except SystemExit, e:
        kill(alreadyRunning())

    # Task of main thread: waiting for stop signal
    signal.signal(signal.SIGTERM, stop)
    signal.signal(signal.SIGINT, stop)

    while True:
        signal.pause()

# Main
if __name__ == '__main__':
    main()

# vim: set expandtab shiftwidth=4 :
