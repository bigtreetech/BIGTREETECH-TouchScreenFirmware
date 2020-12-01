Import("env")

build_flags = env.ParseFlags(env['BUILD_FLAGS'])
#print(build_flags.get("CPPDEFINES"))
flags = {k: v for (k, v) in build_flags.get("CPPDEFINES")}
#print(flags)
if flags.get("HARDWARE") == "MKS_28_V1_0":
    filename = "MKSTFT28"
else:
    filename = flags.get("HARDWARE") + "." + flags.get("SOFTWARE_VERSION")
#print(filename)
env.Replace(PROGNAME=filename)
