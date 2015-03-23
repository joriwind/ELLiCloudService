var express = require('express')
var routes = require('./routes/elliroutes'); //routes are defined here
var bodyParser = require('body-parser');
var http = require('http');
var fs = require('fs');
var app = express();

//configure body-parser
//app.use(bodyParser.json());
//app.use(bodyParser.urlencoded());
app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());

app.use('/api', routes);
//app.set('port', process.env.PORT || 8000);
 //This is our route middleware
 
/*var server = app.listen(app.get('port'), function() {
   
  console.log('Express server listening on port ' + server.address().port);
});*/

//ssh.on('connection', function(){ /* … */ });

var httpServer = http.createServer(app);

httpServer.listen(8000)