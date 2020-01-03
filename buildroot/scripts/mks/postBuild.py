Import("env")
import os
from distutils.dir_util import copy_tree, remove_tree, mkpath
import shutil 
print "Post build scripts"

def after_build(source, target, env):
    # Remove any existing Folder
    dst_dir = env['PROJECT_DIR'] + "/Copy to SD Card - MKS/"
    if os.path.exists(dst_dir):
        remove_tree(dst_dir)
        mkpath(dst_dir)
    # Copy Bin File to SD Card Folder
    source_filename = env['PROJECT_BUILD_DIR'] + "/" + env['PIOENV'] + "/" + env['PROGNAME'] + ".bin"
    dst_filename = dst_dir + env['PROGNAME'] + ".bin"
    shutil.copyfile(source_filename, dst_filename)
    # Copy BMP folder to SD Card Folder
    src_dir = env['PROJECT_DIR'] + "/Copy to SD Card root directory to update/TFT28"
    copy_tree(src_dir, dst_dir + "TFT28/")
    
env.AddPostAction("buildprog", after_build)

