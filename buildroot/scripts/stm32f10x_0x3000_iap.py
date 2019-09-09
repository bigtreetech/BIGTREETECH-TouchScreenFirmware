Import("env")

# Relocate firmware from 0x08000000 to 0x08003000
env.Replace(LDSCRIPT_PATH="buildroot/ldscripts/stm32f10x_0x3000_iap.ld")
