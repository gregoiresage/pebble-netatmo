import webapp2
from urllib import urlencode
import json, urllib2

from secret import app_id, app_secret
import config


def query_json(url, data):
    try:
        return json.loads(urllib2.urlopen(url, data).read())
    except urllib2.HTTPError as e: 
        return {'error' : e.reason}

class AuthRedirector(webapp2.RequestHandler):
    def get(self):
        args = self.request.GET
        args["client_id"]       = app_id
        args["redirect_uri"]    = config.auth_redir_uri
        args["scope"]           = "read_station"
        url = "https://api.netatmo.net/oauth2/authorize?"+urlencode(args)
        self.response.location = url
        self.response.status_int = 302

class AuthCallback(webapp2.RequestHandler):
    def get(self):
        error = self.request.get("error")
        if error:
            url = config.host + "/config.html?error="+error
            self.response.location = url
            self.response.status_int = 302
            return

        # get access token
        args = {}
        args["grant_type"]      = "authorization_code"
        args["client_id"]       = app_id
        args["client_secret"]   = app_secret
        args["code"]            = self.request.get("code")
        args["redirect_uri"]    = config.auth_redir_uri
        args["scope"]           = "read_station"
        result = query_json("https://api.netatmo.net/oauth2/token", urlencode(args))

        if 'error' in result:
            url = config.host + "/?error="+urlencode(result['error'])
            self.response.location = url
            self.response.status_int = 302
            return

        # redirect to main page
        args = {}
        args["refresh_token"] = result["refresh_token"]
        url = config.host + "/?"+urlencode(args)
        self.response.location = url
        self.response.status_int = 302

class AuthRefreshAccessToken(webapp2.RequestHandler):
    def refresh(self, refresh_token):
        args = {}
        args["grant_type"]      = "refresh_token"
        args["client_id"]       = app_id
        args["client_secret"]   = app_secret
        args["refresh_token"]   = refresh_token
        result = query_json("https://api.netatmo.net/oauth2/token", urlencode(args))

        self.response.headers['Content-Type'] = 'application/json;charset=UTF-8'
        self.response.write(json.dumps(result))

    def get(self):
        self.refresh(self.request.get("refresh_token"))

    def post(self):
        self.refresh(self.request.get("refresh_token"))

application = webapp2.WSGIApplication([
    ('/auth',           AuthRedirector),
    ('/auth/result',    AuthCallback),
    ('/auth/refresh',   AuthRefreshAccessToken)
], debug=True)