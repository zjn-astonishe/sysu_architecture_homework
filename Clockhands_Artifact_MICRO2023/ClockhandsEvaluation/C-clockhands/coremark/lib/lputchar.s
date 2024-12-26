Function_putchar :
 ADDi.64  s, s2, 0
 ST.64    n, s1, s3, -8
 LUi      t, 460631
 RMOV     s, s2
 ADDi.64  s, t0, 1123
 ECALL    n, 0
 LD.64    t, s0, -8
 JR       n, t0, 0
