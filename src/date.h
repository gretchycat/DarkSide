#if defined(PBL_RECT)
#define dateX 3
#define dateY (timeY+timeH+3)
#define dateW (bounds.size.w-(2*dateX))
#define dateH 20
#elif defined(PBL_ROUND)
#define dateX 20
#define dateY (timeY+timeH+3)
#define dateW (bounds.size.w-(2*dateX)-40)
#define dateH 20
#endif
#define dateA GTextAlignmentCenter
#define dateFormat "%m/%d/%Y"
#define dateFormat24 "%m/%d/%Y"


