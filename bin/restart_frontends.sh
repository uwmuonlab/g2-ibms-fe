#!/bin/bash

# The script starts general midas utilites for the experiment.
source $(dirname $(readlink -f $0))/../.expt-env

source $EXPT_DIR/bin/kill_frontends.sh
sleep 2
source $EXPT_DIR/bin/start_frontends.sh

# end script
