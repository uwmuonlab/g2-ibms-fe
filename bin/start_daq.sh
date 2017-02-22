#!/bin/bash

# The script starts general midas utilites for the experiment.
source $(dirname $(readlink -f $0))/../.expt-env

source $EXPT_DIR/bin/start_midas.sh
source $EXPT_DIR/bin/start_frontends.sh
source $EXPT_DIR/bin/start_analyzers.sh

# end script
