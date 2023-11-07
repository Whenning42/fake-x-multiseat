# Setup the second wine prefix with:
# `WINEPREFIX="$HOME/.wine_2" winecfg` 

WORKING_DIR=$(pwd)

python proxy.py &

DISPLAY_A=:1
WINEPREFIX_A="$HOME/.wine"
DISPLAY_B=:1
WINEPREFIX_B="$HOME/.wine_2"

cd ~/.steam/steam/steamapps/common/Noita/
DISPLAY=$DISPLAY_A \
	WINEPREFIX=$WINEPREFIX_A \
	wine noita.exe &> log_a.txt &
DISPLAY=$DISPLAY_B \
	WINEPREFIX=$WINEPREFIX_B \
	wine noita.exe &> log_b.txt &
cd $WORKING_DIR

sleep 3

# Get window ids of open windows named "Noita"
WIN_A=$(xwininfo -root -tree | grep "Noita" | grep -oP " 0x\w+" | head -n 1)
WIN_B=$(xwininfo -root -tree | grep "Noita" | grep -oP " 0x\w+" | head -n 2 | tail -n 1)

# Move the windows into place
xdotool windowmove $WIN_A 0 880
xdotool windowmove $WIN_B 640 880

sleep 10 

# Run presses with the given window ids
./run_presses $WIN_A $WIN_B

wait
