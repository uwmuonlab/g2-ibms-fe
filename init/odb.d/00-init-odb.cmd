create STRING "Experiment/Menu Buttons"
set "Experiment/Menu Buttons" "Status, ODB, Messages, Alarms, Programs, History, MSCB, Help"
create BOOL "/Experiment/Run Parameters/Root Output"
set "/Experiment/Run Parameters/Root Output" y

create BOOL "/Experiment/Run Parameters/Midas Output"
set "/Experiment/Run Parameters/Root Output" n

create DWORD "/Experiment/MAX_EVENT_SIZE"
set "/Experiment/MAX_EVENT_SIZE" 0x1000000

mkdir "/Settings"

