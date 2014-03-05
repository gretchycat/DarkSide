#include <pebble.h>

#define bounds layer_get_bounds(window_get_root_layer(window))

#define timeX 0
#define timeY 2
#define timeW 103
#define timeH 31
#define timeA GTextAlignmentCenter
#define timeFormat "%l:%M"
#define timeFormat24 "%H:%M"

#define secX (timeW+timeX)
#define secY (6+timeY)
#define secW (144-secX)
#define secH 25
#define secA GTextAlignmentCenter
#define secFormat "%P"
#define secFormat24 "%S"

#define dateX 3
#define dateY (timeY+timeH+2)
#define dateW (bounds.size.w-(2*dateX))
#define dateH 20
#define dateA GTextAlignmentCenter
#define dateFormat "%m/%d/%Y"
#define dateFormat24 "%m/%d/%Y"

#define batW 16
#define batH 8
#define batX (144-batW-chargeW-1)
#define batY 0

#define chargeW 8
#define chargeH 8
#define chargeX (144-chargeW)
#define chargeY batY

#define btW 15
#define btH 8
#define btX (144-batW-btW-chargeW-2)
#define btY batY

#define wetX 0
#define wetY (dateY+dateH+2)
#define wetW bounds.size.w
#define wetH 64

#define calX 0
#define calY (wetY+wetH+2)
#define calW bounds.size.w
#define calH (168-calY)
#define calR 3
#define calDayW ((144/7)-1)
#define calDayH ((calH/calR)-1)
#define calA GTextAlignmentCenter
#define TAPTIMER 10
#define WEATHERTIMER 60*30
#define FORECASTDAYS 5

char str_time[10];
char str_date[14];
char str_sec[4];

GFont *timeF=NULL;
GFont *secF=NULL;
GFont *dateF=NULL;
GFont *calHeadF=NULL;
GFont *calDayF=NULL;
GFont *calNowF=NULL;
GFont *tinyF=NULL;
GFont *medF=NULL;
GFont *medBF=NULL;

bool useFahrenheit=true;
static GBitmap *bat[11];
static GBitmap *charge=NULL;
static GBitmap *bton=NULL;
static GBitmap *btoff=NULL;
static GBitmap *back=NULL;
static GBitmap *darkside=NULL;
static Window *window=NULL;
static Layer *window_layer=NULL;
static Layer *weather_layer=NULL;
static TextLayer *time_layer=NULL;
static TextLayer *sec_layer=NULL;
static TextLayer *date_layer=NULL;
static TextLayer *calendar[7][calR];
static char calDay[7][calR-1][3];
static BitmapLayer *bt_layer=NULL;
static BitmapLayer *bat_layer=NULL;
static BitmapLayer *charge_layer=NULL;
static BitmapLayer *back_layer=NULL;
static BitmapLayer *darkside_layer=NULL;
static int today=0;
static int tapsec=0;
static int wetsec=0;
static TextLayer *city_layer=NULL;
static TextLayer *temperature_layer=NULL;
static BitmapLayer *icon_layer=NULL;
static TextLayer *tempMin[FORECASTDAYS];
static TextLayer *tempMax[FORECASTDAYS];
static TextLayer *forecastDay[FORECASTDAYS];
static BitmapLayer *forecastIcon[FORECASTDAYS];
char temp[FORECASTDAYS][16];
char min[FORECASTDAYS][16];
char max[FORECASTDAYS][16];
static GBitmap *weather_icon[4];
static AppSync sync;
static uint8_t sync_buffer[4000];

int convertTemp(int c)
{
	if(useFahrenheit)
		return (c*9/5)+32;
	return c;
}

