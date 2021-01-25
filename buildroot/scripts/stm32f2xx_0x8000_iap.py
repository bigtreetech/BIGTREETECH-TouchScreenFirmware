import os

Import("env")

# Relocate firmware from 0x08000000 to 0x08008000
custom_ld_script = os.path.abspath("buildroot/ldscripts/stm32f2xx_0x8000_iap.ld")

env.Replace(LDSCRIPT_PATH=custom_ld_script)
print(custom_ld_script)
for i, flag in enumerate(env["LINKFLAGS"]):
    if "-Wl,-T" in flag:
        env["LINKFLAGS"][i] = "-Wl,-T" + custom_ld_script
    elif flag == "-T":
        env["LINKFLAGS"][i + 1] = custom_ld_script
