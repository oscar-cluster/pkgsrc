#!/usr/bin/perl
#########################################################################
#  Script Name  : kernel_picker                                         #
#  Programmed by: Terry Fleury (tfleury@ncsa.uiuc.edu)                  #
#  Date         : June 3, 2002                                          #
#  Purpose      : This Perl script is a hack to allow a user to choose  #
#                 an Oscar image to install.  Type "imagepicker -h"     #
#                 for more information.                                 #
#  Modifications:                                                       #
#  Sep 03, 2003 : Added --autoselectimage option - TGF                  #
#                 Fixed '/' bug in getOscarImageName - TGF              #
#########################################################################
my $versionstring = '$Id: kernel_picker.pl,v 1.5 2003/11/04 22:16:37 tfleury Exp $'; 

use lib "/usr/lib/systemconfig";
use Initrd::Generic;   # Used by kernel_version()
use Cwd;               # Used by cwd()
use Getopt::Long;      # Used to get command line options
use Carp;              # Used by croak() and carp()

# The absolute path of the directory holding oscarimage directories
my $systemimagesdir = "/var/lib/systemimager/images";
my $commandlineoptions = 0; # Was anything specified as command line options?

#########################################################################
#  Subroutine name : parseCommandLine                                   #
#  Parameters : none                                                    #
#  Returns    : nothing                                                 #
#  This function scans the command line for options and stores their    #
#  values in the global variables $opt_OPTIONNAME where OPTIONNAME is   #
#  the name of the command line option.                                 #
#########################################################################
sub parseCommandLine # 
{
  $Getopt::Long::autoabbrev = 1;        # Allow abbreviated options
  $Getopt::Long::getopt_compat = 1;     # Allow + for options
  $Getopt::Long::order = $PERMUTE;      # Option reordering
  &GetOptions("oscarimage:s",
              "autoselectimage",
              "bootkernel=s",
              "bootlabel=s",
              "bootramdisk:s",
              "networkboot:s",
              "kernelversion=s",
              "modulespath=s",
              "systemmap:s",
              "version",
              "help"
              );
  $opt_oscarimage = 'oscarimage' if
    (defined($opt_oscarimage) && $opt_oscarimage eq "");
  if (defined($opt_version))
    {
      $versionstring =~ /\$Id: (.*)\d\d:\d\d:\d\d/ and
      print "$1\n";
      exit;
    }
  printHelpMessage() if defined($opt_help);
  # If any of the command line options were entered, set global variable
  $commandlineoptions =              defined($opt_oscarimage)    || 
    defined($opt_autoselectimage) || defined($opt_bootkernel)    ||
    defined($opt_bootlabel)       || defined($opt_bootramdisk)   || 
    defined($opt_networkboot)     || defined($opt_kernelversion) ||
    defined($opt_modulespath)     || defined($opt_systemmap);
}

