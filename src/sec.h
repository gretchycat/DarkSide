#if defined(PBL_RECT)
#define secX (timeW+timeX)
#define secY (6+timeY)
#elif defined(PBL_ROUND)
#define secX 100
#define secY (timeH+timeY-7)
#endif
#define secW (watchW-secX)
#define secH 25+5
#define secA GTextAlignmentCenter
#define secFormat "%P"
#define secFormat24 "%S"

