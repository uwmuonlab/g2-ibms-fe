#!/bin/bash

# Run all ODB subtree init scripts.
cd $(dirname $(readlink -f $0))/odb.d
source ../../.expt-env

# Make sure the odb is initialized.
odbedit -e $(EXPT) -s 20000000

# Run variable scripts.
for script in `ls [0-9]?-*.cmd`
do
    odbedit -e $(EXPT) -c @${script}
done