#########################################################################
#  Subroutine name : printHelpMessage                                   #
#  Parameters : none                                                    #
#  Returns    : nothing                                                 #
#  This function prints out an extensive help message on how to use     #
#  this script.                                                         #
#########################################################################
sub printHelpMessage 
{
  print <<HELP;
SYNOPSIS
   kernel_picker [--oscarimage <IMAGE_NAME>] [--bootkernel BOOT_KERNEL]
   [--bootlabel KERNEL_LABEL] [--bootramdisk {Y/n}] [--networkboot {Y/n}]
   [--kernelversion VERSION_NUM] [--modulespath MODULES_PATH]
   [--systemmap SYSTEM_MAP] [--version] [--help]

DESCRIPTION
   kernel_picker allows you to substitute a given kernel into your
   OSCAR (SIS) image prior to building your nodes.  If executed with
   no command line options, you will be prompted for all required
   information.  You can also specify command line options as shown below.
   Any necessary information that you do not give via an option will cause
   the program to prompt you for that information.

OPTIONS
   --oscarimage <IMAGE_NAME>
       The name of the OSCAR image directory.  If you use this option but
       do not specify IMAGE_NAME, the default directory is 'oscarimage'.
   --autoselectimage 
       In the event that there are two or more OSCAR image directories
       but you do not want to have to specify one via the '--oscarimage'
       option (e.g. when you want to call kernel_picker from another 
       program), use this option.  (The first OSCAR image directory,
       alphabetcially ignoring case, will be used.)  Otherwise, you will 
       be presented with a list of OSCAR image directories and be asked
       to choose one to use.
   --bootkernel BOOT_KERNEL
       The full path specifier of the kernel to use at boot time.
       This is the source kernel file which will be copied to the
       system's boot directory.  Also, systemconfig.conf's PATH
       option will be set appropriately.
   --bootlabel KERNEL_LABEL
       The label for the kernel to use at boot time.  This value is
       set as the LABEL option in the systemconfig.conf file.  If you 
       provide the BOOT_KERNEL but do not provide the KERNEL_LABEL, 
       the KERNEL_LABEL defaults to the file name specified by BOOT_KERNEL
       without any leading path information.
   --bootramdisk {Y/n}
       Whether or not to configure a ram disk for booting.  This
       value is set as the CONFIGRD option in the systemconfig.conf
       file.  If option present without Y or N, defaults to YES.
   --networkboot {Y/n}
       Whether or not to use the specified BOOT_KERNEL for network
       booting the nodes during the build process.  If so, the kernel
       gets copied to /tftpboot.  If option is present without Y or N,
       defaults to YES.
   --kernelversion VERSION_NUM
       If your boot kernel uses loadable modules, you must provide the
       full version number/name of the kernel.  This is the value output
       by the Unix command 'uname -r'.
   --modulespath MODULES_PATH
       If your boot kernel uses loadable modules, you must provide the
       source directory containing the /lib/modules/VERSION_NUM directory
       tree.  Do not prepend 'lib/modules/VERSION_NUM' to this option.
   --systemmap SYSTEM_MAP
       If your boot kernel uses loadable modules, you may optionally
       provide the full path specifier of the System.map file to use by
       depmod.  Enter the full path/file name of the source file.  This
       file will be copied to the boot directory.
   --version
       Prints out the version string of the program and then quits.
   --help
       Print out a help message and then quits.
HELP
  exit;
}

#########################################################################
#  Subroutine name : uniqueFile                                         #
#  Parameter: A string of the file name to be checked                   #
#  Returns  : A string similar to the input string but made "unique"    #
#             by appending a number                                     #
#  This function takes in a string (which should be the name of a file  #
#  to be saved to disk) and makes sure that it is unique.  It does this #
#  by adding a number to the filename.  The new name is returned.       #
#########################################################################
sub uniqueFile # (instr) -> filename
{
  my($instr) = @_;
  my($counter,$filename);

  $filename = $instr;
  while (-e $filename)
    {
      $counter++;
      $filename = $instr . '-' . $counter;
    }
  return $filename;  # Return the 'new' string
} 

