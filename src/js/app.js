var HOST = 'http://pebble-netatmo.appspot.com';

var 
  access_token, 
  expires_in, 
  refresh_token,
  main_device,
  units_metric;

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

  // access_token = null;
  // refresh_token = null;
  // main_device = null;

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
  var uri = HOST + '/';
  if(refresh_token !== null){
    uri += "?refresh_token=" + encodeURIComponent(refresh_token) + "&pebble_token=" + encodeURIComponent(Pebble.getAccountToken());
  }
  else {
    uri += "auth";
  }
  console.log('showConfiguration ' + uri);
  Pebble.openURL(uri);
});

Pebble.addEventListener('webviewclosed', function(e) {
  console.log('webviewclosed ' + e.response);
  if(e.response == "CANCELLED"){
    return;
  }
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
  console.log('webviewclosed ' + e.response);
});

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
  var url = 'https://api.netatmo.net/api/devicelist?access_token=' + encodeURIComponent(access_token);
  console.log(url);
  sendRequest(url, "GET", 
  function(e) {
    var result = JSON.parse(e);

    // console.log(e);

    var names = [''];
    var device_ids = [main_device];

    for(var i=0; i<result.body.devices.length; i++){
      if(result.body.devices[i]._id == main_device)
        names[0] = toUTF8Array(result.body.devices[i].station_name, 40);
      else {
        device_ids[device_ids.length] = result.body.devices[i]._id;
        names[names.length]           = toUTF8Array(result.body.devices[i].station_name, 40);
      }
    }

    var names_arr = [];
    for(var i = 0; i<names.length; i++)
      for(var j = 0; j<40; j++)
        pushUInt8(names_arr, names[i][j]);

    MessageQueue.sendAppMessage({'reset_data':names_arr}, 
      function(e) {
        console.log('reset sent');
        fetchDataForDevices(device_ids);
      },
      function(e) {
        console.log('reset NOT sent');
      });
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
  },
  function(e){
    senderror("Connection timeout");
  });
}

