import sys

file = open("out.zip", "wb")

for line in sys.stdin:
    data = line.strip().split('-')
    if (len(data) > 0):
        hexdata = data[0]
        databytes = hexdata.split()
        for byte in databytes:
            print(byte)
            databyte = bytes.fromhex(byte)
            file.write(databyte)
