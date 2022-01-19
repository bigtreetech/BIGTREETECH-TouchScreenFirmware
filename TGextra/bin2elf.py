Import("env")
import shutil

def elf2bin(source, target, env):
 print("copying ELF to BIN................................................")
 
 firmware_name = env['PROGNAME']
 firmware_path = env.subst('$BUILD_DIR') + "\\"
 #print(firmware_name)
 #print(firmware_path)
 shutil.copyfile(firmware_path + firmware_name + ".elf", firmware_path + firmware_name + ".bin")
 print("bin file created.")
 #env.Replace($PROGNAME="firmware_%s" % "elf" )

env.AddPostAction("$BUILD_DIR/${PROGNAME}.elf", elf2bin)
env.AddPreAction("upload", elf2bin) # added to make .elf file before uploading when only running upload