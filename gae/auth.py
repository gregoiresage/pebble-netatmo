import webapp2
from urllib import urlencode
import json, urllib2

import os

from secret import app_id, app_secret
import config

import jinja2

JINJA_ENVIRONMENT = jinja2.Environment(
    loader=jinja2.FileSystemLoader(os.path.dirname(__file__)),
    extensions=['jinja2.ext.autoescape'],
    autoescape=True)

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
            url = "http://pebble-netatmo.appspot.com/config.html?error="+error
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
            url = "http://pebble-netatmo.appspot.com/config.html?error="+urlencode(result['error'])
            self.response.location = url
            self.response.status_int = 302
            return

        args = {}
        # args["access_token"] = result["access_token"]
        args["refresh_token"] = result["refresh_token"]
        # args["expires_in"] = result["expires_in"]
        # url = "http://pebble-netatmo.appspot.com/config.html?"+urlencode(args)
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

class Test(webapp2.RequestHandler):
    def get(self):
        refresh_token = self.request.get("refresh_token")
        error = self.request.get("error")
        template_values = {}

        if error :
            template_values['error'] = error
        elif refresh_token :
            args = {}
            args["grant_type"] = "refresh_token"
            args["client_id"] = app_id
            args["client_secret"] = app_secret
            args["refresh_token"] = self.request.get("refresh_token")
            result = query_json("https://api.netatmo.net/oauth2/token", urlencode(args))

            print 'Test1 ' + json.dumps(result)

            if 'error' in result :
                template_values['error'] = result['error'];
                print config.host + '/auth/refresh error ' + result['error'];
            else :
                access_token    = result['access_token']
                refresh_token   = result['refresh_token']
                expires_in      = result['expires_in']

                args['access_token'] = access_token
                args['app_type'] = 'app_station'
                result = query_json("https://api.netatmo.net/api/devicelist", urlencode(args))

                print 'Test2 ' + json.dumps(result)

                if 'error' in result :
                    template_values['error'] = result['error']
                    print 'https://api.netatmo.net/api/devicelist error ' + result['error']
                else :
                    template_values['access_token']     = access_token
                    template_values['refresh_token']    = refresh_token
                    template_values['expires_in']       = expires_in
                    template_values['devices']          = result['body']['devices']

        print 'Test3 ' + json.dumps(template_values)

        template = JINJA_ENVIRONMENT.get_template('templates/config.html.jinja')
        self.response.write(template.render(template_values))

application = webapp2.WSGIApplication([
    ('/auth', AuthRedirector),
    ('/auth/result', AuthCallback),
    ('/auth/refresh', AuthRefreshAccessToken),
    ('/config.html', Test)
], debug=True)