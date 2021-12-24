Import("env")

print("Generating firmware...")
build_flags = env.ParseFlags(env['BUILD_FLAGS'])
#print(build_flags.get("CPPDEFINES"))
flags = {k: v for (k, v) in build_flags.get("CPPDEFINES")}
#print(flags)
filename = flags.get("BINARY_FILENAME")
if filename == None:
    filename = flags.get("HARDWARE") + "." + flags.get("SOFTWARE_VERSION")
if flags.get("PORTRAIT_MODE") != None:
    filename = filename + flags.get("PORTRAIT_MODE")
#print(filename)
print("Done.")

env.Replace(PROGNAME = filename)
