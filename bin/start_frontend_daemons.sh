#!/bin/bash

# Load our experiment environment variables
source $(dirname $(readlink -f $0))/../.expt-env

odbedit -e $EXPT -c 'clean'
for fe in "${EXPT_FE[@]}"; do
    ${EXP_DIR}bin/${fe} -e $EXPT -D
done

# end script
