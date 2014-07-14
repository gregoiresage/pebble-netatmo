import webapp2
from urllib import urlencode
import json, urllib2

from secret import app_id, app_secret
import config

def query_json(url, data):
    """
    Query JSON data using POST request.
    Returns only data and ignores result code.
    """
    if not (data is str):
        data = urlencode(data)
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
            self.response.status_int = 400
            return

        args = self.request.GET
        args["grant_type"] = "authorization_code"
        args["client_id"] = app_id
        args["client_secret"] = app_secret
        args["code"] = self.request.get("code")
        # args["redirect_uri"] = config.token_redir_uri
        args["scope"] = "read_station"
        result = query_json("https://api.netatmo.net/oauth2/token", urlencode(args))
        if 'error' in result:
            self.response.status_int = 400
            return

        self.response.headers['Content-Type'] = 'text/plain'
        self.response.write('Token response : ' + self.request.get("access_token"))

        # self.response.location = url
        # self.response.status_int = 302


# grant_type=authorization_code
#     client_id=[YOUR_APP_ID]
#     client_secret=[YOUR_CLIENT_SECRET]
#     code=[CODE_RECEIVED_FROM_USER]
#     redirect_uri=[YOUR_REDIRECT_URI]
#     scope=[SCOPE_DOT_SEPARATED]

class TokenCallback(webapp2.RequestHandler):
    def get(self):
        self.response.headers['Content-Type'] = 'text/plain'
        self.response.write('Code response : ' + self.request.get("code"))

application = webapp2.WSGIApplication([
    ('/auth', AuthRedirector),
    ('/auth/result', AuthCallback),
    ('/auth/token', TokenCallback),
], debug=True)