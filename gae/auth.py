import webapp2
from urllib import urlencode
import json, urllib2

from secret import app_id, app_secret
import config

def query_json(url, data):
    """
    Query JSON data using POST request.
    """
    try:
        return json.loads(urllib2.urlopen(url, data).read())
    except urllib2.HTTPError as e: # exception is a file-like object
        return json.loads(e.read())

class AuthRedirector(webapp2.RequestHandler):
    def get(self):
        args = self.request.GET
        args["client_id"] = app_id
        args["redirect_uri"] = config.auth_redir_uri
        args["scope"] = "read_station"
        url = "https://api.netatmo.net/oauth2/authorize?"+urlencode(args)
        self.response.location = url
        self.response.status_int = 302

class AuthCallback(webapp2.RequestHandler):
    def get(self):
        error = self.request.get("error")
        if error:
            args = {}
            args["error"] = error
            url = "http://pebble-netatmo.appspot.com/config.html?"+urlencode(args)
            self.response.location = url
            self.response.status_int = 302
            return

        # get access token
        args = {}
        args["grant_type"] = "authorization_code"
        args["client_id"] = app_id
        args["client_secret"] = app_secret
        args["code"] = self.request.get("code")
        args["redirect_uri"] = config.auth_redir_uri
        args["scope"] = "read_station"
        result = query_json("https://api.netatmo.net/oauth2/token", urlencode(args))

        if 'error' in result:
            args = {}
            args["error"] = error
            url = "http://pebble-netatmo.appspot.com/config.html?"+urlencode(args)
            self.response.location = url
            self.response.status_int = 302
            return

        args = {}
        args["access_token"] = result["access_token"]
        args["refresh_token"] = result["refresh_token"]
        args["expires_in"] = result["expires_in"]
        url = "http://pebble-netatmo.appspot.com/config.html?"+urlencode(args)
        self.response.location = url
        self.response.status_int = 302

        # self.response.headers['Content-Type'] = 'text/plain'
        # self.response.write('Token response : ' + result["access_token"])

class AuthRefreshAccessToken(webapp2.RequestHandler):
    def get(self):
        args = {}
        args["grant_type"] = "refresh_token"
        args["client_id"] = app_id
        args["client_secret"] = app_secret
        args["refresh_token"] = self.request.get("refresh_token")
        result = query_json("https://api.netatmo.net/oauth2/token", urlencode(args))

        self.response.headers['Content-Type'] = 'application/json;charset=UTF-8'
        self.response.write(json.dumps(result))

application = webapp2.WSGIApplication([
    ('/auth', AuthRedirector),
    ('/auth/result', AuthCallback),
    ('/auth/refresh', AuthRefreshAccessToken)
], debug=True)