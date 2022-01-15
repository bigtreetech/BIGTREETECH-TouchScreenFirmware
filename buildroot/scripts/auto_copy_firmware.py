Import("env")
import shutil

def copy_firmware(source, target, env):
    print("Releasing firmware...")
    build_dir = env.subst('$BUILD_DIR')
    file_name = "/" + env['PROGNAME'] + ".bin"
    file_path = build_dir + file_name
    build_flags = env.ParseFlags(env['BUILD_FLAGS'])
    flags = {k: v for (k, v) in build_flags.get("CPPDEFINES")}
    target_dir = flags.get("BINARY_DIRECTORY")
    if target_dir == None:
        target_dir = "Copy to SD Card root directory to update"
    shutil.copyfile(file_path, target_dir + file_name)
    print("Done.")

env.AddPostAction("buildprog", copy_firmware)
