
var n0 = parseInt( "0" );
var f0 = parseFloat( "0.0" );
var AG = f0;   // Moon's age
var DI = f0;   // Moon's distance in earth radii
var LA = f0;   // Moon's ecliptic latitude
var LO = f0;   // Moon's ecliptic longitude
var Phase = " ";
var MOONPCT = 100;
var Zodiac = " ";
var YEAR;
var MONTH;
var DAY;
/*var AGE;
var DST;
var FAZ;
var LAT;
var LON;
var SGN;*/
var n28 = parseInt( "28" );
var n30 = parseInt( "30" );
var n31 = parseInt( "31" );
var dim = new Array( n31, n28, n31, n30, n31, n30, n31, n31, n30, n31, n30, n31 );


function initialize()
{
    var d = new Date();

    YEAR  = d.getFullYear();
    MONTH = d.getMonth() + 1;
    DAY   = d.getDate();
}

function calculate()
{
    var year  = YEAR;//parseInt( YEAR, 10 );    
    var month = MONTH;//parseInt( MONTH, 10 );    
    var day   = DAY;//parseInt( DAY, 10 );

    if( !isdayofmonth( year, month, day ) )
    {
        alert( "Invalid date" );
        return;
    }

    moon_posit( year, month, day );
/*
    AGE = round2( AG );
    DST = round2( DI );
    FAZ = Phase;
    LAT = round2( LA );
    LON = round2( LO );
    SGN = Zodiac;*/
}

function isdayofmonth( y, m, d )
{
    if( m != 2 )
    {
        if( 1 <= d && d <= dim[m-1] )
            return true;
        else
            return false;
    }

    var feb = dim[1];
 
    if( isleapyear( y ) )
        feb += 1;                                   // is leap year

    if( 1 <= d && d <= feb )
        return true;

    return false;       
}

function isleapyear( y )
{
    var x = Math.floor( y - 4*Math.floor( y/4 ) );
    var w = Math.floor( y - 100*Math.floor( y/100 ) );
    var z = Math.floor( y - 400*Math.floor( y/400 ) );

    if( x == 0 )                           // possible leap year
    {
        if( w == 0 && z != 0 )
            return false;                  // not leap year
        else
            return true;                   // is leap year
    }

    return false;
}

function backup( n )
{
    var year = YEAR;//parseInt( YEAR, 10 );
    var month = MONTH;//parseInt( MONTH, 10 );
    var day = DAY;//parseInt( DAY, 10 );

    switch( n )
    {
    case 1:
        YEAR = year - 1;
        calculate();
        break;
    case 2:
        if( month < 2 )
        {
            MONTH = 12;
            YEAR = year - 1;
        }
        else
            MONTH = month - 1;
        calculate();
        break;
    case 3:
        if( day < 2 )
        {
            if( month < 2 )
            {
                MONTH = 12;
                YEAR = year - 1;
            }
            else
                MONTH = month - 1;
            
            month = parseInt( MONTH, 10 );
            if( month == 2 && isleapyear( year ) )
                DAY = 29;
            else 
                DAY = dim[month-1];
        }
        else
            DAY = day - 1;
        calculate();
        break;
    }
}

function advance( n )
{
    var year = parseInt( YEAR, 10 );
    var month = parseInt( MONTH, 10 );
    var day = parseInt( DAY, 10 );

    switch( n )
    {
    case 1:
        YEAR = year + 1;
        calculate();
        break;
    case 2:
        if( month < 12 )
            MONTH = month + 1;
        else
        {
            MONTH = 1;
            YEAR = year + 1;
        }
        calculate();
        break;
    case 3:
        if( isdayofmonth( year, month, day + 1 ) )
            DAY = day + 1;
        else
        {
            if( month < 12 )
                MONTH = month + 1;
            else
            {
                MONTH = 1;
                YEAR = year + 1;
            }

            DAY = 1;
        }
        calculate();
        break;
    }
}

