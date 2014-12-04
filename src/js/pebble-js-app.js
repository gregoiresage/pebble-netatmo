var HOST = 'http://pebble-netatmo.appspot.com';

var 
  access_token, 
  expires_in, 
  refresh_token,
  main_device;

function store(){
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
}

function read(){
  access_token  = localStorage.getItem("access_token");
  expires_in    = localStorage.getItem("expires_in");
  refresh_token = localStorage.getItem("refresh_token");
  main_device   = localStorage.getItem("main_device");

  console.log("read " + access_token + " "+ expires_in + " "+ refresh_token + " " + main_device);
}

function disconnect() {
  console.log("Disconnection");
  access_token = undefined;
  expires_in = 0; 
  refresh_token = undefined; 
  main_device = undefined;
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
    uri += "?refresh_token=" + refresh_token;
  }
  Pebble.openURL(uri);
});

Pebble.addEventListener('webviewclosed', function(e) {
  console.log('webviewclosed ' + e.response);
  var s = decodeURIComponent(e.response);
  if (s) {
    var c = JSON.parse(s);
    if(c.access_token) {
      console.log(c.access_token);
      updateToken(c);
      fetchData();
    }
    else {
      disconnect();
    }
  }
});

function fetchData(){
  console.log("fetchData");
  if (!access_token)
    return console.log("Not signed in");

  var n = new Date;
  if (expires_in < n.valueOf()) 
    return renewToken(function() { fetchData() });

  var req = new XMLHttpRequest();
  var url = 'https://api.netatmo.net/api/devicelist?access_token=' + access_token + '&device_id=' + main_device;
  console.log(url);
  sendRequest(url, "GET", 
  function(e) {
    var json = JSON.parse(e);
    console.log("Name : " +                       json.body.devices[0].module_name);
    console.log("Inside Temperature : " +         json.body.devices[0].dashboard_data.Temperature);
    console.log("Inside Temperature Min/Max: " +  json.body.devices[0].dashboard_data.min_temp + "/" + json.body.devices[0].dashboard_data.max_temp);
    console.log("Inside Humidity : " +            json.body.devices[0].dashboard_data.Humidity);
    console.log("Inside Pressure : " +            json.body.devices[0].dashboard_data.Pressure);
    console.log("Inside CO2 : " +                 json.body.devices[0].dashboard_data.CO2);

    console.log("Name : " +                       json.body.modules[0].module_name);
    console.log("Outside Temperature : " +        json.body.modules[0].dashboard_data.Temperature);
    console.log("Outside Temperature Min/Max: " + json.body.modules[0].dashboard_data.min_temp + "/" + json.body.modules[0].dashboard_data.max_temp);
    console.log("Outside Humidity : " +           json.body.modules[0].dashboard_data.Humidity);

    var data = {};
    if(json.body.devices.length > 0){
      var dashboard_data = [];
      var device_data = json.body.devices[0].dashboard_data;
      fillDashBoardData(
        dashboard_data,
        json.body.devices[0].module_name,
        device_data.Temperature,
        device_data.min_temp,
        device_data.max_temp,
        device_data.Humidity,      
        device_data.Noise,
        device_data.CO2,
        device_data.Pressure);
      data.module_data = dashboard_data;
    }

    for(var i=0; i<json.body.modules.length; i++){
      var dashboard_data = [];
      var module_data = json.body.modules[i].dashboard_data;
      fillDashBoardData(
        dashboard_data,
        json.body.modules[i].module_name,
        module_data.Temperature,
        module_data.min_temp,
        module_data.max_temp,
        module_data.Humidity,      
        0xFFFF, 
        0xFFFF, 
        0xFFFF);
      data['station_data_' + i] = dashboard_data;
    }

    console.log(JSON.stringify(data));

    Pebble.sendAppMessage(data, 
      function(e) {
        console.log('data sent');
      },
      function(e) {
        console.log('data NOT sent');
      });
  },
  function(e) {
    console.log("Error status");
    disconnect();
  });
}

function fillDashBoardData(arr, name, temp, temp_min, temp_max, humidity, noise, co2, pressure){
  var i = 0;
  for(; i<name.length && i<19; i++)
    pushUInt8(arr, name.charCodeAt(i));
  
  for(; i<20; i++)
    pushUInt8(arr, 0);

  pushUInt16(arr, temp*10);
  pushUInt16(arr, temp_min*10);
  pushUInt16(arr, temp_max*10);
  pushUInt8(arr, humidity);         
  pushUInt8(arr, noise);
  pushUInt16(arr, co2);
  pushUInt16(arr, pressure*10);
}

function renewToken(callback) {
  console.log("renewToken");
  var url = HOST + "/auth/refresh?refresh_token=" + encodeURIComponent(refresh_token);
  sendRequest(url, "GET", function(e) {
    var res = JSON.parse(e);
    updateToken(res);
    if(callback)
      callback();
  });
}

function updateToken(e) {
  console.log("updateToken");
  access_token = e.access_token;
  refresh_token = e.refresh_token;
  var t = new Date;
  expires_in = t.valueOf() + e.expires_in * 1000;
  main_device = e.main_device;
  store();
}

function sendRequest(url, type, success, error, data) {
  data = data || null;
  var req = new XMLHttpRequest;
  req.open(type, url, !0);
  req.onload = function() {
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
   array.push(value >> 0 & 0xFF);
   array.push(value >> 8 & 0xFF);
}

function pushUInt32(array, value){
   array.push(value >> 0 & 0xFF);
   array.push(value >> 8 & 0xFF);
   array.push(value >> 16 & 0xFF);
   array.push(value >> 24 & 0xFF);
}


// var result = [];                 // Array of bytes that we produce
//    pushUInt16(result, widthBytes);  // row_size_bytes
//    pushUInt16(result, flags);       // info_flags
//    pushUInt16(result, 0);           // bounds.origin.x
//    pushUInt16(result, 0);           // bounds.origin.y
//    pushUInt16(result, width);       // bounds.size.w
//    pushUInt16(result, height);      // bounds.size.h