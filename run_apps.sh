# Setup the second wine prefix with:
# `WINEPREFIX="$HOME/.wine_2" winecfg` 

DISPLAY_A=:0
WINEPREFIX_A="$HOME/.wine"
DISPLAY_B=:0
WINEPREFIX_B="$HOME/.wine_2"

cd ~/.steam/steam/steamapps/common/Noita/
DISPLAY=$DISPLAY_A \
	WINEPREFIX=$WINEPREFIX_A \
	wine noita.exe &
DISPLAY=$DISPLAY_B \
	WINEPREFIX=$WINEPREFIX_B \
	wine noita.exe &
wait
