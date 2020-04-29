##QT Redis
###The least you need to know

##Building Redis etc.
To use this you'll need to install Redis and Webdis.

Redis is the fast key-value system, while Webdis is an HTTP interface for Redis.
Using Http makes life much easier, though is an additional level of code to understand.

Building Webdis requires events.h, so you'll also need to install eventLib.

##Building Qt_redis
Open the .pro file in creator.

You'll need to set the environment variables:
PREFIX - a location for your peronal libraries/dylib files
DYLD_LIBRARY_PATH - the location for your dylibs (this is LD_LIBRARY_PATH on linux)

Build the qt_redis library, then test with redis_example.

You can test by pushing values to Redis via the command line. For example:

curl http://127.0.0.1:7379/SET/my:key/1.0
(set the key "my:key" to 1.0)

curl http://127.0.0.1:7379/Get/my:key
(get the value of "my:key")

curl http://127.0.0.1:7379/PUBLISH/my:published:key/1.0
(publish the value 1.0 to key "my:published:key")