enum WeatherKey {
  WEATHER_CITY = 0,         // TUPLE_CSTRING
  WEATHER_TEMPERATURE, WEATHER_ICON,	WEATHER_MIN, WEATHER_MAX,	WEATHER_HUMIDITY,	WEATHER_PRESSURE, WEATHER_DATE,
  WEATHER_TEMPERATURE2, WEATHER_ICON2, WEATHER_MIN2, WEATHER_MAX2, WEATHER_HUMIDITY2, WEATHER_PRESSURE2, WEATHER_DATE2,
  WEATHER_TEMPERATURE3, WEATHER_ICON3, WEATHER_MIN3, WEATHER_MAX3, WEATHER_HUMIDITY3, WEATHER_PRESSURE3, WEATHER_DATE3,
  WEATHER_TEMPERATURE4, WEATHER_ICON4, WEATHER_MIN4, WEATHER_MAX4, WEATHER_HUMIDITY4, WEATHER_PRESSURE4, WEATHER_DATE4,
  WEATHER_TEMPERATURE5, WEATHER_ICON5, WEATHER_MIN5, WEATHER_MAX5, WEATHER_HUMIDITY5, WEATHER_PRESSURE5, WEATHER_DATE5,
};

static const uint8_t Wtemps[] = {
	WEATHER_TEMPERATURE,
	WEATHER_TEMPERATURE2,
	WEATHER_TEMPERATURE3,
	WEATHER_TEMPERATURE4,
	WEATHER_TEMPERATURE5,
};

static const uint8_t Wicons[] = {
	WEATHER_ICON,
	WEATHER_ICON2,
	WEATHER_ICON3,
	WEATHER_ICON4,
	WEATHER_ICON5
};

static const uint8_t Wmin[] = {
	WEATHER_MIN,
	WEATHER_MIN2,
	WEATHER_MIN3,
	WEATHER_MIN4,
	WEATHER_MIN5
};

static const uint8_t Wmax[] = {
	WEATHER_MAX,
	WEATHER_MAX2,
	WEATHER_MAX3,
	WEATHER_MAX4,
	WEATHER_MAX5
};

static const uint8_t Whumidity[] = {
	WEATHER_HUMIDITY,
	WEATHER_HUMIDITY2,
	WEATHER_HUMIDITY3,
	WEATHER_HUMIDITY4,
	WEATHER_HUMIDITY5
};


static const uint8_t Wpressure[] = {
	WEATHER_PRESSURE,
	WEATHER_PRESSURE2,
	WEATHER_PRESSURE3,
	WEATHER_PRESSURE4,
	WEATHER_PRESSURE5
};

static const uint8_t Wdate[] = {
	WEATHER_DATE,
	WEATHER_DATE2,
	WEATHER_DATE3,
	WEATHER_DATE4,
	WEATHER_DATE5
};

static const uint8_t Wicon[] = {
	WEATHER_ICON,
	WEATHER_ICON2,
	WEATHER_ICON3,
	WEATHER_ICON4,
	WEATHER_ICON5
};


static const uint32_t WEATHER_ICONS[] = {
  RESOURCE_ID_IMAGE_SUN, //0
  RESOURCE_ID_IMAGE_CLOUD, //1
  RESOURCE_ID_IMAGE_RAIN, //2
  RESOURCE_ID_IMAGE_SNOW //3
};

char* dayStr[7]={"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};

static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
}

static void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
  switch (key) {
    case WEATHER_ICON:
      bitmap_layer_set_bitmap(icon_layer, weather_icon[new_tuple->value->uint8]);
      break;
    case WEATHER_TEMPERATURE:
			snprintf(temp[0], sizeof(temp[0]), "%d\u00B0", convertTemp((int)new_tuple->value->int32));
      text_layer_set_text(temperature_layer, temp[0]);
      break;
    case WEATHER_CITY:
      text_layer_set_text(city_layer, new_tuple->value->cstring);
      break;
  }
	for(int x=0;x<FORECASTDAYS;x++)
	{	
		if(key==Wtemps[x])
		{
		}
		if(key==Wmin[x])
		{
			snprintf(min[x], sizeof(min[x]), "%d\u00B0", convertTemp((int)new_tuple->value->int32));
			text_layer_set_text(tempMin[x], min[x]);
		}
		if(key==Wmax[x])
		{
			snprintf(max[x], sizeof(max[x]), "%d\u00B0", convertTemp((int)new_tuple->value->int32));
      text_layer_set_text(tempMax[x], max[x]);
		}
		if(key==Wdate[x])
		{
			time_t t=new_tuple->value->uint32;
			struct tm *tm=localtime(&t);
			text_layer_set_text(forecastDay[x], dayStr[tm->tm_wday]);
		}
		if(key==Wicon[x])
		{
      bitmap_layer_set_bitmap(forecastIcon[x], weather_icon[new_tuple->value->uint8]);
		}
	}
}

