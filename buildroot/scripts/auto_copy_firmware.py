Import("env")
import os
import shutil

def copy_firmware(source, target, env):
    print("Releasing firmware...")
    repo_path = os.path.realpath("")
    build_dir = env.subst('$BUILD_DIR')
    filename = env['PROGNAME'] + ".bin"
    src_file_path = os.path.join(repo_path, build_dir, filename)
    build_flags = env.ParseFlags(env['BUILD_FLAGS'])
    flags = {k: v for (k, v) in build_flags.get("CPPDEFINES")}
    target_dir = flags.get("BINARY_DIRECTORY")
    if target_dir == None:
        target_dir = "Copy to SD Card root directory to update"
    target_file_path = os.path.join(repo_path, target_dir, filename)
    shutil.copyfile(src_file_path, target_file_path)
    print("Done.")

env.AddPostAction("buildprog", copy_firmware)
