var ffi = require('ffi');
var ref = require('ref');
var util            = require('util');
var events          = require('events');

var Certs;

var certs_interface = ffi.Library('certs', {
  'gen_cert': [ 'int', ['string','string' ] ],
  'gen_key': ['int', ['string']]
});

Certs.prototype.gen_cert = function(requestFile, certificateFile, callback){
   certs_interface.gen_cert.async(requestFile, certificateFile, function(err, res){
      callback(res);
   });
}

Certs.prototype.gen_key = function(keyFile, callback){
   certs_interface.gen_key.async(keyFile, function(err, res){
      callback(res);
   });
}

module.exports = Certs;

