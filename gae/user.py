from google.appengine.ext import ndb

class PebbleUser(ndb.Model):
  pebble_id 	= ndb.StringProperty()
  refresh_count = ndb.IntegerProperty()
  last_refresh 	= ndb.DateProperty(auto_now=True)
