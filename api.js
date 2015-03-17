var express = require('express')
var routes = require('./routes/elliroutes'); //routes are defined here
var bodyParser = require('body-parser');
var https = require('https');
var fs = require('fs');
var app = express();

//configure body-parser
//app.use(bodyParser.json());
//app.use(bodyParser.urlencoded());
app.use(bodyParser.urlencoded({ extended: true }));
app.use(bodyParser.json());

app.use('/api', routes);
app.set('port', process.env.PORT || 8000);
 //This is our route middleware

//app.set('port', process.env.PORT || 8000);
 
/*var server = app.listen(app.get('port'), function() {
   
  console.log('Express server listening on port ' + server.address().port);
});*/

https.createServer(options, app).listen(443);

//module.exports = app;

/*
app.get('/', function (req, res) {
  res.send('Hello World!')
})

app.get('/test', function (req, res) {
  res.send('Huh?')
})

var server = app.listen(3000, function () {

  var host = server.address().address
  var port = server.address().port

  console.log('Ell-i Express cloud service listening at http://%s:%s', host, port)

})*/