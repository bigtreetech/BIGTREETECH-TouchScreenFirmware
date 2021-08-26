Import("env")
import shutil

def make_short_file(source, target, env):
    print("Generating firmware with short file name...")
    buildDir = env.subst('$BUILD_DIR')
    build_flags = env.ParseFlags(env['BUILD_FLAGS'])
    flags = {k: v for (k, v) in build_flags.get("CPPDEFINES")}
    filename = flags.get("BINARY_FILENAME")
    if filename == None:
        filename = flags.get("HARDWARE") + "." + flags.get("SOFTWARE_VERSION")
    filename_short = flags.get("HARDWARE_SHORT") + flags.get("SOFTWARE_VERSION_SHORT")
    shutil.copyfile(buildDir + "/" + filename + ".bin", buildDir + "/" + filename_short + ".new")
    print("Done.")

env.AddPostAction("buildprog", make_short_file)