#########################################################################
#  Subroutine name : getSystemConfigVars                                #
#  Parameters : none                                                    #
#  Returns : 1. The full path/name of the source kernel to use to boot. #
#            2. The value of the LABEL option to select the boot kernel.#
#            3. Whether or not to configure a ram disk.                 #
#            4. The version of the kernel as read from the kernel.      #
#  This function prompts the user for three values.  The first is the   #
#  the full path/name of the boot kernel.  This kernel will eventually  #
#  be copied over to the /boot directory.  The second is the value of   #
#  the LABEL option in the systemconfig.conf file used to select this   #
#  kernel for booting.  It verifies that the entered kernel file is     #
#  readable (for copying to the /boot directory later) and that the     #
#  LABEL is not empty.  The third is whether or not to configure a      #
#  ram disk which is used to set the CONFIGRD option.  The fourth is    #
#  the version number of the kernel as read from the kernel file itself #
#  if possible.                                                         #
#########################################################################
sub getSystemConfigVars # -> ($kernelfile,$kernellabel,$kernelrd,$kernelvers)
{
  my $kernelfile = "";
  my $kernellabel = "";
  my $kernelrd = "";
  my $kernelvers = "";

  # Check for values set by command line options
  $kernelfile = $opt_bootkernel if 
    ($opt_bootkernel ne "" && -r $opt_bootkernel);
  $kernellabel = $opt_bootlabel if $opt_bootlabel ne "";
  # Default to the bootkernel name if provided and bootlabel is empty
  # Extract the kernel name from the passed-in kernelpath
  $kernelfile =~ m,[^/]+$, && ($kernellabel = $&) if
    ((length($kernellabel) < 1) && (length($kernelfile) > 0));

  if (defined($opt_bootramdisk))
    {
      $kernelrd = "YES";
      $opt_bootramdisk =~ tr/A-Z/a-z/;
      $kernelrd = "NO" if
        ($opt_bootramdisk eq "n") || ($opt_bootramdisk eq "no") || 
        ($opt_bootramdisk eq "none") || ($opt_bootramdisk eq "0");
    }

  # If the user didn't enter some info via options, prompt for the info
  if (($kernelfile eq "") || ($kernellabel eq "") || ($kernelrd eq ""))
    {
      print "For the purposes of booting the kernel, enter\n";
      my $counter = 1;
      if ($kernelfile eq "")
        {
          do
            {
              print "  (".$counter.") The full path/name of the kernel file : ";
              $kernelfile = <STDIN>;
              chomp $kernelfile;
              print "Could not read the file $kernelfile.  Please try again.\n" 
                unless ($kernelfile ne "") && (-r $kernelfile);
            } until ($kernelfile ne "") && (-r $kernelfile);
          $counter++;
        }

      if ($kernellabel eq "")
        {
          $kernelvers = kernel_version($kernelfile);
          $kernelvers = "" if $kernelvers eq "0.0.0";
          do
            {
              print "  (" . $counter . ") The label for selecting this " .
                     "kernel at boot time" .
                     (($kernelvers eq "") ? "" : " [$kernelvers]") .
                     " : ";
              $kernellabel = <STDIN>;
              chomp $kernellabel;
              $kernellabel = $kernelvers if
                $kernellabel eq "" && $kernelvers ne "";
            } until $kernellabel ne "";
          $counter++;
        }

      if ($kernelrd eq "")
        {
          print "  (" . $counter. ") Should we configure a ram disk [Y/n] : ";
          my $ans = <STDIN>;
          chomp($ans);
          $ans =~ tr/A-Z/a-z/;
          $kernelrd = "YES";
          $kernelrd = "NO" if
            ($ans eq "n") || ($ans =~ "^no") || ($ans eq "0");
          $counter++;
        }
    }

  #$kernellabel = substr($kernellabel,-15); # Restrict LABEL to last 15 chars

  return ($kernelfile,$kernellabel,$kernelrd,$kernelvers);
}

#########################################################################
#  Subroutine name : getNetworkBoot                                     #
#  Parameters : The full path/name of the kernel file to be copied.     #
#  Returns : 1. YES or NO for if we should use the specified kernel for #
#               a network build/boot.  We use this value later to copy  #
#               the kernel to the /tftpboot directory.                  #
#            2. If the destination file exists, do we back it up first? #
#########################################################################
sub getNetworkBoot # ($kernelpath) -> ($networkboot,$backupkernel)
{
  my $kernelpath = @_;
  my $networkboot = "";
  my $backupkernel = 0;

  if (defined($opt_networkboot))
    {
      $networkboot = "YES";
      $opt_networkboot =~ tr/A-Z/a-z/;
      $networkboot = "NO" if
        ($opt_networkboot eq "n") || ($opt_networkboot eq "no") || 
        ($opt_networkboot eq "none") || ($opt_networkboot eq "0");
    }

  if ($networkboot eq "")
    {
      print "Should we use this kernel for network building [y/N] : ";
      my $ans = <STDIN>;
      chomp($ans);
      $ans =~ tr/A-Z/a-z/;
      $networkboot = "NO";
      $networkboot = "YES" if
        ($ans eq "y") || ($ans eq "yes") || ($ans eq "1");
    }

  $kernelpath =~ m,[^/]+$, &&
    (my $kernelfile = $&);
  if (($networkboot eq "YES") && (-r "/tftpboot/$kernelfile") && 
      (-f "/tftpboot/$kernelfile"))
    {
      print "The file /tftpboot/$kernelfile exists. What should I do?\n";
      print "   (O)verwrite/(B)ackup/(D)on't copy [O] : ";
      my $ans = <STDIN>;
      chomp($ans);
      $ans =~ tr/A-Z/a-z/;
      if ($ans =~ "^b")   { $backupkernel = 1; }
      elsif ($ans =~ "^d") { $networkboot = "NO"; }
    }

  return ($networkboot,$backupkernel);
}