function fetchDataForDevices(device_ids){
  console.log("fetchDataForDevices " + device_ids);
  if (!access_token){
    senderror("Please sign in");
    return;
  }

  var n = new Date;
  if (expires_in < n.valueOf()) 
    return renewToken(function() { fetchDataForDevices(device_ids) });

  for(var d=0; d<device_ids.length; d++){
    var req = new XMLHttpRequest();
    var url = 'https://api.netatmo.net/api/devicelist?access_token=' + encodeURIComponent(access_token) + '&device_id=' + encodeURIComponent(device_ids[d]);
    console.log(url);
    (function(id) {
        sendRequest(url, "GET", 
        function(e) {
          var result = JSON.parse(e);
          if(result.body.devices.length > 0){
            fetchMeasures(id, result.body.devices[0]);
          }
          for(var i=0; i<result.body.modules.length; i++){
            fetchMeasures(id, result.body.devices[0], result.body.modules, i);
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
        },
        function(e){
          senderror("Connection timeout");
        });
    })(d);
  }
}

function fetchMeasures(id, device, modules, index){
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

  var startTimeSeconds = n.getTime()/1000 - 24 * 3600;
  var startUTCseconds = startTimeSeconds + n.getTimezoneOffset()*60;

  var req = new XMLHttpRequest();
  var url = 'https://api.netatmo.net/api/getmeasure?access_token=' + encodeURIComponent(access_token) + '&device_id=' + encodeURIComponent(device._id) + '&type=Temperature,CO2,Humidity,Pressure,Noise,sum_rain&scale=1hour&limit=24&optimize=true&date_begin=' + startUTCseconds;
  if(modules)
    url += '&module_id=' + encodeURIComponent(modules[index]._id);
  console.log(url);
  sendRequest(url, "GET", 
  function(e) {
    var result = JSON.parse(e);
    var dashboard_data = modules ? modules[index].dashboard_data : device.dashboard_data;
    
    var array = [];
    fillDashBoardData(
        array,
        id,
        modules ? moduleType(modules[index].type) : moduleType(device.type),
        modules ? modules[index].module_name : device.module_name,
        'Temperature' in dashboard_data ? dashboard_data.Temperature : 0,
        'min_temp'    in dashboard_data ? dashboard_data.min_temp    : 0,
        'max_temp'    in dashboard_data ? dashboard_data.max_temp    : 0,
        'Humidity'    in dashboard_data ? dashboard_data.Humidity    : 0,
        'Noise'       in dashboard_data ? dashboard_data.Noise       : 0,
        'CO2'         in dashboard_data ? dashboard_data.CO2         : 0,
        'Pressure'    in dashboard_data ? dashboard_data.Pressure    : 0,
        'sum_rain_24' in dashboard_data ? dashboard_data.sum_rain_24 : 0,
        result.body,
        startTimeSeconds);

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
  },
  function(e){
    senderror("Connection timeout");
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

function fillDashBoardData(arr, station_id, type, name, temp, temp_min, temp_max, humidity, noise, co2, pressure, rain, measures, start_measure_time){
  pushUInt16(arr, station_id);
  pushUInt16(arr, type);

  var utf8Name = toUTF8Array(name, 40);
  var i = 0;
  for(; i<40; i++)
    pushUInt8(arr, utf8Name[i]);

  pushUInt16(arr, (units_metric == 0 ? 32 + 1.8 * temp : temp) * 10);
  pushUInt16(arr, (units_metric == 0 ? 32 + 1.8 * temp_min : temp_min) * 10);
  pushUInt16(arr, (units_metric == 0 ? 32 + 1.8 * temp_max : temp_max) * 10);
  pushUInt8(arr,  humidity);         
  pushUInt8(arr,  noise);
  pushUInt16(arr, co2);
  pushUInt16(arr, pressure*10);
  pushUInt16(arr, rain*1000);

  var measures_per_day    = 24;
  var measures_step_time  = 24 * 3600 / measures_per_day ;

  // fix measures
  // TODO comment the algorithm
  var fixed_measures = [];
  if(measures.length == 1 && measures[0].value.length == measures_per_day){
    fixed_measures = measures[0].value;
  }
  else {
    var failed_measure = [0, 0, 0, 0, 0, 0];
    var current_index = 0;
    for(i = 0; i<measures.length; i++){
      var beg_time = measures[i].beg_time;
      while(start_measure_time + measures_step_time < beg_time){
        fixed_measures[current_index++] = failed_measure;
        start_measure_time += measures_step_time;
      }
      var step_time = measures[i].step_time;
      for(var j=0; j<measures[i].value.length; j++){
        for(var k=0; j>0 && k<(step_time/measures_step_time - 1); k++)
          fixed_measures[current_index++] = measures[i].value[j-1];
        fixed_measures[current_index++] = measures[i].value[j];
      }
      start_measure_time = beg_time + step_time * (measures[i].value.length - 1);
    }
    if(current_index > measures_per_day){
      var diff = current_index - measures_per_day;
      for(i=0;i<measures_per_day;i++){
        fixed_measures[i] = fixed_measures[i + diff];
      }
      current_index = measures_per_day;
    }
    else while(current_index < measures_per_day){
      fixed_measures[current_index++] = failed_measure;
    }
  }  

  // Temperature
  for(i=0; i<measures_per_day; i++){
    var tmp = fixed_measures[i][0] != null ? fixed_measures[i][0] : 0;
    tmp = units_metric == 0 ? 32 + 1.8 * tmp : tmp;
    pushUInt16(arr, tmp * 10);
  }
  // CO2
  for(i=0; i<measures_per_day; i++){
    pushUInt16(arr, fixed_measures[i][1] != null ? fixed_measures[i][1] : 0);
  }
  // Humidity
  for(i=0; i<measures_per_day; i++){
    pushUInt16(arr, fixed_measures[i][2] != null ? fixed_measures[i][2] : 0);
  }
  // Pressure
  for(i=0; i<measures_per_day; i++){
    pushUInt16(arr, fixed_measures[i][3] != null ? fixed_measures[i][3] * 10 : 0);
  }
  // Noise
  for(i=0; i<measures_per_day; i++){
    pushUInt16(arr, fixed_measures[i][4] != null ? fixed_measures[i][4] : 0);
  }
  // sum_rain
  for(i=0; i<measures_per_day; i++){
    pushUInt16(arr, fixed_measures[i][5] != null ? fixed_measures[i][5] * 1000 : 0);
  }  
}

function renewToken(callback) {
  console.log("renewToken");
  var url = HOST + "/auth/refresh?refresh_token=" + encodeURIComponent(refresh_token) + "&pebble_token=" + encodeURIComponent(Pebble.getAccountToken());
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
  },
  function(e){
    senderror("Connection timeout");
  });
}

function updateToken(e) {
  console.log("updateToken " + e.access_token + " " + e.refresh_token + " " + e.expires_in);
  access_token = e.access_token;
  refresh_token = e.refresh_token;
  var t = new Date;
  expires_in = t.valueOf() + e.expires_in * 1000 / 2;
}
