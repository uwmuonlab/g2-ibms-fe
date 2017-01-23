mkdir "/Settings/Hardware/caen_1742/template"
cd "/Settings/Hardware/caen_1742/template"
create INT base_address
set base_address 0x00000000

create DOUBLE sampling_rate
set sample_rate 1.0

create INT pretrigger_delay
set pretrigger_delay 50

create BOOL drs_cell_corrections
set drs_cell_corrections y

create BOOL drs_time_corrections
set drs_time_corrections y

create BOOL drs_peak_corrections
set drs_peak_corrections n

create DOUBLE channel_offset[32]
set channel_offset[*] 0.0

cd ..
cp template 0
