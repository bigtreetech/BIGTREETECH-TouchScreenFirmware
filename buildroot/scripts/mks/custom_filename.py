Import("env")

#my_flags = env.ParseFlags(env['BUILD_FLAGS'])
# Rename output file so that the MKS bootloader picks it up.
env.Replace(PROGNAME="mkstft28")