// compute moon position and phase
function moon_posit( Y, M, D )
{
    var YY = n0;
    var MM = n0;
    var K1 = n0; 
    var K2 = n0; 
    var K3 = n0;
    var JD = n0;
    var IP = f0;
    var DP = f0;
    var NP = f0;
    var RP = f0;
    
    // calculate the Julian date at 12h UT
    YY = Y - Math.floor( ( 12 - M ) / 10 );       
    MM = M + 9; 
    if( MM >= 12 ) MM = MM - 12;
    
    K1 = Math.floor( 365.25 * ( YY + 4712 ) );
    K2 = Math.floor( 30.6 * MM + 0.5 );
    K3 = Math.floor( Math.floor( ( YY / 100 ) + 49 ) * 0.75 ) - 38;
    
    JD = K1 + K2 + D + 59;                  // for dates in Julian calendar
    if( JD > 2299160 ) JD = JD - K3;        // for Gregorian calendar
        
    // calculate moon's age in days
    IP = normalize( ( JD - 2451550.1 ) / 29.530588853 );
    AG = IP*29.53;
    
    if(      AG <  1.84566 ) Phase = "New moon";
    else if( AG <  5.53699 ) Phase = "Evening crescent";
    else if( AG <  9.22831 ) Phase = "First quarter";
    else if( AG < 12.91963 ) Phase = "Waxing gibbous";
    else if( AG < 16.61096 ) Phase = "Full moon";
    else if( AG < 20.30228 ) Phase = "Waning gibbous";
    else if( AG < 23.99361 ) Phase = "Last quarter";
    else if( AG < 27.68493 ) Phase = "Morning crescent";
    else                     Phase = "New moon";
		var FULL=29.5/2;
		MOONPCT=Math.round(((FULL-Math.abs((AG-FULL)))/FULL)*10)*10;
    IP = IP*2*Math.PI;                      // Convert phase to radians

    // calculate moon's distance
    DP = 2*Math.PI*normalize( ( JD - 2451562.2 ) / 27.55454988 );
    DI = 60.4 - 3.3*Math.cos( DP ) - 0.6*Math.cos( 2*IP - DP ) - 0.5*Math.cos( 2*IP );

    // calculate moon's ecliptic latitude
    NP = 2*Math.PI*normalize( ( JD - 2451565.2 ) / 27.212220817 );
    LA = 5.1*Math.sin( NP );

    // calculate moon's ecliptic longitude
    RP = normalize( ( JD - 2451555.8 ) / 27.321582241 );
    LO = 360*RP + 6.3*Math.sin( DP ) + 1.3*Math.sin( 2*IP - DP ) + 0.7*Math.sin( 2*IP );

    if(      LO <  33.18 ) Zodiac = /*"\u2653 */"Pisces";
    else if( LO <  51.16 ) Zodiac = /*"\u2648 */"Aries";
    else if( LO <  93.44 ) Zodiac = /*"\u2649 */"Taurus";
    else if( LO < 119.48 ) Zodiac = /*"\u264a */"Gemini";
    else if( LO < 135.30 ) Zodiac = /*"\u264b */"Cancer";
    else if( LO < 173.34 ) Zodiac = /*"\u264c */"Leo";
    else if( LO < 224.17 ) Zodiac = /*"\u264d */"Virgo";
    else if( LO < 242.57 ) Zodiac = /*"\e264e */"Libra";
    else if( LO < 271.26 ) Zodiac = /*"\u264f */"Scorpio";
    else if( LO < 302.49 ) Zodiac = /*"\u2650 */"Sagittarius";
    else if( LO < 311.72 ) Zodiac = /*"\u2651 */"Capricorn";
    else if( LO < 348.58 ) Zodiac = /*"\u2652 */"Aquarius";
    else                   Zodiac = /*"\u2653 */"Pisces";

    // so longitude is not greater than 360!
    if ( LO > 360 ) LO = LO - 360;
}

// round to 2 decimal places    
function round2( x )
{
    return ( Math.round( 100*x )/100.0 );
}
    
// normalize values to range 0...1    
function normalize( v )
{
    v = v - Math.floor( v  ); 
    if( v < 0 )
        v = v + 1;
        
    return v;
}

// clear input
function allclear()
{
    YEAR=0;
    MONTH=0;
    DAY=0;
}

function iconFromWeatherId(weatherId) 
{
  if (weatherId < 300) {
		return 4; //thunder
	} else if (weatherId < 600) {
		if(weatherId == 511)	//freezing rain
			return 3;
    return 2;	//rain
  } else if (weatherId < 700) {
    return 3;	//snow
	} else if (weatherId < 800) {
		return 5; //mist
  } else if (weatherId < 900) {
		if (weatherId == 800)
			return 0;
    return 1;	//cloud
  } else if (weatherId < 910) {
		return 6;	//extreme
	} else if (weatherId < 957)	{
    return 0;	//sun
  } else {
		return 6;
	}
}

