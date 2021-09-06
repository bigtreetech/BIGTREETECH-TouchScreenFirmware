Import("env")

print("Generating firmware...")
build_flags = env.ParseFlags(env['BUILD_FLAGS'])
#print(build_flags.get("CPPDEFINES"))
flags = {k: v for (k, v) in build_flags.get("CPPDEFINES")}
#print(flags)
if flags.get("HARDWARE") == "MKS_TFT28_V4.0":
    filename = "MKSTFT28"
else:
    filename = flags.get("HARDWARE") + "." + flags.get("SOFTWARE_VERSION")
#print(filename)
print("Done.")

env.Replace(PROGNAME = filename)
