#!/bin/bash

# The script starts general midas utilites for the experiment.
source $(dirname $(readlink -f $0))/../.expt-env

source $EXPT_DIR/bin/kill_daq.sh
sleep 2
source $EXPT_DIR/bin/start_daq.sh

# end script
