Import("env")

# Relocate firmware from 0x08000000 to 0x08008000
env.Replace(LDSCRIPT_PATH="buildroot/ldscripts/stm32f2xx_0x8000_iap.ld")
