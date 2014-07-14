import webapp2
from urllib import urlencode
import json, urllib2

from secret import app_id, app_secret
import config

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
        self.response.headers['Content-Type'] = 'text/plain'
        self.response.write('Hello, World!\n')
        self.response.write(self.request.get("code"))

application = webapp2.WSGIApplication([
    ('/auth', AuthRedirector),
    ('/auth/result', AuthCallback),
], debug=True)