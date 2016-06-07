smartbeholder
=============

Smart Beholder: An Open-Source Smart Lens for Mobile Photography

# Overview

Smart Beholder is an open-source smart lens for mobile photogrphy.
Smart lenses are detachable lenses connected to mobile devices via
wireless networks, which are not constrained by the small form factor of
mobile devices, and have potential to deliver better photo (video) quality.
In addition to its openness, we design Smart Beholder for cost effectiveness,
low latency, and high quality.
Smart Beholder currently supports Raspberry Pi, and can be ported to
other embedded boards with camera, Wi-Fi, and hardware
accelerated video codec modules.

# Documents

Official web site: http://www.smartbeholder.org/

Download: http://www.smartbeholder.org/download.html

# Quick Installation Guide

* Prerequisite

  - GNU C++ compiler
  - pkg-config

* Steps

  1. Edit the 'env-setup' script to ensure that SBDEPS points to a correct directory.
     It should be an absolute path to smartbeholder/deps.posix directory.
  2. Merge environment variables from 'env-setup' by using '.' or 'source' command.
  3. Build dependencies by running 'make' in the 'deps.src' directory.
  4. Build smartbeholer by running 'make all' command in the 'codes' directory.
  5. Install smartbeholder by running 'make install' command in the 'codes' directory.
     All the generated files will be installed into 'bin' directory.
