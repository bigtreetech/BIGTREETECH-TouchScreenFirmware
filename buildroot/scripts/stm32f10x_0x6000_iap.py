Import("env")

# Relocate firmware from 0x08000000 to 0x08006000
env.Replace(LDSCRIPT_PATH="buildroot/ldscripts/stm32f10x_0x6000_iap.ld")
