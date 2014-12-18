// Timeout for (any) http requests, in milliseconds
var g_xhr_timeout = 6000;

function sendRequest(url, type, success, error, timeout, data) {
  data = data || null;
  var req = new XMLHttpRequest;
  req.open(type, url, !0);

  var xhrTimeout = setTimeout(function() {
    req.abort();
    if(timeout)
    	timeout();
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