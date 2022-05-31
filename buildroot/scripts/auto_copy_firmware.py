Import("env")
import shutil

def copy_firmware(source, target, env):
    firmware_name = "/" + env['PROGNAME'] + ".bin"
    print("copy firmware for release...")
    firmware_path = env.subst('$BUILD_DIR') + firmware_name
    shutil.copyfile(firmware_path, "Copy to SD Card root directory to update/" + firmware_name)
    print("release Done.")

env.AddPostAction("buildprog", copy_firmware)
