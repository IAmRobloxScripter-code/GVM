from os import write
import struct

def write_bytes(num: int, size: int) -> bytes:
    return num.to_bytes(size, byteorder="little")

def write_float(num: float) -> bytes:
    return struct.pack("<d", num)
Code: list[bytes] = [
    write_bytes(ord('g'), 1),
    write_bytes(ord('v'), 1),
    write_bytes(ord('m'), 1),
    write_bytes(1, 1),
    write_bytes(0, 8)
]

Pos = len(Code)
for i in range(0, 20):
    Code.append(write_bytes(0, 1))

Code += [
    write_bytes(2, 8),
    

    #stored 10_000_000 in id 0 
    write_bytes(2, 1),
    write_bytes(0, 1),
    write_float(10_000_000),
    
    write_bytes(16, 1),
    write_bytes(0, 8),


    #stored 0 in id 1 
    write_bytes(2, 1),
    write_bytes(0, 1),
    write_float(0),
    
    write_bytes(16, 1),
    write_bytes(1, 8),
    
]
Cond = 0 
for i in Code:
    Cond+=len(i)
Cond-=32;

Code += [
    #Cond 
    write_bytes(18, 1),
    write_bytes(0, 8),

    write_bytes(18, 1),
    write_bytes(1, 8),

    write_bytes(8, 1),

    write_bytes(12, 1),
    write_bytes(0, 8)
]
F_Pos = len(Code)
Code += [
    write_bytes(18, 1),
    write_bytes(1, 8),

    write_bytes(2, 1),
    write_bytes(0, 1),
    write_float(1),

    write_bytes(3, 1),

    write_bytes(16, 1),
    write_bytes(1, 8),
    write_bytes(10, 1), 
    write_bytes(Cond, 8)
]
End = 0 
for i in Code:
    End+=len(i)
End-=32 

Code[F_Pos-1] = write_bytes(End, 8)

Count = 0
for i in Code:
    Count+=len(i)
print(Count-32)
Code[Pos - 1] = write_bytes(Count-32, 8)
with open("./build/main.vm", "wb") as File:
    File.write(b''.join(Code))
