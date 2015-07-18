#include <pebble.h>
#include "defines.h"
#include "watch.h"
#include "sec.h"
#include "date.h"
#include "update.h"
#include "indicators.h"
#include "weather.h"
#include "weathermore.h"
#include "calendar.h"
#include "moon.h"
#include "sunlight.h"
#include "compass.h"

#define bounds layer_get_bounds(window_get_root_layer(window))

#define SYNC_ERROR_TIMEOUT 60*1

#define TAPTIMER 15