static void send_cmd(void) {
  Tuplet value = TupletInteger(1, 1);

  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  if (iter == NULL) {
    return;
  }

  dict_write_tuplet(iter, &value);
  dict_write_end(iter);

  app_message_outbox_send();
}

static void refreshTime(struct tm *tm)
{
	if(clock_is_24h_style())
	{
		strftime(str_time, sizeof(str_time), timeFormat24, tm);
		strftime(str_sec, sizeof(str_sec), secFormat24, tm);
		strftime(str_date, sizeof(str_date), dateFormat24, tm);
	}
	else
	{
		strftime(str_time, sizeof(str_time), timeFormat, tm);
		strftime(str_sec, sizeof(str_sec), secFormat, tm);
		strftime(str_date, sizeof(str_date), dateFormat, tm);
	}
	
}

static void updateTime()
{
  text_layer_set_text(time_layer, str_time);
}

static void updateSec()
{
  text_layer_set_text(sec_layer, str_sec);
}

static void updateDate()
{
  text_layer_set_text(date_layer, str_date);
}

static void drawTime(Window* window)
{
  time_layer = text_layer_create((GRect) { .origin = { timeX, timeY }, .size = { timeW, timeH } });
	text_layer_set_text_color(time_layer, GColorWhite);
	text_layer_set_background_color(time_layer, GColorBlack);
	text_layer_set_font(time_layer, timeF); 
  text_layer_set_text_alignment(time_layer, timeA);
  text_layer_set_text(time_layer, str_time);
  layer_add_child(window_layer, text_layer_get_layer(time_layer));
}

static void drawSec(Window* window)
{
  sec_layer = text_layer_create((GRect) { .origin = { secX, secY }, .size = { secW, secH } });
	text_layer_set_text_color(sec_layer, GColorWhite);
	text_layer_set_background_color(sec_layer, GColorBlack);
	text_layer_set_font(sec_layer, secF); 
  text_layer_set_text_alignment(sec_layer, secA);
  text_layer_set_text(sec_layer, str_sec);
  layer_add_child(window_layer, text_layer_get_layer(sec_layer));
}

static void drawDate(Window* window)
{
  date_layer = text_layer_create((GRect) { .origin = { dateX, dateY }, .size = { dateW, dateH } });
	text_layer_set_text_color(date_layer, GColorWhite);
	text_layer_set_background_color(date_layer, GColorBlack);
	text_layer_set_font(date_layer, dateF); 
  text_layer_set_text_alignment(date_layer, dateA);
  text_layer_set_text(date_layer, str_date);
  layer_add_child(window_layer, text_layer_get_layer(date_layer));
}


static void handle_battery(BatteryChargeState charge_state)
{
	int pct=charge_state.charge_percent/10;
	bitmap_layer_set_bitmap(bat_layer, bat[pct]);
	layer_set_hidden(bitmap_layer_get_layer(charge_layer), !charge_state.is_charging);
}

static void showWeatherLayer()
{
	layer_set_hidden(weather_layer, false);
	layer_set_hidden(bitmap_layer_get_layer(darkside_layer), true);
	tapsec=TAPTIMER;
}

static void handle_tap(AccelAxisType axis, int32_t direction)
{
	showWeatherLayer();
}