#########################################################################
#  Subroutine name : getOscarImageList                                  #
#  Parameters : none                                                    #
#  Returns : An array of directories in /var/lib/systemimager/images.   #
#  This function is called by getOscarImageName to return a list of     #
#  directories in /var/lib/systemimager/images.  These should all be    #
#  directories containing oscar images, usually named something like    #
#  'oscarimage'.                                                        #
#########################################################################
sub getOscarImageList  # -> @dirlist
{
  opendir(IMAGESDIR,$systemimagesdir) or 
    croak("Could not read the $systemimagesdir directory");
  # Save current directory for restoration later and do a cd to find images
  my $origdir = Cwd::cwd();
  chdir($systemimagesdir);
  # Generate a list of all directories available
  my @oscarimagedirs = grep { -d && !/^\.\.?$/ } readdir IMAGESDIR;
  closedir IMAGESDIR;
  chdir($origdir);
  croak("No OSCAR image directories!") if scalar(@oscarimagedirs) == 0;

  return @oscarimagedirs;
}

#########################################################################
#  Subroutine name : getOscarImageName                                  #
#  Parameters : none                                                    #
#  Returns : The name of the oscar image (directory) to use.            #
#  This function reads in the names of available oscar image            #
#  directories and, if there are two or more, prompts the user for      #
#  which one to use for booting.                                        #
#########################################################################
sub getOscarImageName # -> $oscarimagename
{
  # Check for a valid command line option first
  if ($opt_oscarimage ne "")
    {
      if (-d $systemimagesdir . '/' . $opt_oscarimage)
        { # User entered a valid oscar image on the command line
          $oscarimagename = $opt_oscarimage;
          return $oscarimagename;
        }
      else
        {
          print "$opt_oscarimage is not a valid OSCAR image directory.\n";
        }
    }

  my $counter = 1;          # Print out list of images to choose from
  my $imagechoice = 1;      # Which option did the user choose?
  my $oscarimagename = "";  # The name of the oscar image directory picked
  my @oscarimages = getOscarImageList();
  my $numimages = scalar(@oscarimages);

  if (($numimages == 1) || 
      (($numimages > 1) && (defined $opt_autoselectimage)))
    { # Either just one Oscar image directory or do auto selection of image.
      # Sort the list alphabetically (ignoring case) if more than one image.
      $oscarimagename = (sort {lc($a) cmp lc($b)} @oscarimages)[0];
      print "Using the OSCAR image named '$oscarimagename'...\n";
    }
  else
    { # More than one Oscar image directory - prompt for choice from list
      print "List of OSCAR Images Available\n";
      print "------------------------------\n";
      foreach my $oscar (sort @oscarimages)
        {
          print "\t" . $counter++ . ". $oscar\n";
        }
      print "Select the OSCAR image to use [1] : ";
      do
        {
          $imagechoice = <STDIN>;
          chomp($imagechoice);
          $imagechoice = 1 if $imagechoice =~ /^\s*$/;
          print "That is not a valid selection.  Please try again [1] : " 
            unless ($imagechoice > 0) && ($imagechoice <= $numimages);
        } until ($imagechoice > 0) && ($imagechoice <= $numimages);

      $oscarimagename = (sort @oscarimages)[$imagechoice-1];
    }

  return $oscarimagename;
}

