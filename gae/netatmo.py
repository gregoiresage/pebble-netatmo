from urllib import urlencode
import json, urllib2, os, config, jinja2, webapp2

import logging

JINJA_ENVIRONMENT = jinja2.Environment(
    loader=jinja2.FileSystemLoader(os.path.dirname(__file__)),
    extensions=['jinja2.ext.autoescape'],
    autoescape=True)

def query_json(url, data):
	try:
		return json.loads(urllib2.urlopen(url, data).read())
	except urllib2.HTTPError as e:
		return {"error" : e.reason}

class MainPage(webapp2.RequestHandler):
    def get(self):
        refresh_token = self.request.get("refresh_token")
        error = self.request.get("error")
        template_values = {}

        if error :
            template_values['error'] = error
        elif refresh_token :
            args = {}
            args["refresh_token"] = self.request.get("refresh_token")
            args["pebble_token"]  = self.request.get("pebble_token")
            result = query_json(config.host + '/auth/refresh', urlencode(args))

            if 'error' in result :
                template_values['error'] = result['error']
                logging.error('Refresh Token error ' + json.dumps(result['error']))
            else :
                access_token    = result['access_token']
                refresh_token   = result['refresh_token']
                expires_in      = result['expires_in']

                args['access_token'] 	= access_token
                args['app_type'] 		= 'app_station'
                result = query_json("https://api.netatmo.net/api/getstationsdata", urlencode(args))

                logging.info('Device List ' + json.dumps(result))

                if 'error' in result :
                    template_values['error'] = result['error']
                    logging.error('Get devicelist error ' + json.dumps(result['error']))
                else :
                    template_values['access_token']		= access_token
                    template_values['refresh_token'] 	= refresh_token
                    template_values['expires_in']       = expires_in
                    template_values['devices']          = result['body']['devices']

        logging.info('Configuration ' + json.dumps(template_values))

        template = JINJA_ENVIRONMENT.get_template('templates/config.html.jinja')
        self.response.write(template.render(template_values))

application = webapp2.WSGIApplication([
    ('/', MainPage),
    ('/config.html', MainPage),
], debug=True)