static void tapTimer()
{
	if(tapsec)
	{
		tapsec--;
		if(!tapsec)
		{
			layer_set_hidden(weather_layer, true);
			layer_set_hidden(bitmap_layer_get_layer(darkside_layer), false);
		}
	}
}

static void weatherTimer()
{
	if(!wetsec)
	{
		showWeatherLayer();
		wetsec=WEATHERTIMER;
		APP_LOG(APP_LOG_LEVEL_INFO, "Updating weather.");
  	send_cmd();
		//weather update
	}
	wetsec--;
}

time_t yesterday(time_t t)
{
	return t-(24*60*60);
}

time_t tomorrow(time_t t)
{
	return t+(24*60*60);
}

static void updateCalendar()
{
	time_t t=time(NULL);
	struct tm *tm=localtime(&t);
	today=tm->tm_mday;
	while(tm->tm_wday)
	{
		t=yesterday(t);
		tm=localtime(&t);
	}
	for(int row=1;row<calR;row++)
	{
		for(int day=0;day<7;day++)
		{
			GColor fg=GColorBlack;
			GColor bg=GColorWhite;
			text_layer_set_text_color(calendar[day][row], fg);
			text_layer_set_background_color(calendar[day][row], bg);
			struct tm *time=localtime(&t); 
			int dayField=time->tm_mday;
			if(dayField==today)
				text_layer_set_font(calendar[day][row], calNowF);
			else
				text_layer_set_font(calendar[day][row], calDayF);
			time=localtime(&t);
			strftime(calDay[day][row-1], 3, "%e", time);
			text_layer_set_text(calendar[day][row], calDay[day][row-1]);
			t=tomorrow(t);
			time=localtime(&t);
		}
	}
}


// Called once per second
static void handle_second_tick(struct tm* tick_time, TimeUnits units_changed)
{
	refreshTime(tick_time);
	updateTime();
	updateSec();
	updateDate();
	if(today!=tick_time->tm_mday)
		updateCalendar();
  handle_battery(battery_state_service_peek());
	tapTimer();
	weatherTimer();
}

static void drawBattery(Window *window)
{
	bat_layer=bitmap_layer_create((GRect) { .origin = { batX, batY }, .size = { batW, batH } });
	bitmap_layer_set_bitmap(bat_layer, bat[0]);
  layer_add_child(window_layer, bitmap_layer_get_layer(bat_layer));
	charge_layer=bitmap_layer_create((GRect) { .origin = { chargeX, chargeY }, .size = { chargeW, chargeH } });
	bitmap_layer_set_bitmap(charge_layer, charge);
	layer_set_hidden(bitmap_layer_get_layer(charge_layer), true);
	layer_add_child(window_layer, bitmap_layer_get_layer(charge_layer));


}

static void drawBluetooth(Window *window)
{
	bt_layer=bitmap_layer_create((GRect) { .origin = { btX, btY }, .size = { btW, btH } });
	bitmap_layer_set_bitmap(bt_layer, bton);
	layer_add_child(window_layer, bitmap_layer_get_layer(bt_layer));
}

static void handle_bluetooth(bool connected) 
{
	if(connected)
		bitmap_layer_set_bitmap(bt_layer, bton);
	else
		bitmap_layer_set_bitmap(bt_layer, btoff);
}

