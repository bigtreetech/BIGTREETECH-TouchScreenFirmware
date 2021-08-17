Import("env")
import shutil

def copy_firmware(source, target, env):
    print("Realeasing firmware...")
    firmware_name = "/" + env['PROGNAME'] + ".bin"
    firmware_path = env.subst('$BUILD_DIR') + firmware_name
    build_flags = env.ParseFlags(env['BUILD_FLAGS'])
    flags = {k: v for (k, v) in build_flags.get("CPPDEFINES")}
    target_directory = flags.get("BINARY_DIRECTORY")
    if target_directory == None:
        shutil.copyfile(firmware_path, "Copy to SD Card root directory to update/" + firmware_name)
    else:
        shutil.copyfile(firmware_path, target_directory + firmware_name)
    print("Done.")

env.AddPostAction("buildprog", copy_firmware)