#########################################################################
#  Subroutine name : getModuleInfo                                      #
#  Parameters : 1. The full path/name of the kernel file.               #
#               2. The version of the kernel as read from the kernel.   #
#  Returns : 1. The final version number/name of the boot kernel.       #
#            2. The original path from which to copy the /lib/modules   #
#               tree for the boot kernel.                               #
#            3. An optional full path/name of the System.map file.      #
#  This function asks the user if the boot kernel uses loadable         #
#  modules.  If so, it prompts for the version number of the kernel,    #
#  the source path containing the 'lib/modules/KERNEL_VERSION' from     #
#  which to copy the modules, and an optional source path/name of the   #
#  file to use as the System.map for depmod.                            #
#########################################################################
sub getModuleInfo # ($kernelfile,$kernelvers) ->
                  # ($kernelversion,$modulespath,$systemmappath)
{
  my ($kernelfile,$kernelvers) = @_;
  my $usesmodules = 0;       # Does the kernel use loadable modules?
  my $kernelversion = "";    # Version number entered by user
  my $modulespath = "";      # Full path to 'lib/modules'
  my $systemmappath = "";    # Fully specified name of System.map file

  # Check for command line options
  if ($opt_kernelversion ne "" || 
      $opt_modulespath ne "" || 
      $opt_systemmap ne "")
    {
      $usesmodules = 1;
      $kernelversion = $opt_kernelversion if $opt_kernelversion ne "";
      $modulespath = $opt_modulespath if $opt_modulespath ne "";
      $systemmappath = $opt_systemmap if $opt_systemmap ne "";
    }
  
  if ((!$usesmodules) && (!$commandlineoptions))
    {
      print "Does the kernel need to load any modules [Y/n] : ";
      my $ans = <STDIN>;
      chomp($ans);
      $ans =~ tr/A-Z/a-z/;
      $usesmodules = 1 if 
        ($ans eq "") || ($ans eq "y") || ($ans eq "yes") || ($ans eq "1");
    }

  if ($usesmodules)
    {
      if ($kernelversion eq "")
        {  
          do
            {
              print "Enter the version name of the compiled kernel" .
              (($kernelvers eq "") ? "" : " [$kernelvers]") .
              " : ";
              $kernelversion = <STDIN>;
              chomp $kernelversion;
              $kernelversion = $kernelvers if
                $kernelversion eq "" && $kernelvers ne "";
            } until ($kernelversion ne "");
        }

      if ($modulespath eq "")
        {
          do 
            {
              print "Enter the full path containing /lib/modules [/lib/modules/$kernelversion] : ";
              $modulespath = <STDIN>;
              chomp $modulespath;
              $modulespath =~ s,$kernelversion/?$,,; 
              $modulespath =~ s,lib/modules/?$,,;  
              $modulespath .= '/' if ($modulespath !~ /\/$/);
              print "Could not find the directory $modulespath.  Please try again.\n" 
                unless ($modulespath ne "") && (-d $modulespath);
            } until ($modulespath ne "") && (-d $modulespath);
        }

      if (($systemmappath eq "") && (!$commandlineoptions))
        {
          do
            {
              print "Enter the full path/name of the System.map file " .
                    "(<ENTER> for none) : ";
              $systemmappath = <STDIN>;
              chomp $systemmappath;
              $systemmappath =~ s/^\s*$//;
              print "Could not read the file $systemmappath.  Please try again.\n"
                unless ($systemmappath ne "") && (-r $systemmappath)
            } until ($systemmappath ne "") && (-r $systemmappath);
        }
    }

  return ($kernelversion,$modulespath,$systemmappath);
}