static void drawDecoration(Window *window)
{
	back_layer=bitmap_layer_create(bounds);
	bitmap_layer_set_bitmap(back_layer, back);
	layer_add_child(window_layer, bitmap_layer_get_layer(back_layer));
	darkside_layer=bitmap_layer_create((GRect) { .origin = { wetX, wetY }, .size = { wetW, wetH } });
	bitmap_layer_set_bitmap(darkside_layer, darkside);
	layer_add_child(window_layer, bitmap_layer_get_layer(darkside_layer));

}
static void drawCalendar(Window* window)
{
	time_t t=time(NULL);
	struct tm *tm=localtime(&t);
	today=tm->tm_mday;
	while(tm->tm_wday)
	{
		t=yesterday(t);
		tm=localtime(&t);
	}
		tm=localtime(&t);
	int r=((144/7)%7)/2;
	for(int row=0;row<calR;row++)
	{
		for(int day=0;day<7;day++)
		{
			int x=(144/7)*day+calX+r;
			int y=(calH/calR)*row+calY;
			calendar[day][row]=text_layer_create((GRect) { .origin = { x, y }, .size = { calDayW, calDayH } });
			GColor fg=GColorBlack;
			GColor bg=GColorWhite;
			if(!row)
			{
				fg=GColorWhite;
				bg=GColorBlack;
			}
			
			text_layer_set_text_color(calendar[day][row], fg);
			text_layer_set_background_color(calendar[day][row], bg);
			text_layer_set_text_alignment(calendar[day][row], calA);
			if(!row)
			{
				text_layer_set_font(calendar[day][row], calHeadF); 
				text_layer_set_text(calendar[day][row], dayStr[day]);
			}
			else
			{
				struct tm *time=localtime(&t); 
				int dayField=time->tm_mday;
				if(dayField==today)
					text_layer_set_font(calendar[day][row], calNowF);
				else
					text_layer_set_font(calendar[day][row], calDayF);
				time=localtime(&t);
				strftime(calDay[day][row-1], 3, "%e", time);
				text_layer_set_text(calendar[day][row], calDay[day][row-1]);
				t=tomorrow(t);
				time=localtime(&t);
			}
			layer_add_child(window_layer, text_layer_get_layer(calendar[day][row]));
		}
	}
}

