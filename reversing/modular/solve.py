from z3 import *

def bv2int(bv):
    return int(str(bv))

matrix = [BitVec(str(i), 32) for i in range(32)]

s = Solver()

s.add(matrix[1] - 3 == matrix[3])
s.add(matrix[0] == matrix[28])
s.add(matrix[3] == matrix[10])
s.add(matrix[16] == matrix[19])
s.add(matrix[23] + 2 == matrix[24])
s.add(matrix[25] + matrix[26] == 106)
s.add(matrix[24] == matrix[25])
s.add(matrix[20] - matrix[16] == 15)
s.add(matrix[28] - matrix[24] == 44)
s.add(matrix[8] + 17 == matrix[9])
s.add(matrix[0] == 95)
s.add(matrix[1] - 1 == matrix[5])
s.add(matrix[2] - matrix[3] == 5)
s.add(matrix[12] == 114)
s.add(matrix[13] == 95)
s.add(matrix[31] == 121)
s.add(matrix[10] == matrix[31] - 9)
s.add(matrix[4] == matrix[11])
s.add(matrix[16] == matrix[4])
s.add(matrix[21] == matrix[22])
s.add(matrix[21] + matrix[22] == 190)
s.add(matrix[21] == matrix[6])
s.add(matrix[22] == matrix[7])
s.add(matrix[11] - matrix[0] == 6)
s.add(matrix[4] == matrix[8] + 1)
s.add(matrix[30] + matrix[29] - matrix[31] == 87)
s.add(matrix[4] == matrix[30])
s.add(matrix[1] == matrix[15])
s.add(matrix[20] - 2 == matrix[18])
s.add(matrix[19] == matrix[30])
s.add(matrix[7] == matrix[27])
s.add(matrix[7] == matrix[14])
s.add(matrix[17] == 99)

print s.check()

m = s.model()
#print m

dec = "".join([chr(bv2int(m[matrix[i]])) for i in range(32)])
print dec
