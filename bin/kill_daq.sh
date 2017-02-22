#!/bin/bash

# The script kills all DAQ utilities for the experiment.
source $(dirname $(readlink -f $0))/../.expt-env

source $EXPT_DIR/bin/kill_analyzers.sh
source $EXPT_DIR/bin/kill_frontends.sh
source $EXPT_DIR/bin/kill_midas.sh

# end script
