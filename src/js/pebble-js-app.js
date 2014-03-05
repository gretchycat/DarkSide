
function iconFromWeatherId(weatherId) {
  if (weatherId < 600) {
    return 2;
  } else if (weatherId < 700) {
    return 3;
  } else if (weatherId > 800) {
    return 1;
  } else {
    return 0;
  }
}

function fetchWeather(latitude, longitude) {
  var response;
  var req = new XMLHttpRequest();
	req.open('GET', "http://api.openweathermap.org/data/2.5/forecast/daily?lat=" + latitude + "&lon=" + longitude + "&cnt=5&mode=json", true);
  req.onload = function(e) {
    if (req.readyState == 4) {
      if(req.status == 200) {
    //    console.log(req.responseText);
        response = JSON.parse(req.responseText);
        if (response && response.list && response.list.length > 0) {
          city = response.city.name;
					var data={
						"city":response.city.name
					}
					for(var i=0;i<response.list.length;i++)
					{
	          var w = response.list[i];
						data["temperature"+i]=Math.round(w.temp.day - 273.15);
						data["icon"+i]=iconFromWeatherId(w.weather[0].id);
						data["min"+i]=Math.round(w.temp.min - 273.15);
						data["max"+i]=Math.round(w.temp.max - 273.15);
						data["humidity"+i]=w.humidity;
						data["pressure"+i]=w.pressure;
						data["date"+i]=w.dt;
					}
          Pebble.sendAppMessage(data);
        }

      } else {
        console.log("Error");
      }
    }
  }
  req.send(null);
}

function locationSuccess(pos) {
  var coordinates = pos.coords;
  fetchWeather(coordinates.latitude, coordinates.longitude);
}

function locationError(err) {
  console.warn('location error (' + err.code + '): ' + err.message);
  Pebble.sendAppMessage({
    "city":"Loc Unavailable",
    "temperature":"N/A"
  });
}

var locationOptions = { "timeout": 15000, "maximumAge": 60000 }; 


Pebble.addEventListener("ready",
                        function(e) {
                          console.log("connect!" + e.ready);
                          locationWatcher = window.navigator.geolocation.watchPosition(locationSuccess, locationError, locationOptions);
                          console.log(e.type);
                        });

Pebble.addEventListener("appmessage",
                        function(e) {
                          window.navigator.geolocation.getCurrentPosition(locationSuccess, locationError, locationOptions);
                          console.log(e.type);
                          console.log(e.payload.temperature);
                          console.log("message!");
                        });

Pebble.addEventListener("webviewclosed",
                                     function(e) {
                                     console.log("webview closed");
                                     console.log(e.type);
                                     console.log(e.response);
                                     });