#########################################################################
#  Subroutine name : modifySytemConfig                                  #
#  Parameters: 1. The full path to the kernel file (ie. vmlinux).       #
#              2. The string value of the LABEL option.                 #
#              3. The value of the CONFIGRD option, "YES" or "NO".      #
#              3. The name of the Oscar image directory to be used.     #
#  Returns   : The path of the kernel boot directory in config file     #
#  This function modifies the /etc/systemconfig/systemconfig.conf       #
#  file by adding a new [KERNEL#] section with the specified PATH and   #
#  LABEL options.  It also sets the DEFAULTBOOT option to be the        #
#  passed-in LABEL so that this kernel boots first.  Finally, it copies #
#  the vmlinux kernel file to the correct location.                     #
#########################################################################
sub modifySystemConfig # ($kernelpath,$configlabel,$configrd,
                       #  $oscarimagename) -> $kerneldir
{
  my ($kernelpath,$configlabel,$configrd,$oscarimagename) = @_;
  # $kernelpath is the full path to the source kernel file
  # $configlabel is the "LABEL" value in systemconfig.conf
  # $oscarimagename is the name of the Oscar image directory to be used
  my $lastkernel = "";  # Number of the last KERNEL section
  my $kerneldir = "";   # Path of the kernel boot directory
  my $kernelname = "";  # The name only of the source kernel file (no path)
  my $lastline = "";    # Temp string for final line of config file
  my $line;             # Temp string for reading in the config file
  my $needtochangedefaultboot = 0;
  my $needtochangeconfigrd = 0;
  my $needtoaddkernel = 1;   # Assume kernel isn't in config file yet

  # Append info to the /etc/systemconfig/systemconfig.conf file.
  $configfile = $systemimagesdir . '/' . $oscarimagename . 
                "/etc/systemconfig/systemconfig.conf";

  # Extract the kernel name from the passed-in kernelpath
  $kernelpath =~ m,[^/]+$, && ($kernelname = $&);

  # Before we actually make any changes to the systemconfig.conf file, we
  # should check to see if we NEED to make any changes.  If all of the
  # information already in the file agrees with what the user wants to
  # add, then there's no need to modify it.
  open(CONF,$configfile);
  while ($line = <CONF>)
    {
      if ($line =~ /^\s*DEFAULTBOOT\s*=\s*([^\s]*)/)
        { $needtochangedefaultboot = 1 if ($1 ne $configlabel); }
      elsif ($line =~ /^\s*CONFIGRD\s*=\s*([^\s]*)/)
        { $needtochangeconfigrd = 1 if ($1 ne $configrd); }
      elsif ($line =~ /^\s*PATH\s*=\s*(.+)$/)
        {
          my $tempstring = $1;    # Save the match 
          if ($kerneldir eq "")
            { # Find kernel '/boot' directory, which is different for ia64
              $kerneldir = $tempstring;
              $kerneldir =~ s,/[^/]+$,,;  # Strip off file name from directory
            }
          $needtoaddkernel = 0 if ($tempstring eq "$kerneldir/$kernelname");
        }
    }
  close CONF;

  if ($needtochangedefaultboot || $needtochangeconfigrd || $needtoaddkernel)
    {
      # Move the original file to a unique backup file - quit if error.
      # Note that 'mv' returns 0 upon success so we have to use "and", not "or".
      my $backupfile = uniqueFile($configfile . ".bak");
      !system('mv ' . $configfile . ' ' . $backupfile) or
        croak("Could not backup $configfile!");

      # Open the input and output files
      open(ORIGCONF,"<$backupfile") or 
        croak("Could not open $backupfile for reading!");
      open(NEWCONF,">$configfile") or
        croak("Could not open $configfile for writing!");

      # Copy the original systemconfig.conf file stored as systemconfig.conf.bak
      # to the new systemconfig.conf file.  Along the way, replace the 
      # DEFAULTBOOT label with the new label.  Also, count how many "KERNEL"
      # sections are already present since we need to add another at the end,
      # and find the path name of the kernel boot directory.
      while ($line = <ORIGCONF>)
        {
          if ($line =~ /^\s*DEFAULTBOOT\s*=\s*/) 
            { 
              if ($needtochangedefaultboot)
                { print NEWCONF $& . $configlabel . "\n"; }
              else
                { print NEWCONF $line; }
            }
          elsif ($line =~ /^\s*CONFIGRD\s*=\s*/)
            { 
              if ($needtochangeconfigrd)
                { print NEWCONF $& . $configrd . "\n"; }
              else
                { print NEWCONF $line; }
            }
          else
            { print NEWCONF $line; }

          $line =~ /^\s*\[KERNEL(\d+)\]/ && ($1 gt $lastkernel) && 
            ($lastkernel = $1);

          $lastline = $line;  # Save last line for future testing
        }
      close ORIGCONF;

      if ($needtoaddkernel)
        {
          # At the end, append the new kernel's PATH and LABEL options.
          $lastkernel eq "" ? $lastkernel = "0" : $lastkernel++;
          print NEWCONF "\n" if $lastline !~ /^\s*\n$/; # Don't add two newlines
          print NEWCONF "[KERNEL" . $lastkernel . "]\n";
          print NEWCONF "\tPATH = $kerneldir/$kernelname\n";
          print NEWCONF "\tLABEL = $configlabel\n\n";
        }

      close NEWCONF;
    }

  # As a last step, copy the kernel file to the correct boot directory
  !system("cp -fp $kernelpath $systemimagesdir/$oscarimagename".$kerneldir) or
    carp("Could not copy kernel $kernelpath to " .
         "$systemimagesdir/$oscarimagename".$kerneldir);

  return $kerneldir;
}

