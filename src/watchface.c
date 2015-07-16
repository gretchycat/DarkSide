#include <math.h>
#include <string.h>
#include <time.h>
#include "watchface.h"

static char str_time[10];
static char str_date[14];
static char str_sec[4];
static float tz=0;	//GMT
static int showsec=1;
static int moonpct=-1;
static GFont *timeF=NULL;
static GFont *secF=NULL;
static GFont *dateF=NULL;
static GFont *calHeadF=NULL;
static GFont *calDayF=NULL;
static GFont *calNowF=NULL;
static GFont *tinyF=NULL;
static GFont *medF=NULL;
static GFont *medBF=NULL;
static uint32_t phaseImages[11]={
									RESOURCE_ID_PHASE_0, RESOURCE_ID_PHASE_10, RESOURCE_ID_PHASE_20, 
									RESOURCE_ID_PHASE_30, RESOURCE_ID_PHASE_40, RESOURCE_ID_PHASE_50, 
									RESOURCE_ID_PHASE_60, RESOURCE_ID_PHASE_70, RESOURCE_ID_PHASE_80, 
									RESOURCE_ID_PHASE_90, RESOURCE_ID_PHASE_100 };


static int tapPage=0;
//static bool useFahrenheit=true;
static GBitmap *bat[11];
static GBitmap *charge=NULL;
static GBitmap *bton=NULL;
static GBitmap *btoff=NULL;
static GBitmap *vbon=NULL;
static GBitmap *vboff=NULL;
static GBitmap *back=NULL;
static GBitmap *splash=NULL;
static GBitmap *am=NULL;
static GBitmap *pm=NULL;
static GBitmap *riseset=NULL;
static GBitmap *compass_imageb=NULL;
static GBitmap *compass_imagew=NULL;
static GBitmap *moon=NULL;
static GBitmap *world=NULL;
static Window *window=NULL;
static Layer *window_layer=NULL;
static Layer *weather_layer=NULL;
static Layer *weather_detail_layer=NULL;
static Layer *compass_layer=NULL;
static TextLayer *update_time_layer=NULL;
static char str_updatetime[25];
static char cityField[25];
static char zodiacField[10];
static char phaseField[18];
static TextLayer *time_layer=NULL;
static TextLayer *sec_layer=NULL;
static TextLayer *date_layer=NULL;
static TextLayer *calendar[7][calR];
static char calDay[7][calR-1][3];
static BitmapLayer *vb_layer=NULL;
static BitmapLayer *bt_layer=NULL;
static BitmapLayer *bat_layer=NULL;
static BitmapLayer *charge_layer=NULL;
static BitmapLayer *back_layer=NULL;
static BitmapLayer *splash_layer=NULL;
static BitmapLayer *moon_layer=NULL;
static BitmapLayer *ampm=NULL;
static int today=0;
static int tapsec=0;
static bool isAM=true;
static bool vibeHour=true;
static int wetsec=0;//WEATHERTIMER;
static TextLayer *city_layer=NULL;
static TextLayer *phase_layer=NULL;
static TextLayer *zodiac_layer=NULL;
static TextLayer *temperature_layer=NULL;
static BitmapLayer *world_layer=NULL;
static BitmapLayer *icon_layer=NULL;
static BitmapLayer *riseset_layer=NULL;
static BitmapLayer *compass_image_layerb=NULL;
static RotBitmapLayer *compass_image_layerw=NULL;
static TextLayer *tempMin[FORECASTDAYS];
static TextLayer *tempMax[FORECASTDAYS];
static TextLayer *forecastDay[FORECASTDAYS];
static TextLayer *detailSunrise;
static TextLayer *detailSunset;
//static TextLayer *detailTemp;
//static TextLayer *detailHumidity;
//static TextLayer *detailPrecipitation;
static BitmapLayer *forecastIcon[FORECASTDAYS];
static char temp[FORECASTDAYS][7];
static char sunrise[20];
static char sunset[20];
//static char humidity[16];
//static char precip[16];
static char min[FORECASTDAYS][7];
static char max[FORECASTDAYS][7];
static GBitmap *weather_icon[4];
static AppSync sync;
static uint8_t sync_buffer[380];
static int lat=0;
static int lon=0;

enum WeatherKey {
  WEATHER_CITY = 0, WEATHER_TEMPERATURE, 
	WEATHER_ICON,	WEATHER_MIN, WEATHER_MAX,	WEATHER_DATE,
  WEATHER_ICON2, WEATHER_MIN2, WEATHER_MAX2, WEATHER_DATE2,
  WEATHER_ICON3, WEATHER_MIN3, WEATHER_MAX3, WEATHER_DATE3,
  WEATHER_ICON4, WEATHER_MIN4, WEATHER_MAX4, WEATHER_DATE4,
  WEATHER_ICON5, WEATHER_MIN5, WEATHER_MAX5, WEATHER_DATE5,
	WEATHER_SUNRISE, WEATHER_SUNSET, TIMEZONE, MOON, PHASE, ZODIAC, SHOWSEC, LAT,LON,VIBE
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
/*
static const uint8_t Whumidity[] = {
	WEATHER_HUMIDITY,
	WEATHER_HUMIDITY2,
	WEATHER_HUMIDITY3,
	WEATHER_HUMIDITY4,
	WEATHER_HUMIDITY5
};*/
/*
static const uint8_t Wpressure[] = {
	WEATHER_PRESSURE,
	WEATHER_PRESSURE2,
	WEATHER_PRESSURE3,
	WEATHER_PRESSURE4,
	WEATHER_PRESSURE5
};
*/
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
  RESOURCE_ID_IMAGE_SNOW, //3
	RESOURCE_ID_IMAGE_THUNDER, //4
	RESOURCE_ID_IMAGE_MIST, //5
	RESOURCE_ID_IMAGE_EXTREME //6
};

