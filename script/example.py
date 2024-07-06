#!/usr/bin/env python
import pyclvhd

m = pyclvhd.Controller()
m.serial_connection(path="/dev/ttyACM0")
n = m.setup()
print("Found %d slaves" % n)
route_table = [[1,2], [3,4], [5,6]]
chx_enable = [True, True, True]
chx_high_res = [True, True, True]
chx_high_freq = [True, True, True]
R1 = [2, 4, 4]
R2 =  4 
R3 =  [4, 4, 4]
#m.test()
for i in range(n):
    m.setupEMG(0, route_table, chx_enable, chx_high_res,
               chx_high_freq, R1, R2, R3);

