Import("env")
import struct, os, platform, shutil
from os import path

def mks_encrypt_firmware(source, target, env):
    print("Encrypting MKS Firmware...")
    buildDir = env.subst('$BUILD_DIR')
    build_flags = env.ParseFlags(env['BUILD_FLAGS'])
    flags = {k: v for (k, v) in build_flags.get("CPPDEFINES")}
    filename = flags.get("HARDWARE") + "." + flags.get("SOFTWARE_VERSION")
    filename_short = flags.get("HARDWARE_SHORT") + flags.get("SOFTWARE_VERSION_SHORT")
    sourceFile = buildDir + "\\" + filename + ".BIN"
    mksFile = filename[:-10] + ".BIN"
    destinationFile = buildDir + "\\" + mksFile

    if path.exists(sourceFile):
        key = [163, 189, 173, 13, 65, 17, 187, 141, 220, 128, 45, 208, 210, 196, 155, 30, 38, 235, 227, 51, 74, 21, 228, 10, 179, 177, 60, 147, 187, 175, 247, 62]
        localFile = open(sourceFile, 'rb')
        resultFile = open(destinationFile, 'wb')
        localFile.seek(0)
        chunk = localFile.read(320)
        if not chunk:
            localFile.close()
            resultFile.close()
            print("Failed to encrypt firmware!")
            exit(1)
        resultFile.write(chunk)

        localFile.seek(320)      # Go to file position 0x140
        while localFile.tell() < 320 + 30720:   # Encode until: 0x7940
            for i in range(32):
                tmp = localFile.read(1)
                tmp1, = struct.unpack('B', tmp)
                tmp1 = tmp1 ^ key[i]
                resultFile.write(struct.pack('B', tmp1))
        chunk = localFile.read()
        if not chunk:
            localFile.close()
            resultFile.close()
            print("Failed to encrypt firmware!")
            exit(1)

        resultFile.write(chunk)
        localFile.close()
        resultFile.close()
        print("UNENCRYPTED MKS Firmware file (rename to upload!): " + sourceFile)
        print("ENCRYPTED MKS Firmware file: " + destinationFile)
    else:
        print("ERROR: MKS Firmware file not found!")

env.AddPostAction("buildprog", mks_encrypt_firmware)