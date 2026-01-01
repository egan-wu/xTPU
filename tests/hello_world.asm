# Hello World Test
# Initialize registers with immediates
LI r1, 0x100
LI r2, 0x200

# Store some values to memory
# r1 (0x100) -> [0x1000]
STORE r1, 0x1000
# r2 (0x200) -> [0x1008]
STORE r2, 0x1008

# Read back
# r4 = [0x1000] -> should be 0x100
LOAD r4, 0x1000

# Arithmetic
# r3 = r1 + r2 = 0x100 + 0x200 = 0x300
VEC_ADD r3, r1, r2

HALT
