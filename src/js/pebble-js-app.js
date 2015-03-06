
function iconFromWeatherId(weatherId) {
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

function fetchWeather(latitude, longitude) {
  var response;
	var responseW;
	var responseH;
  var req = new XMLHttpRequest();
  var reqW = new XMLHttpRequest();
  var reqH = new XMLHttpRequest();
	var data={ "ct":"None" };
	var offset = new Date().getTimezoneOffset()*60;
	data['tz']=-(offset/60/60);

	req.open('GET', "http://api.openweathermap.org/data/2.5/forecast/daily?lat=" + latitude + "&lon=" + longitude + "&cnt=5&mode=json", false);
  req.onload = function(e)
	{
    if (req.readyState == 4)
		{
      if(req.status == 200)
			{
       // console.log("req: "+req.responseText);
        response = JSON.parse(req.responseText);
			}
		}
	}
	req.send(null);

	reqW.open('GET', "http://api.openweathermap.org/data/2.5/weather?lat=" + latitude + "&lon=" + longitude + "&mode=json", false);
	reqW.onload = function(e) 
	{
		if (reqW.readyState == 4) 
		{
    	if(reqW.status == 200) 
			{
//			  console.log("reqW: "+reqW.responseText);
      	responseW = JSON.parse(reqW.responseText);
			}
		}
	}
	reqW.send(null);

	reqH.open('GET', "http://api.openweathermap.org/data/2.5/forecast/hourly?lat=" + latitude + "&lon=" + longitude + "&mode=json", false);
	reqH.onload = function(e) 
	{
  	if (reqH.readyState == 4) 
		{
    	if(reqH.status == 200) 
			{
	//			console.log("reqH: "+reqH.responseText);
        responseH = JSON.parse(reqH.responseText);
			}
		}
	}
	reqH.send(null);

  if (response && response.list && response.list.length > 0)
	{
		data["ct"]=response.city.name;
		for(var i=0;i<response.list.length;i++)
		{
      var w = response.list[i];
			data["ic"+i]=iconFromWeatherId(w.weather[0].id);
			data["mn"+i]=Math.round(w.temp.min - 273.15);
			data["mx"+i]=Math.round(w.temp.max - 273.15);
			data["hm"+i]=w.humidity;
			data["pr"+i]=w.pressure;
			data["dt"+i]=w.dt-offset;
		}
	}
	if(responseW)
	{
		data["tm"]=Math.round(responseW.main.temp - 273.15);
		data['sr']=responseW.sys.sunrise-offset;
		data['ss']=responseW.sys.sunset-offset;
		data['mn']=100;
	}

//	console.log("To Pebble: ("+JSON.stringify(data).length+") "+JSON.stringify(data));
  Pebble.sendAppMessage(data);

}

function locationSuccess(pos) {
	//console.log("Have location.");
  var coordinates = pos.coords;
  fetchWeather(coordinates.latitude, coordinates.longitude);
}

function locationError(err) {
  console.warn('location error (' + 1 + '): ' + err.message);
  Pebble.sendAppMessage({
    "city":"Loc Unavailable"
  });
}

var locationOptions = { "timeout": 15000, "maximumAge": 60000 }; 


Pebble.addEventListener("ready",
                        function(e) {
               //           console.log("connected: " + e.ready);
                          locationWatcher = window.navigator.geolocation.watchPosition(locationSuccess, locationError, locationOptions);
                 //         console.log(e.type);
                        });

Pebble.addEventListener("appmessage",
                        function(e) {
                          window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
                       //   console.log(e.type);
                     //     console.log(e.payload.temperature);
                   //       console.log("message.");
                        });

Pebble.addEventListener("webviewclosed",
                                     function(e) {
                          //           console.log("webview closed");
                            //         console.log(e.type);
                              //       console.log(e.response);
                                     });


