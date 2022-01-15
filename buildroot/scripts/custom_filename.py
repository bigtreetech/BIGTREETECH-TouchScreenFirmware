Import("env")

print("Generating firmware...")
build_flags = env.ParseFlags(env['BUILD_FLAGS'])
flags = {k: v for (k, v) in build_flags.get("CPPDEFINES")}
file_name = flags.get("BINARY_FILENAME")
if file_name == None:
    file_name = flags.get("HARDWARE") + "." + flags.get("SOFTWARE_VERSION")
if flags.get("PORTRAIT_MODE") != None:
    file_name = file_name + flags.get("PORTRAIT_MODE")
print("Done.")

env.Replace(PROGNAME = file_name)
