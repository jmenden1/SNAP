File: scan.config
======================

Format: *variable value*
----------------------

-All entries into the configuration file must start at the beginning of the line
-Variables and Values are one space apart and contain \n at the end of a line
-Variables are constrained to 100 characters per line
-Values are constrained to 1000 characters per line

Currently supported configuration settings:
------------------------------------------

    devicepath /path to port connected to Sweep LIDAR
    numscansper # of scans to perform on each call to scan
    motorspeed Speed 1- 10 Hz
    samplerate Sample Rate

In order to add a new configuration option for this file. The Scan::Scan() constructor should be changed in the scan.cpp file. You'll need to add another

    else if (strcmp(var, "newvar"))

statement to the while loop.

