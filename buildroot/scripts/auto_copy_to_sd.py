Import("env")
import os
import shutil
import psutil

def copy_to_sd(source, target, env):
    repo_path = os.path.realpath("")
    build_dir = env.subst('$BUILD_DIR')
    filename = env['PROGNAME'] + ".bin"
    src_file_path = os.path.join(repo_path, build_dir, filename)
    copied = False
    unsupported_format = False
    # Iterate through all connected devices
    for partition in psutil.disk_partitions():
        # Check if the device is removable
        if partition.fstype != "" and 'removable' in partition.opts:
            if (partition.fstype == "vfat" or partition.fstype == "FAT32" or partition.fstype == "exFAT"):
                # Define destination file path
                target_file_path = os.path.join(partition.device, filename)
                # Copy the file
                shutil.copyfile(src_file_path, target_file_path)
                print(filename + " copied to " + partition.device + " succesfully.")
                copied = True
                unsupported_format = False
                break
            else:
                unsupported_format = True
    # print error if unsuccessful
    if copied == False:
        if unsupported_format:
            print("No removable storage found with file format FAT32 or exFAT!")
        else:
            print("No removable storage found!")
env.AddPostAction("buildprog", copy_to_sd)
