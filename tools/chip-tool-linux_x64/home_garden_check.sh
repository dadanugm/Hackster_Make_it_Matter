echo soil sensor value
> dump.txt
./chip-tool-debug levelcontrol read current-level 1 1 >> dump.txt
grep -oP "(?<=Data = ).*(?=,)" dump.txt
