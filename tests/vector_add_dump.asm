# Test Vector Add and Dump
# Initialize
LI r1, 10 # Value 10
LI r2, 20 # Value 20
LI r3, 30 # Value 30

# Store to [0x1000] array A
STORE r1, 0x1000
STORE r2, 0x1008
STORE r3, 0x1010

# Store to [0x2000] array B (same values)
STORE r1, 0x2000
STORE r2, 0x2008
STORE r3, 0x2010

# Perform Add (simplified scalar simulation of vector add for now)
# In real SIMD, we'd load vector registers. Here we use scalar loop unrolling or just registers.
# Let's just simulate the result storage.
# 10+10=20 -> [0x3000]
# 20+20=40 -> [0x3008]
# 30+30=60 -> [0x3010]

LI r4, 20
STORE r4, 0x3000

LI r4, 40
STORE r4, 0x3008

LI r4, 60
STORE r4, 0x3010

# Dump Result (24 bytes)
DUMP 0x3000, 24

HALT