static char* dayStr[7]={"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};

void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
//  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
	strcpy(cityField, "Sync Error");
	wetsec=SYNC_ERROR_TIMEOUT;
}

void handle_vibe(bool vibe) 
{
	if(vibe)
		bitmap_layer_set_bitmap(vb_layer, vbon);
	else
		bitmap_layer_set_bitmap(vb_layer, vboff);
}


void sync_tuple_changed_callback(const uint32_t key, const Tuple* new_tuple, const Tuple* old_tuple, void* context) 
{
  switch (key) 
	{
    case WEATHER_ICON:
      bitmap_layer_set_bitmap(icon_layer, weather_icon[new_tuple->value->uint8]);
      break;
    case WEATHER_CITY:
			strcpy(cityField, new_tuple->value->cstring);
      text_layer_set_text(city_layer, cityField);
      break;
  	case WEATHER_TEMPERATURE:
			snprintf(temp[0], sizeof(temp[0]), "%d\u00B0", (int)new_tuple->value->int32);
			text_layer_set_text(temperature_layer, temp[0]);
			break;
		case WEATHER_SUNRISE:
			{
				time_t t=new_tuple->value->uint32;
				struct tm *tm=gmtime(&t);
				if(clock_is_24h_style())
					strftime(sunrise, sizeof(sunrise), sunriseTimeFormat24, tm);
				else
					strftime(sunrise, sizeof(sunrise), sunriseTimeFormat, tm);
				text_layer_set_text(detailSunrise, sunrise);
			}
			break;
		case WEATHER_SUNSET:
			{
				time_t t=new_tuple->value->uint32;
				struct tm *tm=gmtime(&t);
				if(clock_is_24h_style())
					strftime(sunset, sizeof(sunset), sunsetTimeFormat24, tm);
				else
					strftime(sunset, sizeof(sunset), sunsetTimeFormat, tm);
				text_layer_set_text(detailSunset, sunset);
			}
			break;
		case TIMEZONE:
			tz=new_tuple->value->int32/-3600.0;
			break;
		case MOON:
			moonpct=(int)new_tuple->value->int32;
			if(moon)
				gbitmap_destroy(moon);
			moon=gbitmap_create_with_resource(phaseImages[moonpct/10]);
			if(moon)
				bitmap_layer_set_bitmap(moon_layer, moon);
			break;
		case ZODIAC:
			strcpy(zodiacField, new_tuple->value->cstring);
      text_layer_set_text(zodiac_layer, zodiacField);
			break;
		case PHASE:
			strcpy(phaseField, new_tuple->value->cstring);
      text_layer_set_text(phase_layer, phaseField);
 		break;
		case SHOWSEC:
			showsec=new_tuple->value->uint32;
			break;
		case LAT:
			lat=new_tuple->value->int32;
			break;
		case LON:
			lon=new_tuple->value->int32;
			break;
		case VIBE:
			vibeHour=new_tuple->value->int32;
			handle_vibe(vibeHour);
			break;


	}

  		//APP_LOG(APP_LOG_LEVEL_DEBUG, "PHASE=(%d) '%s'", (int)key,phaseField);
	for(int x=0;x<FORECASTDAYS;x++)
	{	
		if(key==Wmin[x])
		{
			snprintf(min[x], sizeof(min[x]), "%d\u00B0", (int)new_tuple->value->int32);
			text_layer_set_text(tempMin[x], min[x]);
		}
		if(key==Wmax[x])
		{
			snprintf(max[x], sizeof(max[x]), "%d\u00B0", (int)new_tuple->value->int32);
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
	time_t rawtime;
	struct tm *ut;
	time(&rawtime);
	ut=localtime(&rawtime);
	if(clock_is_24h_style())
		strftime(str_updatetime, sizeof(str_updatetime), updateTimeFormat24, ut);
	else	
		strftime(str_updatetime, sizeof(str_updatetime), updateTimeFormat, ut);
	if(weather_detail_layer)
		text_layer_set_text(update_time_layer, str_updatetime);
}

void refreshTime(struct tm *tm)
{
	isAM=(tm->tm_hour<12);
	if(clock_is_24h_style())
	{
		strftime(str_time, sizeof(str_time), timeFormat24, tm);
		strftime(str_sec, sizeof(str_sec), secFormat24, tm);
		strftime(str_date, sizeof(str_date), dateFormat24, tm);		
	}
	else
	{
		strftime(str_time, sizeof(str_time), timeFormat, tm);
		strftime(str_sec, sizeof(str_sec), secFormat24, tm);
		strftime(str_date, sizeof(str_date), dateFormat, tm);
	}
	
//		APP_LOG(APP_LOG_LEVEL_INFO, "T:'%s' S:'%s' D:'%s'", str_time,str_sec, str_date);
}

void updateTime()
{
  text_layer_set_text(time_layer, str_time);
	if(isAM)
		bitmap_layer_set_bitmap(ampm, am);
	else
	  bitmap_layer_set_bitmap(ampm, pm);
	layer_set_hidden(bitmap_layer_get_layer(ampm), clock_is_24h_style());
}

void updateSec()
{
	layer_set_hidden(text_layer_get_layer(sec_layer), !showsec);
  text_layer_set_text(sec_layer, str_sec);
}

void updateDate()
{
  text_layer_set_text(date_layer, str_date);
}

void drawTime(Window* window)
{
  time_layer = text_layer_create((GRect) { .origin = { timeX, timeY }, .size = { timeW, timeH } });
	text_layer_set_text_color(time_layer, GColorWhite);
	text_layer_set_background_color(time_layer, GColorClear);
	text_layer_set_font(time_layer, timeF); 
  text_layer_set_text_alignment(time_layer, timeA);
  text_layer_set_text(time_layer, str_time);
  layer_add_child(window_layer, text_layer_get_layer(time_layer));
	ampm=bitmap_layer_create((GRect) { .origin = { secX-7, secY+2 }, .size = { 9, 5 } });
  layer_add_child(window_layer, bitmap_layer_get_layer(ampm));
}

void drawSec(Window* window)
{
  sec_layer = text_layer_create((GRect) { .origin = { secX, secY }, .size = { secW, secH } });
	text_layer_set_text_color(sec_layer, GColorWhite);
	text_layer_set_background_color(sec_layer, GColorClear);
	text_layer_set_font(sec_layer, secF); 
  text_layer_set_text_alignment(sec_layer, secA);
  text_layer_set_text(sec_layer, str_sec);
  layer_add_child(window_layer, text_layer_get_layer(sec_layer));
}

void drawDate(Window* window)
{
  date_layer = text_layer_create((GRect) { .origin = { dateX, dateY }, .size = { dateW, dateH } });
	text_layer_set_text_color(date_layer, GColorWhite);
	text_layer_set_background_color(date_layer, GColorClear);
	text_layer_set_font(date_layer, dateF); 
  text_layer_set_text_alignment(date_layer, dateA);
  text_layer_set_text(date_layer, str_date);
  layer_add_child(window_layer, text_layer_get_layer(date_layer));
}

void handle_battery(BatteryChargeState charge_state)
{
	int pct=charge_state.charge_percent/10;
	bitmap_layer_set_bitmap(bat_layer, bat[pct]);
	layer_set_hidden(bitmap_layer_get_layer(charge_layer), !charge_state.is_charging);
}

void handle_compass(CompassHeadingData d)
{
	if(tapPage==4)
	{
		rot_bitmap_layer_set_angle(compass_image_layerw, d.magnetic_heading);	
	}
}

void draw_earth() {

  #ifdef PBL_PLATFORM_APLITE 
  int time_offset=(tz)*-3600; 
  //APPLITE
  int now = (int)time(NULL) + time_offset;
  float day_of_year; // value from 0 to 1 of progress through a year
  float time_of_day; // value from 0 to 1 of progress through a day
  // approx number of leap years since epoch
  // = now / SECONDS_IN_YEAR * .24; (.24 = average rate of leap years)
  int leap_years = (int)((float)now / 131487192.0);
  // day_of_year is an estimate, but should be correct to within one day
  day_of_year = now - (((int)((float)now / 31556926.0) * 365 + leap_years) * 86400);
  day_of_year = day_of_year / 86400.0;
  time_of_day = day_of_year - (int)day_of_year;
  day_of_year = day_of_year / 365.0;
  // ##### calculate the position of the sun
  // left to right of world goes from 0 to 65536
  int sun_x = (int)((float)TRIG_MAX_ANGLE * (1.0 - time_of_day));
  // bottom to top of world goes from -32768 to 32768
  // 0.2164 is march 20, the 79th day of the year, the march equinox
  // Earth's inclination is 23.4 degrees, so sun should vary 23.4/90=.26 up and down
  int sun_y = -sin_lookup((day_of_year - 0.2164) * TRIG_MAX_ANGLE) * .26 * .25;
  // ##### draw the bitmap
	int w=144;
	int h=72;
  int x, y;
//	if(!world)
//	{		
//			world=gbitmap_create_with_resource(RESOURCE_ID_WORLD);
//	}
  for(x = 0; x < w; x++) {
    int x_angle = (int)((float)TRIG_MAX_ANGLE * (float)x / (float)(w));
    for(y = 0; y < h; y++) {
						if(y<64)
						{
										int byte = y * world->row_size_bytes + (int)(x / 8);
										int y_angle = (int)((float)TRIG_MAX_ANGLE * (float)y / (float)(h * 2)) - TRIG_MAX_ANGLE/4;
										// spherical law of cosines
										float angle = ((float)sin_lookup(sun_y)/(float)TRIG_MAX_RATIO) * ((float)sin_lookup(y_angle)/(float)TRIG_MAX_RATIO);
										angle = angle + ((float)cos_lookup(sun_y)/(float)TRIG_MAX_RATIO) * ((float)cos_lookup(y_angle)/(float)TRIG_MAX_RATIO) * ((float)cos_lookup(sun_x - x_angle)/(float)TRIG_MAX_RATIO);
										if ((angle < 0) ^ (0x1 & (((char *)world->addr)[byte] >> (x % 8)))) {
														// white pixel
														((char *)world->addr)[byte] = ((char *)world->addr)[byte] | (0x1 << (x % 8));
										} else {
														// black pixel
														((char *)world->addr)[byte] = ((char *)world->addr)[byte] & ~(0x1 << (x % 8));
										}
			}
    }
  }
//  layer_mark_dirty(bitmap_layer_get_layer(world_layer));
  #else
  //BASALT
  layer_mark_dirty(bitmap_layer_get_layer(world_layer));
  #endif
  //
}

#ifdef PBL_COLOR
///*
void getARGB(int *a, int *r, int *g, int *b, uint8_t c)
{
	*a=(0b11000000&c)/64*256/4;	
	*r=(0b00110000&c)/16*256/4;	
	*g=(0b00001100&c)/4*256/4;	
	*b=(0b00000011&c)*256/4;	
}

static uint8_t lighten(uint8_t c)
{
	int a,r,g,b;
	getARGB(&a, &r, &g, &b, c);
	float mul=1.5;
	r=(int)(r*mul);g=(int)(g*mul);b=(int)(b*mul);
	if(r>255)
		r=255;
	if(g>255)
		g=255;
	if(b>255)
		b=255;
	return GColorFromRGBA(r, g, b, a).argb;
}

static uint8_t darken(uint8_t c)
{
	int a,r,g,b;
	getARGB(&a, &r, &g, &b, c);
	float mul=.45;
	r=(int)(r*mul);g=(int)(g*mul);b=(int)(b*mul);
	if(r>255)
		r=255;
	if(g>255)
		g=255;
	if(b>255)
		b=255;
	return GColorFromRGBA(r, g, b, a).argb;

}


void draw_earth_color(struct Layer *layer, GContext *ctx) {
  int time_offset=0;//(tz+6)*-3600; 
    //if (redraw_counter < REDRAW_INTERVAL) {
    // GBitmap *fb = graphics_capture_frame_buffer_format(ctx, GBitmapFormat8Bit);// 
    // graphics_draw_bitmap_in_rect(ctx, fb,GRect(0, 0, 144, 72));
    //}else {
    //redraw_counter = 0;
//		if(!world)
//		{
//			world=gbitmap_create_with_resource(RESOURCE_ID_WORLD);
//		}
				graphics_draw_bitmap_in_rect(ctx, world, GRect(0, 0, wetW, wetH));
        int now2 = (int)time(NULL) + time_offset;
  
        float day_of_year; // value from 0 to 1 of progress through a year
        float time_of_day; // value from 0 to 1 of progress through a day
        // approx number of leap years since epoch
        // = now / SECONDS_IN_YEAR * .24; (.24 = average rate of leap years)
        int leap_years = (int)((float)now2 / 131487192.0);
        // day_of_year is an estimate, but should be correct to within one day
        day_of_year = now2 - (((int)((float)now2 / 31556926.0) * 365 + leap_years) * 86400);
        day_of_year = day_of_year / 86400.0;
        time_of_day = day_of_year - (int)day_of_year;
        day_of_year = day_of_year / 365.0;
        // ##### calculate the position of the sun
        // left to right of world goes from 0 to 65536
        int sun_x = (int)((float)TRIG_MAX_ANGLE * (1.0 - time_of_day));
        // bottom to top of world goes from -32768 to 32768
        // 0.2164 is march 20, the 79th day of the year, the march equinox
        // Earth's inclination is 23.4 degrees, so sun should vary 23.4/90=.26 up and down
        int sun_y = -sin_lookup((day_of_year - 0.2164) * TRIG_MAX_ANGLE) * .26 * .25;
        // ##### draw the bitmap
        int x, y;

        #define WINDOW_WIDTH 144 
  
        GBitmap *fb = graphics_capture_frame_buffer_format(ctx, GBitmapFormat8Bit);//
        uint8_t *fb_data = gbitmap_get_data(fb);   
        uint8_t (*fb_matrix)[WINDOW_WIDTH] = (uint8_t (*)[WINDOW_WIDTH]) fb_data;
  
        for(x = 0; x < 144; x++) {
          int x_angle = (int)((float)TRIG_MAX_ANGLE * (float)x / (float)(144));
           for(y = 0; y < 72; y++) {
              int y_angle = (int)((float)TRIG_MAX_ANGLE * (float)y / (float)(72 * 2)) - TRIG_MAX_ANGLE/4;
              // spherical law of cosines
              float angle = ((float)sin_lookup(sun_y)/(float)TRIG_MAX_RATIO) * ((float)sin_lookup(y_angle)/(float)TRIG_MAX_RATIO);
              angle = angle + ((float)cos_lookup(sun_y)/(float)TRIG_MAX_RATIO) * ((float)cos_lookup(y_angle)/(float)TRIG_MAX_RATIO) * ((float)cos_lookup(sun_x - x_angle)/(float)TRIG_MAX_RATIO);
              if (angle > 0) { //^ (0x1 & (((char *)world_bitmap->addr)[byte] >> (x % 8)))) {
                // white pixel
              } else {
                // black pixel
								if(y<64)
                	fb_matrix[y+wetY][x] = darken(fb_matrix[y+wetY][x]);//background_matrix[y][x];
              }
          }
        }
        graphics_release_frame_buffer(ctx,fb);
      //}
}//*/
#endif


void showTapPage(int pg)
{
	int l0=true;
	int l1=true;
	int l2=true;
	int l3=true;
	int l4=true;
	if(pg!=4)
		compass_service_unsubscribe();
	if((pg!=0)&&(pg!=4))
	{
		if(splash)
		{
			gbitmap_destroy(splash);
			splash=NULL;
		}
	}
	if(pg!=3)
	{
		if(world)
		{
			gbitmap_destroy(world);
			world=NULL;
		}
	}
	switch(pg)
	{
		case 1:
		{//set city text field to cityField
			l1=false;
	  	text_layer_set_text(city_layer, cityField);
		};break;
		case 2:
		{//set city text to str_updatetime
			if(!weather_detail_layer)
			{
				l1=false;
 	 			text_layer_set_text(city_layer, str_updatetime);
			}
			else
				l2=false;
		};break;
		case 3:
		{
			world=gbitmap_create_with_resource(RESOURCE_ID_WORLD);
			l3=false;
			draw_earth();
			bitmap_layer_set_bitmap(world_layer, world);
		};break;
		case 4:
		{
			compass_service_subscribe(&handle_compass);
			l4=false;
		};
		case 0:
		default:
		{
			if(!splash)
				splash=gbitmap_create_with_resource(RESOURCE_ID_SPLASH);
			bitmap_layer_set_bitmap(splash_layer, splash);
			l0=false;
		};break;
	}
	layer_set_hidden(bitmap_layer_get_layer(splash_layer), l0);
	layer_set_hidden(weather_layer, l1);
	layer_set_hidden(weather_detail_layer, l2);
	layer_set_hidden(bitmap_layer_get_layer(world_layer), l3);
	layer_set_hidden(compass_layer, l4);
	tapsec=TAPTIMER;
}

void handle_tap(AccelAxisType axis, int32_t direction)
{
	tapPage++;
//	if(tapPage>4)
		tapPage%=5;
	showTapPage(tapPage);
}

void tapTimer()
{
	if(tapsec)
	{
		tapsec--;
		if(!tapsec)
		{
			tapPage=0;
			showTapPage(0);
		}
	}
}

void weatherTimer()
{
	if(wetsec==0)
	{
		tapPage=1;
		showTapPage(tapPage);
		wetsec=WEATHERTIMER;
  	app_message_outbox_send();
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

void updateCalendar()
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
			struct tm *time=localtime(&t); 
			int dayField=time->tm_mday;
			GColor fg=GColorBlack;
			GColor bg=GColorWhite;
			GColor hfg=GColorWhite;
#ifdef PBL_COLOR
			GColor hbg=GColorDarkCandyAppleRed;
#else
			GColor hbg=GColorBlack;
#endif
			if(!row)
			{
				fg=GColorWhite;
				bg=GColorBlack;
			}
			if(dayField==today)
			{
				text_layer_set_font(calendar[day][row], calNowF);
				text_layer_set_text_color(calendar[day][row], hfg);
				text_layer_set_background_color(calendar[day][row], hbg);
			}
			else
			{
				text_layer_set_font(calendar[day][row], calDayF);
				text_layer_set_text_color(calendar[day][row], fg);
				text_layer_set_background_color(calendar[day][row], bg);
			}
	
			time=localtime(&t);
			strftime(calDay[day][row-1], 3, "%e", time);
			text_layer_set_text(calendar[day][row], calDay[day][row-1]);
			t=tomorrow(t);
			time=localtime(&t);
		}
	}
}

void handle_second_tick(struct tm* tick_time, TimeUnits units_changed)
{
	refreshTime(tick_time);
	updateTime();
	updateSec();
	updateDate();
	if(today!=tick_time->tm_mday)
		updateCalendar();
	tapTimer();
	if((tick_time->tm_sec==0)&&(tick_time->tm_min==0))
		if(vibeHour)
			vibes_short_pulse();
	weatherTimer();
}

void drawBattery(Window *window)
{
	bat_layer=bitmap_layer_create((GRect) { .origin = { batX, batY }, .size = { batW, batH } });
	bitmap_layer_set_bitmap(bat_layer, bat[0]);
  layer_add_child(window_layer, bitmap_layer_get_layer(bat_layer));
	charge_layer=bitmap_layer_create((GRect) { .origin = { chargeX, chargeY }, .size = { chargeW, chargeH } });
	bitmap_layer_set_bitmap(charge_layer, charge);
	layer_set_hidden(bitmap_layer_get_layer(charge_layer), true);
	layer_add_child(window_layer, bitmap_layer_get_layer(charge_layer));
}

void drawBluetooth(Window *window)
{
	bt_layer=bitmap_layer_create((GRect) { .origin = { btX, btY }, .size = { btW, btH } });
	bitmap_layer_set_bitmap(bt_layer, bton);
	layer_add_child(window_layer, bitmap_layer_get_layer(bt_layer));
}

void drawVibe(Window *window)
{
	vb_layer=bitmap_layer_create((GRect) { .origin = { vbX, vbY }, .size = { vbW, vbH } });
	bitmap_layer_set_bitmap(vb_layer, vbon);
	layer_add_child(window_layer, bitmap_layer_get_layer(vb_layer));
}

void handle_bluetooth(bool connected) 
{
	if(connected)
		bitmap_layer_set_bitmap(bt_layer, bton);
	else
		bitmap_layer_set_bitmap(bt_layer, btoff);
}

void drawDecoration(Window *window)
{
	back_layer=bitmap_layer_create(bounds);
	if(back)
	{
		bitmap_layer_set_bitmap(back_layer, back);
		layer_add_child(window_layer, bitmap_layer_get_layer(back_layer));
	}
	splash_layer=bitmap_layer_create((GRect) { .origin = { wetX, wetY }, .size = { wetW, wetH } });
	if(!splash)
		splash=gbitmap_create_with_resource(RESOURCE_ID_SPLASH);
	bitmap_layer_set_bitmap(splash_layer, splash);
	layer_add_child(window_layer, bitmap_layer_get_layer(splash_layer));
}

void drawWorld(Window *window)
{
	world_layer=bitmap_layer_create((GRect) { .origin = { wetX, wetY }, .size = { wetW, wetH } });
	layer_add_child(window_layer, bitmap_layer_get_layer(world_layer));
#ifdef PBL_COLOR
  layer_set_update_proc(bitmap_layer_get_layer(world_layer), draw_earth_color);
#endif
}

void drawCalendar(Window* window)
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
			GColor hfg=GColorWhite;
#ifdef PBL_COLOR
			GColor hbg=GColorDarkCandyAppleRed;
#else
			GColor hbg=GColorBlack;
#endif
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
				{
					text_layer_set_font(calendar[day][row], calNowF);
					text_layer_set_text_color(calendar[day][row], hfg);
					text_layer_set_background_color(calendar[day][row], hbg);
				}
				else
				{
					text_layer_set_font(calendar[day][row], calDayF);
					text_layer_set_text_color(calendar[day][row], fg);
					text_layer_set_background_color(calendar[day][row], bg);
				}
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

void drawWeather(Window* window)
{
	weather_layer=layer_create((GRect) { .origin = { wetX, wetY }, .size = { wetW, wetH } });
	layer_add_child(window_layer, weather_layer);
	//showWeatherLayer();
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
}

void weatherSync()
{
	time_t t=time(NULL);
  Tuplet initial_values[] = {
    TupletCString(WEATHER_CITY, "Loading..."),
    TupletInteger(WEATHER_TEMPERATURE, 0),
    TupletInteger(WEATHER_ICON, (uint8_t) 1),
		TupletInteger(WEATHER_MIN, 0),
		TupletInteger(WEATHER_MAX, 0),
		//TupletInteger(WEATHER_HUMIDITY, 0),
		//TupletInteger(WEATHER_PRESSURE, 800),
		TupletInteger(WEATHER_DATE,t),
    TupletInteger(WEATHER_ICON2, (uint8_t) 1),
		TupletInteger(WEATHER_MIN2, 0),
		TupletInteger(WEATHER_MAX2, 0),
		//TupletInteger(WEATHER_HUMIDITY2, 0),
		//TupletInteger(WEATHER_PRESSURE2, 800),
		TupletInteger(WEATHER_DATE2,t+(secInDay)),
    TupletInteger(WEATHER_ICON3, (uint8_t) 1),
		TupletInteger(WEATHER_MIN3, 0),
		TupletInteger(WEATHER_MAX3, 0),
		//TupletInteger(WEATHER_HUMIDITY3, 0),
		//TupletInteger(WEATHER_PRESSURE3, 800),
		TupletInteger(WEATHER_DATE3,t+(secInDay*2)),
    TupletInteger(WEATHER_ICON4, (uint8_t) 1),
		TupletInteger(WEATHER_MIN4, 0),
		TupletInteger(WEATHER_MAX4, 0),
		//TupletInteger(WEATHER_HUMIDITY4, 0),
		//TupletInteger(WEATHER_PRESSURE4, 800),
		TupletInteger(WEATHER_DATE4,t+(secInDay*3)),
    TupletInteger(WEATHER_ICON5, (uint8_t) 1),
		TupletInteger(WEATHER_MIN5, 0),
		TupletInteger(WEATHER_MAX5, 0),
		//TupletInteger(WEATHER_HUMIDITY5, 0),
		//TupletInteger(WEATHER_PRESSURE5, 800),
		TupletInteger(WEATHER_DATE5, t+(secInDay*4)),
		TupletInteger(WEATHER_SUNRISE, 0),
		TupletInteger(WEATHER_SUNSET, 0),
		TupletInteger(TIMEZONE, 0),
		TupletInteger(MOON, -1),
		TupletCString(PHASE, "-"),
		TupletCString(ZODIAC, "-"),
		TupletInteger(SHOWSEC, 1),
		TupletInteger(LAT, 0),
		TupletInteger(LON, 0),
		TupletInteger(VIBE, 1)
		
  };

  app_sync_init(&sync, sync_buffer, sizeof(sync_buffer), initial_values, ARRAY_LENGTH(initial_values), sync_tuple_changed_callback, sync_error_callback, NULL);
	//send_cmd();
}

void drawWeatherDetail(Window* window)
{
	weather_detail_layer=layer_create((GRect) { .origin = { wetX, wetY }, .size = { wetW, wetH } });
	layer_add_child(window_layer, weather_detail_layer);
  update_time_layer = text_layer_create(GRect(0, 0, 144, 16));
  text_layer_set_text_color(update_time_layer, GColorWhite);
  text_layer_set_background_color(update_time_layer, GColorClear);
  text_layer_set_font(update_time_layer, tinyF);
  text_layer_set_text_alignment(update_time_layer, GTextAlignmentCenter);
  layer_add_child(weather_detail_layer, text_layer_get_layer(update_time_layer));	

	riseset_layer=bitmap_layer_create((GRect) { .origin = { 144/2-29-15, 12 }, .size = { 29, 20 } });
	#ifdef PBL_COLOR
		bitmap_layer_set_compositing_mode(riseset_layer, GCompOpSet);
	#else
		bitmap_layer_set_compositing_mode(riseset_layer, GCompOpOr);
	#endif
	bitmap_layer_set_bitmap(riseset_layer, riseset);
	layer_add_child(weather_detail_layer, bitmap_layer_get_layer(riseset_layer));	

	detailSunrise = text_layer_create(GRect(144/2-15, 14, 144/2, 16));
  text_layer_set_text_color(detailSunrise, GColorWhite);
  text_layer_set_background_color(detailSunrise, GColorClear);
  text_layer_set_font(detailSunrise, tinyF);
  text_layer_set_text_alignment(detailSunrise, GTextAlignmentLeft);
  layer_add_child(weather_detail_layer, text_layer_get_layer(detailSunrise));	

  detailSunset = text_layer_create(GRect(144/2-15, 24, 144/2, 16));
  text_layer_set_text_color(detailSunset, GColorWhite);
  text_layer_set_background_color(detailSunset, GColorClear);
  text_layer_set_font(detailSunset, tinyF);
  text_layer_set_text_alignment(detailSunset, GTextAlignmentLeft);
  layer_add_child(weather_detail_layer, text_layer_get_layer(detailSunset));
	moon_layer=bitmap_layer_create((GRect) { .origin = { 4, 35 }, .size = { ms, ms } });
	layer_add_child(weather_detail_layer, bitmap_layer_get_layer(moon_layer));	

  phase_layer = text_layer_create(GRect(4+ms, 35, 144-ms-4, 16));
  text_layer_set_text_color(phase_layer, GColorWhite);
  text_layer_set_background_color(phase_layer, GColorClear);
  text_layer_set_font(phase_layer, tinyF);
  text_layer_set_text_alignment(phase_layer, GTextAlignmentCenter);
  layer_add_child(weather_detail_layer, text_layer_get_layer(phase_layer));

  zodiac_layer = text_layer_create(GRect(4+ms, 35+14, 144-ms-4, 16));
  text_layer_set_text_color(zodiac_layer, GColorWhite);
  text_layer_set_background_color(zodiac_layer, GColorClear);
  text_layer_set_font(zodiac_layer, tinyF);
  text_layer_set_text_alignment(zodiac_layer, GTextAlignmentCenter);
  layer_add_child(weather_detail_layer, text_layer_get_layer(zodiac_layer));

}

void drawCompass(Window* window)
{
	int compassW=91; //sqrt((64*64)+(64*64));
	int compassH=compassW;
	int compassX=((144/2)-(compassW/2));
	int compassY=compassX+17;
	compass_layer=layer_create((GRect) { .origin = { compassX, compassY }, .size = { compassW, compassH } });
	layer_add_child(window_layer, compass_layer);
	compass_image_layerw=rot_bitmap_layer_create(compass_imagew);
	compass_image_layerb=bitmap_layer_create((GRect) { .origin = { 0, 0 }, .size = { compassW, compassH } });
	#ifdef PBL_COLOR
	{
		rot_bitmap_set_compositing_mode(compass_image_layerw, GCompOpSet);
	}
	#else
	{
		if(compass_imageb)
		{
			bitmap_layer_set_compositing_mode((BitmapLayer*)(compass_image_layerb), GCompOpAnd);
			bitmap_layer_set_bitmap(compass_image_layerb, compass_imageb);
			layer_add_child(compass_layer, (Layer*)(compass_image_layerb));
		}
		rot_bitmap_set_compositing_mode(compass_image_layerw, GCompOpOr);
	}
	#endif
	layer_add_child(compass_layer, (Layer*)(compass_image_layerw));
}

void window_load(Window *window) 
{
	drawDecoration(window);
	time_t t=time(NULL);
	refreshTime(localtime(&t));
	drawTime(window);
	drawSec(window);
	drawDate(window);
	drawWeatherDetail(window);
	drawWeather(window);
	drawCalendar(window);
	drawBattery(window);
	drawBluetooth(window);
	drawVibe(window);
	drawCompass(window);
	drawWorld(window);
//	drawPedometer(window);

	showTapPage(0);
  handle_battery(battery_state_service_peek());
	handle_bluetooth(bluetooth_connection_service_peek());
}

void window_unload(Window *window) 
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
	gbitmap_destroy(splash);
}