static void drawWeather(Window* window)
{
	weather_layer=layer_create((GRect) { .origin = { wetX, wetY }, .size = { wetW, wetH } });
	layer_add_child(window_layer, weather_layer);
	showWeatherLayer();
	int tempWidth=40;
	icon_layer = bitmap_layer_create(GRect(144-tempWidth-16, 0, 15, 15));
  layer_add_child(weather_layer, bitmap_layer_get_layer(icon_layer));

  temperature_layer = text_layer_create(GRect(144-tempWidth, 0, tempWidth, 16));
  text_layer_set_text_color(temperature_layer, GColorWhite);
  text_layer_set_background_color(temperature_layer, GColorClear);
  text_layer_set_font(temperature_layer, medF);
  text_layer_set_text_alignment(temperature_layer, GTextAlignmentCenter);
  layer_add_child(weather_layer, text_layer_get_layer(temperature_layer));

  city_layer = text_layer_create(GRect(0, 0, 144-tempWidth-16, 16));
  text_layer_set_text_color(city_layer, GColorWhite);
  text_layer_set_background_color(city_layer, GColorClear);
  text_layer_set_font(city_layer, tinyF);
  text_layer_set_text_alignment(city_layer, GTextAlignmentCenter);
  layer_add_child(weather_layer, text_layer_get_layer(city_layer));
	for(int x=0;x<FORECASTDAYS;x++)
	{
		int r=(144%FORECASTDAYS)/2;
		int w=(144/FORECASTDAYS);
		forecastIcon[x] = bitmap_layer_create(GRect(x*w+r, 27, w, 15));
		bitmap_layer_set_bitmap(forecastIcon[x], weather_icon[0]);
		bitmap_layer_set_alignment(forecastIcon[x], GAlignCenter);
		layer_add_child(weather_layer, bitmap_layer_get_layer(forecastIcon[x]));

		forecastDay[x]=text_layer_create(GRect(x*w+r, 14, w, 14));
		text_layer_set_text_color(forecastDay[x], GColorWhite);
		text_layer_set_background_color(forecastDay[x], GColorClear);
		text_layer_set_font(forecastDay[x], tinyF);
		text_layer_set_text_alignment(forecastDay[x], GTextAlignmentCenter);
		text_layer_set_text(forecastDay[x], dayStr[(today+x)%7]);
		layer_add_child(weather_layer, text_layer_get_layer(forecastDay[x]));

		tempMin[x]=text_layer_create(GRect(x*w+r, 54, w, 14));
		text_layer_set_text_color(tempMin[x], GColorWhite);
		text_layer_set_background_color(tempMin[x], GColorClear);
		text_layer_set_font(tempMin[x], tinyF);
		text_layer_set_text_alignment(tempMin[x], GTextAlignmentCenter);
		text_layer_set_text(tempMin[x], "---\u00B0");
		layer_add_child(weather_layer, text_layer_get_layer(tempMin[x]));

		tempMax[x]=text_layer_create(GRect(x*w+r, 42, w, 14));
		text_layer_set_text_color(tempMax[x], GColorWhite);
		text_layer_set_background_color(tempMax[x], GColorClear);
		text_layer_set_font(tempMax[x], tinyF);
		text_layer_set_text_alignment(tempMax[x], GTextAlignmentCenter);
		text_layer_set_text(tempMax[x], "---\u00B0");
		layer_add_child(weather_layer, text_layer_get_layer(tempMax[x]));




	}

  Tuplet initial_values[] = {
    TupletCString(WEATHER_CITY, "Weather Loading..."),
    TupletInteger(WEATHER_TEMPERATURE, 0),
    TupletInteger(WEATHER_ICON, (uint8_t) 1),
		TupletInteger(WEATHER_MIN, 0),
		TupletInteger(WEATHER_MAX, 0),
		TupletInteger(WEATHER_HUMIDITY, 0),
		TupletInteger(WEATHER_PRESSURE, 800),
		TupletInteger(WEATHER_DATE,0),
    TupletInteger(WEATHER_TEMPERATURE2, 0),
    TupletInteger(WEATHER_ICON2, (uint8_t) 1),
		TupletInteger(WEATHER_MIN2, 0),
		TupletInteger(WEATHER_MAX2, 0),
		TupletInteger(WEATHER_HUMIDITY2, 0),
		TupletInteger(WEATHER_PRESSURE2, 800),
		TupletInteger(WEATHER_DATE2,0),
    TupletInteger(WEATHER_TEMPERATURE3, 0),
    TupletInteger(WEATHER_ICON3, (uint8_t) 1),
		TupletInteger(WEATHER_MIN3, 0),
		TupletInteger(WEATHER_MAX3, 0),
		TupletInteger(WEATHER_HUMIDITY3, 0),
		TupletInteger(WEATHER_PRESSURE3, 800),
		TupletInteger(WEATHER_DATE3,0),
    TupletInteger(WEATHER_TEMPERATURE4, 0),
    TupletInteger(WEATHER_ICON4, (uint8_t) 1),
		TupletInteger(WEATHER_MIN4, 0),
		TupletInteger(WEATHER_MAX4, 0),
		TupletInteger(WEATHER_HUMIDITY4, 0),
		TupletInteger(WEATHER_PRESSURE4, 800),
		TupletInteger(WEATHER_DATE4,0),
    TupletInteger(WEATHER_TEMPERATURE5, 0),
    TupletInteger(WEATHER_ICON5, (uint8_t) 1),
		TupletInteger(WEATHER_MIN5, 0),
		TupletInteger(WEATHER_MAX5, 0),
		TupletInteger(WEATHER_HUMIDITY5, 0),
		TupletInteger(WEATHER_PRESSURE5, 800),
		TupletInteger(WEATHER_DATE5,0),
  };

  app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values),
      sync_tuple_changed_callback, sync_error_callback, NULL);

  send_cmd();

}

static void drawPedometer(Window *window)
{
}

static void window_load(Window *window) 
{
	drawDecoration(window);
	time_t t=time(NULL);
	refreshTime(localtime(&t));
	drawTime(window);
	drawSec(window);
	drawDate(window);
	drawWeather(window);
	drawCalendar(window);
	drawBattery(window);
	drawBluetooth(window);
	drawPedometer(window);
}

