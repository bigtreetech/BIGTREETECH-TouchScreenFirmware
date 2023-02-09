Import("env")

build_flags = env.ParseFlags(env['BUILD_FLAGS'])
flags = {k: v for (k, v) in build_flags.get("CPPDEFINES")}
filename = flags.get("BINARY_FILENAME")
# set file name by hardware and firmware version
if filename == None:
    filename = flags.get("HARDWARE") + "." + flags.get("SOFTWARE_VERSION")
# rename firmware if portrait mode is selected
if flags.get("PORTRAIT_MODE") != None:
    filename = filename + flags.get("PORTRAIT_MODE")

env.Replace(PROGNAME = filename)
