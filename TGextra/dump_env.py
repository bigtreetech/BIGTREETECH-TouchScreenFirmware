Import("env")
print("RUNNING EXTRA SCRIPT")
#print(env.Dump())
build_flags = env.ParseFlags(env['BUILD_FLAGS'])
#print(env.get("PROGNAME"))
filename = env.get("PROGNAME") + env.get("PROGSUFFIX")
filetype = env.get("PROGSUFFIX").lstrip(".")
print(filename)
print(filetype)
# this allows reading the upload flags and appending all to the openocd invocation
# putting the openocd as upload_command and items in upload_flags doesn't call
# openocd correctly, the upload_flags are NOT added unless we do this here as workaround
env.Replace(
    UPLOADER="$PROJECT_PACKAGES_DIR\\tool-openocd\\bin\\openocd",
    UPLOADCMD="$UPLOADER $UPLOADERFLAGS"
)
print(env.Dump())