static void window_unload(Window *window) 
{
  text_layer_destroy(time_layer);
  text_layer_destroy(sec_layer);
  text_layer_destroy(date_layer);
	bitmap_layer_destroy(bat_layer);
	bitmap_layer_destroy(charge_layer);
	bitmap_layer_destroy(bt_layer);
	bitmap_layer_destroy(back_layer);
	for(int i=0;i<11;i++)
		gbitmap_destroy(bat[i]);
	gbitmap_destroy(charge);
	gbitmap_destroy(bton);
	gbitmap_destroy(btoff);
	gbitmap_destroy(back);
	gbitmap_destroy(darkside);
}

static void init(void) 
{
  const bool animated = true;
	timeF=fonts_load_custom_font(resource_get_handle((uint32_t)RESOURCE_ID_FONT_BEYOND_30_BOLD));
	secF=fonts_load_custom_font(resource_get_handle((uint32_t)RESOURCE_ID_FONT_BEYOND_24_BOLD));
	dateF=fonts_load_custom_font(resource_get_handle((uint32_t)RESOURCE_ID_FONT_BEYOND_16));
	tinyF=fonts_load_custom_font(resource_get_handle((uint32_t)RESOURCE_ID_FONT_UBUNTU_10));
	medF=fonts_load_custom_font(resource_get_handle((uint32_t)RESOURCE_ID_FONT_UBUNTU_14));
	medBF=fonts_load_custom_font(resource_get_handle((uint32_t)RESOURCE_ID_FONT_UBUNTU_14_BOLD));
	calHeadF=fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
	calDayF=fonts_get_system_font(FONT_KEY_GOTHIC_14);
	calNowF=fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);

  window = window_create();
  window_set_background_color(window, GColorBlack);
	window_layer=window_get_root_layer(window);

	back=gbitmap_create_with_resource(RESOURCE_ID_BACK);
	darkside=gbitmap_create_with_resource(RESOURCE_ID_DARKSIDE);

	const uint32_t batImages[11]={
	RESOURCE_ID_BATTERY_0, RESOURCE_ID_BATTERY_10, RESOURCE_ID_BATTERY_20, 
	RESOURCE_ID_BATTERY_30, RESOURCE_ID_BATTERY_40, RESOURCE_ID_BATTERY_50, 
	RESOURCE_ID_BATTERY_60, RESOURCE_ID_BATTERY_70, RESOURCE_ID_BATTERY_80, 
	RESOURCE_ID_BATTERY_90, RESOURCE_ID_BATTERY_100 };
	for(int i=0;i<11;i++)
		bat[i]=gbitmap_create_with_resource(batImages[i]);

	charge=gbitmap_create_with_resource(RESOURCE_ID_CHARGE);

	bton=gbitmap_create_with_resource(RESOURCE_ID_BTON);
	btoff=gbitmap_create_with_resource(RESOURCE_ID_BTOFF);

	tick_timer_service_subscribe(SECOND_UNIT, &handle_second_tick);
  battery_state_service_subscribe(&handle_battery);
  bluetooth_connection_service_subscribe(&handle_bluetooth);
	accel_tap_service_subscribe(&handle_tap);
	
  window_set_window_handlers(window, (WindowHandlers) 
	{
    .load = window_load,
    .unload = window_unload,
  });
	for(int x=0;x<4;x++)
	{
		weather_icon[x]=gbitmap_create_with_resource(WEATHER_ICONS[x]);	
	}
  const int inbound_size = 4000;
  const int outbound_size = 4000;
  app_message_open(inbound_size, outbound_size);

  window_stack_push(window, animated);
}

static void deinit(void)
{
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
	accel_tap_service_unsubscribe();
  window_destroy(window);
}

int main(void) 
{
  init();
  app_event_loop();
  deinit();
}
