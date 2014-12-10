var HOST = 'http://pebble-netatmo.appspot.com';

var 
  access_token, 
  expires_in, 
  refresh_token,
  main_device,
  units_metric;

function store(){
  console.log("store " + units_metric);

  if(access_token)
    localStorage.setItem("access_token", access_token);
  else 
    localStorage.removeItem("access_token");

  if(expires_in)
    localStorage.setItem("expires_in", expires_in);
  else 
    localStorage.removeItem("expires_in");

  if(refresh_token)
    localStorage.setItem("refresh_token", refresh_token);
  else 
    localStorage.removeItem("refresh_token");

  if(main_device)
    localStorage.setItem("main_device", main_device);
  else 
    localStorage.removeItem("main_device");

  localStorage.setItem("units_metric", units_metric);
}

function read(){
  access_token  = localStorage.getItem("access_token");
  expires_in    = localStorage.getItem("expires_in");
  refresh_token = localStorage.getItem("refresh_token");
  main_device   = localStorage.getItem("main_device");
  units_metric  = localStorage.getItem("units_metric");
  if(units_metric == null)
    units_metric = 1;

  console.log("read " + access_token + " "+ expires_in + " "+ refresh_token + " " + main_device + " " + units_metric);
}

function senderror(error){
  console.log("error : " + error);

  var data = { 'error' : error };
  MessageQueue.sendAppMessage(data, 
  function(e) {
    console.log('error sent');
  },
  function(e) {
    console.log('error NOT sent');
  });
}

function disconnect() {
  console.log("Disconnection");
  access_token = undefined;
  expires_in = 0; 
  refresh_token = undefined; 
  store();
}

Pebble.addEventListener("ready", function(e) {
  console.log("NetAtmo js, ready to go!");  
  read();
  fetchData();
});

Pebble.addEventListener('showConfiguration', function(e) {
  read();
  var uri = HOST + '/config.html';
  if(refresh_token !== null){
    uri += "?refresh_token=" + encodeURIComponent(refresh_token);
  }
  console.log('showConfiguration ' + uri);
  Pebble.openURL(uri);
});

Pebble.addEventListener('webviewclosed', function(e) {
  console.log('webviewclosed ' + e.response);
  var s = decodeURIComponent(e.response);
  if (s) {
    var c = JSON.parse(s);
    if(c.access_token) {
      main_device = c.main_device;
      units_metric = c.units;
      updateToken(c);
      store();
      fetchData();
    }
    else {
      disconnect();
    }
  }
});

function fetchMeasures(device, modules, index){
  console.log("fetchMeasures " + device._id);
  if(modules)
    console.log("fetchMeasures module " + modules[index]._id );

  if (!access_token){
    senderror("Please sign in");
    return;
  }

  var n = new Date;
  if (expires_in < n.valueOf()) 
    return renewToken(function() { fetchMeasures(module, station) });

  var UTCseconds = (n.getTime() + n.getTimezoneOffset()*60*1000)/1000 - 24 * 3600;

  var req = new XMLHttpRequest();
  var url = 'https://api.netatmo.net/api/getmeasure?access_token=' + encodeURIComponent(access_token) + '&device_id=' + encodeURIComponent(device._id) + '&type=Temperature,CO2,Humidity,Pressure,Noise,sum_rain&scale=1hour&limit=24&optimize=true&date_begin=' + UTCseconds;
  if(modules)
    url += '&module_id=' + encodeURIComponent(modules[index]._id);
  console.log(url);
  sendRequest(url, "GET", 
  function(e) {
    // console.log("fetchMeasures " + e);

    var result = JSON.parse(e);

    var dashboard_data = modules ? modules[index].dashboard_data : device.dashboard_data;

    var array = [];
    fillDashBoardData(
        array,
        modules ? moduleType(modules[index].type) : moduleType(device.type),
        removeDiacritics(modules ? modules[index].module_name : device.module_name),
        'Temperature' in dashboard_data ? dashboard_data.Temperature : 0,
        'min_temp'    in dashboard_data ? dashboard_data.min_temp    : 0,
        'max_temp'    in dashboard_data ? dashboard_data.max_temp    : 0,
        'Humidity'    in dashboard_data ? dashboard_data.Humidity    : 0,
        'Noise'       in dashboard_data ? dashboard_data.Noise       : 0,
        'CO2'         in dashboard_data ? dashboard_data.CO2         : 0,
        'Pressure'    in dashboard_data ? dashboard_data.Pressure    : 0,
        'Rain'        in dashboard_data ? dashboard_data.Rain        : 0,
        result.body[0].value);

    MessageQueue.sendAppMessage({'dashboard_data' : array }, 
      function(e) {
        console.log('data sent');
      },
      function(e) {
        console.log('data NOT sent');
      });

  },
  function(e) {
    if(e.responseText){
      console.log("fetchMeasures Error " + e.responseText)
      var response = JSON.parse(e.responseText);
      if(response.error && response.error.code == 2){
        // Invalid access token
        // disconnect();
        // senderror("Please sign in");
      }
    }
    else if(e.status == 500){
      // Invalid deviceID ???
      // senderror("Please sign in");
    }
    console.log("fetchMeasures Error")
  });
}

