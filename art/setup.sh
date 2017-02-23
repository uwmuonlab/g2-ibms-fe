1;95;0c#!/bin/bash

# A setup script for field art modules.

# Make sure CervVM-FS is setup already.
if [ ! -d "/cvmfs/gm2.opensciencegrid.org" ]; then
    exit 1
fi;

# Switch to the scripts directory.
cd $(dirname $(readlink -f $0))

# Now proceed with setting up gm2 packages via UPS.
source /cvmfs/gm2.opensciencegrid.org/prod7/g-2/setup
setup gm2 v7_03_01 -q prof

# And initialize a new development space.
mrb newDev -f
source localProducts*/setup
cd $MRB_SOURCE

mrb g -b feature/tier0v7-field gm2dataproducts
mrb g -b feature/tier0v7-field gm2midastoart
mrb g -b feature/tier0v7-field gm2unpackers

# End script
