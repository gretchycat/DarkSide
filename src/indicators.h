#if defined(PBL_RECT)
#define chargeW 8
#define chargeH 8
#define chargeX (144-chargeW)
#define chargeY 1

#define batW 16
#define batH chargeH
#define batX (144-batW-chargeW-1)
#define batY chargeY

#define btW 15
#define btH chargeH
#define btX (144-batW-btW-chargeW-2)
#define btY chargeY

#define vbW 12
#define vbH chargeH
#define vbX (144-vbW-batW-btW-chargeW-2)
#define vbY chargeY
#elif defined(PBL_ROUND)
#define chargeW 8
#define chargeH 8
#define chargeX 124-(chargeW/2)
#define chargeY 25

#define batW 16
#define batH chargeH
#define batX 124-(batW/2)
#define batY chargeY+8

#define btW 15
#define btH chargeH
#define btX 124-(btW/2)
#define btY chargeY+16

#define vbW 12
#define vbH chargeH
#define vbX 124-(vbW/2)
#define vbY chargeY+24
#endif