function fetchWeather(latitude, longitude) 
{
	initialize();
	calculate();
  var response;
	var responseW;
	var responseH;
  var req = new XMLHttpRequest();
  var reqW = new XMLHttpRequest();
  var reqH = new XMLHttpRequest();
	var data={ "ct":"None" };
	data["mn"]=MOONPCT;
	data["ps"]=Phase;
	data["zd"]=Zodiac;

	var offset = new Date().getTimezoneOffset()*60;
	//data['tz']=-(offset/60/60);
	var url="http://api.openweathermap.org/data/2.5/forecast/daily?lat=" + latitude + "&lon=" + longitude + "&cnt=5&mode=json";
	console.log("Connecting to: "+url);
	req.open('GET', url, false);
  req.onload = function(e)
	{
		console.log("req.readystate: "+req.readyState);
    if (req.readyState == 4)
		{
			console.log("req.status: "+req.status);
      if(req.status == 200)
			{
        console.log("req: "+req.responseText);
        response = JSON.parse(req.responseText);
				if (response)
				{
								data["ct"]=response.city.name;
								for(var i=0;i<response.list.length;i++)
								{
												var w = response.list[i];
												data["ic"+i]=iconFromWeatherId(w.weather[0].id);
												data["mn"+i]=Math.round(w.temp.min - 273.15);
												data["mx"+i]=Math.round(w.temp.max - 273.15);
												//		data["hm"+i]=w.humidity;
												//		data["pr"+i]=w.pressure;
												data["dt"+i]=w.dt-offset;
								}
				}
//				console.log("1:To Pebble: ("+JSON.stringify(data).length+") "+JSON.stringify(data));
//				if(data["ss"]!="")
//					Pebble.sendAppMessage(data);
			}
		}
	}
	req.send(null);
	url="http://api.openweathermap.org/data/2.5/weather?lat=" + latitude + "&lon=" + longitude + "&mode=json";
	console.log("Connecting to: "+url);
	reqW.open('GET', url, false);
	reqW.onload = function(e) 
	{
		console.log("reqW.readystate: "+reqW.readyState);
		if (reqW.readyState == 4) 
		{
			console.log("reqW.status: "+reqW.status);
    	if(reqW.status == 200) 
			{
			  console.log("reqW: "+reqW.responseText);
      	responseW = JSON.parse(reqW.responseText);
				if(responseW)
				{
								data["tm"]=Math.round(responseW.main.temp - 273.15);
								data["sr"]=responseW.sys.sunrise-offset;
								data["ss"]=responseW.sys.sunset-offset;
				}
				console.log("2:To Pebble: ("+JSON.stringify(data).length+") "+JSON.stringify(data));
				if(data["ct"]!="None")
						Pebble.sendAppMessage(data);
			}
		}
	}
	reqW.send(null);
//				console.log("To Pebble: ("+JSON.stringify(data).length+") "+JSON.stringify(data));
//				Pebble.sendAppMessage(data);
}

function locationSuccess(pos) 
{
	console.log("Have location.");
  var coordinates = pos.coords;
  fetchWeather(coordinates.latitude, coordinates.longitude);
	window.navigator.geolocation.clearWatch(locationWatcher);
}

function locationError(err) 
{
  console.warn('location error: ');
//  Pebble.sendAppMessage({"ct":"Loc Unavailable"});
}

var locationOptions = { "timeout": 15000, "maximumAge": 1800000 }; 


Pebble.addEventListener("ready",
                        function(e) {
                          console.log("connected: " + e);
                          locationWatcher = window.navigator.geolocation.watchPosition(locationSuccess, locationError, locationOptions);
                          console.log(e.type);
                        });

Pebble.addEventListener("appmessage",
                        function(e) {
                          locationWatcher = window.navigator.geolocation.watchPosition(locationSuccess, locationError, locationOptions);
                          console.log(e.type);
                          console.log(e.payload.temperature);
                          console.log("message.");
                        });

Pebble.addEventListener("webviewclosed",
                        function(e) {
                          console.log("webview closed");
                          console.log(e.type);
                          console.log(e.response);
                        });


