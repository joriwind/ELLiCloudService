var Node = require('../objects/Node');
var databaseHandler = require('../databaseHandler.js');
var express = require('express');
//var bodyParser = require('body-parser');
var router = express.Router();

var dbHandler = new databaseHandler();

router.route('/nodes').get(function(req, res) {
   console.log("GET all");
   dbHandler.getNodes(function(obj){
      res.send(JSON.stringify(obj));
   });
});

router.route('/nodes/:id').get(function(req, res) {
   console.log("Get one node: " + req.params.id);
   dbHandler.getNode(req.params.id, function(obj){
      res.send(JSON.stringify(obj));
   });
});

router.route('/nodes/').post(function(req, res) {
   console.log("POST: " + "param: " + req.params+" body: "  + req.body);
   var var1 = req.body.node;
   dbHandler.setNode(new Node(JSON.parse(var1)), function(obj){
      res.send(JSON.stringify(obj));
   });
   res.send(req.body);
});

router.route('/dbHandler').get(function(req, res) {
   return res.send(dbHandler);
});

router.route('/testObject').post(function(req, res) {
  
});

module.exports = router;