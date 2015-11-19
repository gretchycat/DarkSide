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

#define apX secX-7 
#define apY secY+2 

#elif defined(PBL_ROUND)
#define chargeW 8
#define chargeH 8
#define chargeX 130
#define chargeY 12

#define batW 16
#define batH chargeH
#define batX 130
#define batY 20 

#define vbW 12
#define vbH 8 
#define vbX 38 
#define vbY 16 

#define btW 15
#define btH	8 
#define btX	vbX-2 
#define btY vbH+vbY 

#define apX secX+12 
#define apY dateY 

#endif