function fetchData(){
  console.log("fetchData " + access_token);
  if (!access_token){
    senderror("Please sign in");
    return;
  }

  var n = new Date;
  console.log("fetchData " + (expires_in < n.valueOf()) + " " + expires_in + " " + n.valueOf());
  if (expires_in < n.valueOf()) 
    return renewToken(function() { fetchData() });

  var req = new XMLHttpRequest();
  var url = 'https://api.netatmo.net/api/devicelist?access_token=' + encodeURIComponent(access_token) + '&device_id=' + encodeURIComponent(main_device);
  console.log(url);
  sendRequest(url, "GET", 
  function(e) {
    MessageQueue.sendAppMessage({'reset_data':0}, 
      function(e) {
        console.log('reset sent');
      },
      function(e) {
        console.log('reset NOT sent');
      });

    var result = JSON.parse(e);
    if(result.body.devices.length > 0){
      fetchMeasures(result.body.devices[0]);
    }
    for(var i=0; i<result.body.modules.length; i++){
      fetchMeasures(result.body.devices[0], result.body.modules, i);
    }
  },
  function(e) {
    if(e.responseText){
      var response = JSON.parse(e.responseText);
      if(response.error && response.error.code == 2){
        // Invalid access token
        disconnect();
        senderror("Please sign in");
      }
    }
    else if(e.status == 500){
      // Invalid deviceID ???
      senderror("Please sign in");
    }

    console.log("fetchData Error")
  });
}

function moduleType(type){
  if(type == "NAMain")    return 0;
  if(type == "NAModule1") return 1;
  if(type == "NAModule4") return 2;
  if(type == "NAModule3") return 3;
  if(type == "NAPlug")    return 4;
  if(type == "NATherm1")  return 5;
  return 0;
}

function fillDashBoardData(arr, type, name, temp, temp_min, temp_max, humidity, noise, co2, pressure, rain, measures){
  pushUInt16(arr, type);

  var i = 0;
  for(; i<name.length && i<19; i++)
    pushUInt8(arr, name.charCodeAt(i));
  
  for(; i<20; i++)
    pushUInt8(arr, 0);

  pushUInt16(arr, (units_metric == 0 ? 32 + 1.8 * temp : temp) * 10);
  pushUInt16(arr, (units_metric == 0 ? 32 + 1.8 * temp_min : temp_min) * 10);
  pushUInt16(arr, (units_metric == 0 ? 32 + 1.8 * temp_max : temp_max) * 10);
  pushUInt8(arr,  humidity);         
  pushUInt8(arr,  noise);
  pushUInt16(arr, co2);
  pushUInt16(arr, pressure*10);
  pushUInt16(arr, rain*1000);

  // Temperature
  for(i=0; i<24; i++){
    var tmp = measures[i][0] != null ? measures[i][0] : 0;
    tmp = units_metric == 0 ? 32 + 1.8 * tmp : tmp;
    pushUInt16(arr, tmp * 10);
  }
  // CO2
  for(i=0; i<24; i++){
    pushUInt16(arr, measures[i][1] != null ? measures[i][1] : 0);
  }
  // Humidity
  for(i=0; i<24; i++){
    pushUInt16(arr, measures[i][2] != null ? measures[i][2] : 0);
  }
  // Pressure
  for(i=0; i<24; i++){
    pushUInt16(arr, measures[i][3] != null ? measures[i][3] * 10 : 0);
  }
  // Noise
  for(i=0; i<24; i++){
    pushUInt16(arr, measures[i][4] != null ? measures[i][4] : 0);
  }
  // sum_rain
  for(i=0; i<24; i++){
    pushUInt16(arr, measures[i][5] != null ? measures[i][5] * 1000 : 0);
  }  
}

function renewToken(callback) {
  console.log("renewToken");
  var url = HOST + "/auth/refresh?refresh_token=" + encodeURIComponent(refresh_token);
  sendRequest(url, "GET", function(e) {
    var res = JSON.parse(e);
    updateToken(res);
    store();
    if(callback)
      callback();
  },
  function(e) {
    if(e.responseText){
      var response = JSON.parse(e.responseText);
      console.log("renewToken error response " + e.responseText)
      if(response.error && response.error.code == 2){
        // Invalid access token
        disconnect();
        senderror("Please sign in");
      }
    }

    console.log("renewToken Error");
    
  });
}

function updateToken(e) {
  console.log("updateToken " + e.access_token + " " + e.refresh_token + " " + e.expires_in);
  access_token = e.access_token;
  refresh_token = e.refresh_token;
  var t = new Date;
  expires_in = t.valueOf() + e.expires_in * 1000 / 2;
}

// Timeout for (any) http requests, in milliseconds
var g_xhr_timeout = 6000;

function sendRequest(url, type, success, error, data) {
  data = data || null;
  var req = new XMLHttpRequest;
  req.open(type, url, !0);
  var xhrTimeout = setTimeout(function() {
    req.abort();
    senderror("Connection timeout");
  }, g_xhr_timeout);
  req.onload = function() {
    clearTimeout(xhrTimeout); // got response, no more need in timeout
    4 == req.readyState && (200 == req.status ? success(req.responseText) : error && error(req))
  };
  if(data){
    req.setRequestHeader("Content-type", "application/json;encoding=utf-8");
  }
  req.send(data);
}

function pushUInt8(array, value){
   array.push(value >> 0 & 0xFF);
}

function pushUInt16(array, value){
  if(value < 0){
    value = (-value - 1) & 0xFF;
    value = ~value;
  }
   array.push(value >> 0 & 0xFF);
   array.push(value >> 8 & 0xFF);
}

function pushUInt32(array, value){
   array.push(value >> 0 & 0xFF);
   array.push(value >> 8 & 0xFF);
   array.push(value >> 16 & 0xFF);
   array.push(value >> 24 & 0xFF);
}

