Import("env")
import struct, os, platform, shutil
from os import path

def mks_encrypt_firmware(source, target, env):
    print("Encrypting MKS firmware...")
    repo_path = os.path.realpath("")
    build_dir = env.subst('$BUILD_DIR')
    filename = env['PROGNAME'] + ".bin"
    filename_unencrypted = filename + "_unencrypted"
    src_file_path = os.path.join(repo_path, build_dir, filename)
    src_file_path_unencrypted = os.path.join(repo_path, build_dir, filename_unencrypted)

    shutil.move(src_file_path, src_file_path_unencrypted)

    if path.exists(src_file_path_unencrypted):
        key = [163, 189, 173, 13, 65, 17, 187, 141, 220, 128, 45, 208, 210, 196, 155, 30, 38, 235, 227, 51, 74, 21, 228, 10, 179, 177, 60, 147, 187, 175, 247, 62]
        localFile = open(src_file_path_unencrypted, 'rb')
        resultFile = open(src_file_path, 'wb')
        localFile.seek(0)
        chunk = localFile.read(320)
        if not chunk:
            localFile.close()
            resultFile.close()
            print("Failed to encrypt firmware!")
            exit(1)
        resultFile.write(chunk)

        localFile.seek(320)                    # Go to file position 0x140
        while localFile.tell() < 320 + 30720:  # Encode until: 0x7940
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
        print("UNENCRYPTED MKS firmware file (rename to upload!): " + filename_unencrypted)
        print("ENCRYPTED MKS firmware file: " + filename)
    else:
        print("ERROR: MKS firmware file not found!")
    print("Done.")

env.AddPostAction("buildprog", mks_encrypt_firmware)
