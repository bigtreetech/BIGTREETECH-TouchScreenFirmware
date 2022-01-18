Import("env")
import shutil

def make_short_file(source, target, env):
    print("Generating firmware with short file name...")
    build_dir = env.subst('$BUILD_DIR')
    filename = "/" + env['PROGNAME'] + ".bin"
    build_flags = env.ParseFlags(env['BUILD_FLAGS'])
    flags = {k: v for (k, v) in build_flags.get("CPPDEFINES")}
    filename_short = "/" + flags.get("HARDWARE_SHORT") + flags.get("SOFTWARE_VERSION_SHORT") + ".new"
    shutil.copyfile(build_dir + filename, build_dir + filename_short)
    print("Done.")

env.AddPostAction("buildprog", make_short_file)