#########################################################################
#  Subroutine name : copyKernelToNetworkBoot                            #
#  Parameters : The full path/name of the kernel file to be copied.     #
#  Returns : nothing                                                    #
#  This function copies the user-specified kernel file to the /tftpboot #
#  directory.  If the /tftpboot/kernel file already exists, it moves    #
#  it to a unique backup file.  If the kernel is not named "kernel",    #
#  it also creates a symlink.                                           #
#########################################################################
sub copyKernelToNetworkBoot # ($kernelpath, $backupkernel)
{
  my ($kernelpath,$backupkernel) = @_;

  # First, move any existing kernel to a backup file
  if ((-r "/tftpboot/kernel") && (-f "/tftpboot/kernel")) # Readable and plain
    {
      my $newkernel = uniqueFile("/tftpboot/kernel");
      !system("mv /tftpboot/kernel $newkernel") or
        carp("Could not rename /tftpboot/kernel to $newkernel");
    }

  # Copy the user-specified kernel to the /tftpboot directory
  # backing up the destination if necessary
  $kernelpath =~ m,[^/]+$, &&
    (my $kernelfile = $&);
  if ((-r "/tftpboot/$kernelfile") && ($backupkernel))
    {
      my $newkernel = uniqueFile("/tftpboot/$kernelfile");
      !system("mv /tftpboot/$kernelfile $newkernel") or
        carp("Could not rename /tftpboot/$kernelfile to $newkernel");
    }
  !system("cp -fp $kernelpath /tftpboot") or
    carp("Could not copy $kernelpath to /tftpboot");

  # If necessary, create a symbolic link from /tftpboot/kernel
  system("ln -sf /tftpboot/$kernelfile /tftpboot/kernel") if
    ($kernelfile ne 'kernel');
}


########################
#  BEGIN MAIN PROGRAM  #
########################

# First, parse the command line for all valid options
parseCommandLine();             

# For any info not entered via command line options, prompt the user
my $oscarimagename = getOscarImageName();
my ($configpath,$configlabel,$configrd,$kernelvers) = getSystemConfigVars();
my ($networkboot,$backupkernel) = getNetworkBoot($configpath);
my ($kernelversion,$modulespath,$systemmappath) = 
  getModuleInfo($configpath,$kernelvers);

# Copy the various files/directories to the appropriate locations
my $kerneldir = modifySystemConfig($configpath,$configlabel,
                                   $configrd,$oscarimagename);
copyKernelToNetworkBoot($configpath,$backupkernel) if ($networkboot eq "YES");
!system("cp -Rfp $modulespath" . "lib/modules/$kernelversion" .
       " $systemimagesdir/$oscarimagename/lib/modules") or
  carp("Could not copy the directory $modulespath"."lib/modules/$kernelversion".
       " to $systemimagesdir/$oscarimagename/lib/modules");
if ($systemmappath ne "")
  {
    !system("cp -fp $systemmappath $systemimagesdir/$oscarimagename/boot") or
      carp("Could not copy $systemmappath to " .
           "$systemimagesdir/$oscarimagename/boot");

    $systemmappath =~ m,[^/]+$, &&
      (my $systemmapfile = $&);
    system("ln -sf $systemimagesdir/$oscarimagename/boot/$systemmapfile" .
           " $systemimagesdir/$oscarimagename/boot/System.map") if
      ($systemmapfile ne 'System.map');
  }

!system('depmod -a ' . $kernelversion) or
  carp("Could not run depmod -a $kernelversion");

