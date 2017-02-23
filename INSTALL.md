# IBMS Installation Notes

- clone repo to anywhere
```bash
mkdir ~/Applications && cd ~/Applications
git clone https://github.com/uwmuonlab/g2-ibms-fe
```

- install headers in core to /usr/local/include
```bash
cd core
sudo make install
cd ..
``

- setup dependencies with scripts (`init/install.d`)
  - can delete 20-cvmfs.sh and 45-gm2midas.sh if those are already setup
  - also make sure there is an activie kerberos ticket
```bash
cd init
./install.sh
cd ..
```

- set up expt-env (used by my kill, start, and init scripts)
  - edit to fit the set up
```bash
cp init/expt-env .expt-env
```

- initialize odb for ibms
  - 
```bash
./init-odb.sh

- install field-hardware-lib (only vme)
```bash
mkdir ~/Packages && cd ~/Packages
git clone https://github.com/g2-field-team/field-hardware-lib
cd field-hardware-lib/vme-interface
make
sudo make install
```

- build the front-end
```bash
cd ~/Applications/g2-ibms-fe/frontend
make
```

- try running things
  - run the front-end manually
  - can also use the start/kill scripts
```bash
cd ~/Applications/g2-ibms-fe
./bin/restart_daq.sh
```
