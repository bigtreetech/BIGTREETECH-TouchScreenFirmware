# Custom helper for LPC17XX series chips by TG 1/26/20
# creates variable FIRMWARENAME, LOADADDRESS, and CALLER variables for use by config
# files to get env info when called by PIO Upload task (debug task doesn't need help).
# Also calculates checksum needed from the first 7 words of flash and puts it in the
# seventh word. LPC chips validate this checksum and won't run the user code if it is wrong.
# This checksum is added to the .bin and .elf files.


Import("env")

# this allows reading the upload flags and appending all of them to the openocd invocation command
# note: setting 'upload_command=openocd' and 'upload_flags= (optional commands, etc) in platformio.ini
# doesn't call openocd correctly, the upload_flags are NOT added unless we do this here as workaround!
#env.Prepend(UPLOADERFLAGS=["-c", "set FIRMWARENAME {$PROJECT_DIR\$SOURCE}"])  #want this done early!
env.Replace(
    UPLOADER="$PROJECT_PACKAGES_DIR\\tool-openocd\\bin\\openocd",
    UPLOADCMD="$UPLOADER $UPLOADERFLAGS"
)
