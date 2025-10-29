lw x2, 0(x0)       // Încarcă 10 în x2
lw x4, 4(x0)       // Încarcă 5 în x4
/*
bla
bla
blaaaa
*/
lw x6, 8(x0)       // Încarcă 20 în x6
lw x7, 12(x0)      // Încarcă 8 în x7
add x1, x2, x4     // x1 = 10 + 5 = 15
sub x5, x6, x7     // x5 = 20 - 8 = 12
sw x1, 16(x0)      // Scrie rezultatul ADD (15) în memorie[0x18+16=0x28]
sw x5, 20(x0)      // Scrie rezultatul SUB (12) în memorie[0x18+20=0x2C]