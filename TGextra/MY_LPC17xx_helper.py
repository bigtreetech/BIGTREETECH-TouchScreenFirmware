# Custom helper for LPC17XX series chips by TG 1/26/20
# creates variable FIRMWARENAME, LOADADDRESS, and CALLER variables for use by config
# files to get env info when called by PIO Upload task (debug task doesn't need help).
# Also calculates checksum needed from the first 7 words of flash and puts it in the
# seventh word. LPC chips validate this checksum and won't run the user code if it is wrong.
# This checksum is added to the .bin and .elf files.


Import("env")

# get any py modules that are needed if not already present
try:
 import sys
except:
 env.Execute("$PYTHONEXE -m pip install sys")
try:
 import struct
except:
 env.Execute("$PYTHONEXE -m pip install struct")
try:
 import argparse
except:
 env.Execute("$PYTHONEXE -m pip install argparse")
try:
 import intelhex
except:
 env.Execute("$PYTHONEXE -m pip install intelhex")
try:
 import os.path
except:
 env.Execute("$PYTHONEXE -m pip install os.path")

# this allows reading the upload flags and appending all of them to the openocd invocation command
# note: setting 'upload_command=openocd' and 'upload_flags= (optional commands, etc) in platformio.ini
# doesn't call openocd correctly, the upload_flags are NOT added unless we do this here as workaround!
#env.Prepend(UPLOADERFLAGS=["-c", "set FIRMWARENAME {$PROJECT_DIR\$SOURCE}"])  #want this done early!
env.Replace(
    UPLOADER="$PROJECT_PACKAGES_DIR\\tool-openocd\\bin\\openocd",
    UPLOADCMD="$UPLOADER $UPLOADERFLAGS"
)

#start of the LPC checksum calc needed for all LPC1700 parts to run loaded program correctly.
"""
Calculate checksum image for LPC firmware images and write. Code is a Python port of the C version
written by Roel Verdult named `lpcrc'. The checksum is the two's-complement of the sum of the first
seven 4-byte blocks (w.r.t the start address). This value is placed in the eight block.
The checksum is written back to the file and is returned. When read_only is set to True,
the file will not be changed.

filename  -- firmware file to checksum
format    -- (optional) input file format (bin or hex, doesn't matter hsndles both bin and elf)
read_only -- (optional) whether to write checksum back to the file (default False)
"""

def start_checksum(source, target, env):
                 
    #get the name of the output file
    #print("lpc17xx_helper.py: Calculating lpc_checksum.................")
    firmware_path = env.subst('$BUILD_DIR') + "\\"
        
    #patch the bin file first
    print ("")
    shortfilename = env.get("PROGNAME") + ".bin"
    filename = firmware_path + shortfilename
    if os.path.isfile(filename):
        (checksum(filename,shortfilename,"bin"))
    
    #now patch the elf file
    shortfilename = env.get("PROGNAME") + ".elf"
    filename = firmware_path + shortfilename
    #if os.path.isfile(filename):
    #    (checksum(filename,shortfilename,"bin"))
    print ("")

def checksum(filename, shortfilename, format="bin", read_only=False):
    BLOCK_COUNT = 7
    BLOCK_SIZE = 4
    BLOCK_TOTAL = (BLOCK_COUNT * BLOCK_SIZE)
    
    # Open the firmware file.
    handle = intelhex.IntelHex()
    handle.loadfile(filename, format=format)
    
    #bin file code starts at 0x00000000
    block_start = handle.minaddr() 
    #elf file code starts at 0x00010000
    if "elf" in filename:
        block_start = block_start + 0x00010000

    # Read the data blocks used for checksum calculation.
    block = bytearray(handle.gets(block_start, BLOCK_TOTAL))

    if len(block) != BLOCK_TOTAL:
        raise Exception("lpc17xx_helper.py: Could not read the required number of bytes.")

    # Compute the checksum value.
    result = 0
    for i in range(BLOCK_COUNT):
        value, = struct.unpack_from("I", block, i * BLOCK_SIZE)
        result = (result + value) & 0xFFFFFFFF                  # add up the first 7 vectors in interrupt table @0x00000000

    result = ((~result) + 1) & 0xFFFFFFFF                       # form two's complement

    # Write checksum back to the file.
    if not read_only:
        handle.puts(block_start + BLOCK_TOTAL, struct.pack("I", result))
        handle.tofile(filename, format=format)

    print("lpc17xx_helper.py: Succesfully updated " + shortfilename + " checksum to 0x{0:X}".format(result))
    #print(" checksum to 0x%08x \n" % result)
    # Done
    return result

# add the desired actions to the appropriate tasks here
env.AddPostAction("buildprog", start_checksum)  # make sure this calls after the build finishes (.bin file must be present first!)
env.AddPreAction("upload", start_checksum)      # make sure this calls before any upload begins