void init(void) 
{
  const bool animated = true;
	timeF=fonts_load_custom_font((ResHandle)resource_get_handle((uint32_t)RESOURCE_ID_FONT_BEYOND_30_BOLD));
	secF=fonts_load_custom_font((ResHandle)resource_get_handle((uint32_t)RESOURCE_ID_FONT_BEYOND_24_BOLD));
	dateF=fonts_load_custom_font((ResHandle)resource_get_handle((uint32_t)RESOURCE_ID_FONT_BEYOND_16));
	tinyF=fonts_load_custom_font((ResHandle)resource_get_handle((uint32_t)RESOURCE_ID_FONT_UBUNTU_10));
	medF=fonts_load_custom_font((ResHandle)resource_get_handle((uint32_t)RESOURCE_ID_FONT_UBUNTU_14));
	medBF=medF;//fonts_load_custom_font((ResHandle)resource_get_handle((uint32_t)RESOURCE_ID_FONT_UBUNTU_14_BOLD));
	calDayF=fonts_get_system_font(FONT_KEY_GOTHIC_14);
	calHeadF=calDayF;//fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
	calNowF=calHeadF;

  window = window_create();
  window_set_background_color(window, GColorBlack);
	window_layer=window_get_root_layer(window);
	{
//					back=gbitmap_create_with_resource(RESOURCE_ID_BACK);
					//splash=gbitmap_create_with_resource(RESOURCE_ID_SPLASH);

					uint32_t batImages[11]={
									RESOURCE_ID_BATTERY_0, RESOURCE_ID_BATTERY_10, RESOURCE_ID_BATTERY_20, 
									RESOURCE_ID_BATTERY_30, RESOURCE_ID_BATTERY_40, RESOURCE_ID_BATTERY_50, 
									RESOURCE_ID_BATTERY_60, RESOURCE_ID_BATTERY_70, RESOURCE_ID_BATTERY_80, 
									RESOURCE_ID_BATTERY_90, RESOURCE_ID_BATTERY_100 };
					for(int i=0;i<11;i++)
									bat[i]=gbitmap_create_with_resource(batImages[i]);
					charge=gbitmap_create_with_resource(RESOURCE_ID_CHARGE);
					compass_imageb=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_COMPASS_BACK);
					compass_imagew=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_COMPASS);
					bton=gbitmap_create_with_resource(RESOURCE_ID_BTON);
					btoff=gbitmap_create_with_resource(RESOURCE_ID_BTOFF);
					vbon=gbitmap_create_with_resource(RESOURCE_ID_VIBEON);
					vboff=gbitmap_create_with_resource(RESOURCE_ID_VIBEOFF);
					am=gbitmap_create_with_resource(RESOURCE_ID_AM);
					pm=gbitmap_create_with_resource(RESOURCE_ID_PM);
					riseset=gbitmap_create_with_resource(RESOURCE_ID_IMAGE_RISESET);
	}
	tick_timer_service_subscribe(SECOND_UNIT, &handle_second_tick);
  battery_state_service_subscribe(&handle_battery);
  bluetooth_connection_service_subscribe(&handle_bluetooth);
	accel_tap_service_subscribe(&handle_tap);
	
	for(int x=0;x<4;x++)
	{
		weather_icon[x]=gbitmap_create_with_resource(WEATHER_ICONS[x]);	
	}
  window_set_window_handlers(window, (WindowHandlers) 
	{
    .load = window_load,
    .unload = window_unload,
  });
  const int inbound_size = sizeof(sync_buffer);
  const int outbound_size = sizeof(sync_buffer);
  app_message_open(inbound_size, outbound_size);

  window_stack_push(window, animated);
	weatherSync();
}

void deinit(void)
{
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
	accel_tap_service_unsubscribe();;
  window_destroy(window);
}

int main(void) 
{
  init();
  app_event_loop();
  deinit();
}

