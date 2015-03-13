var Object1 = require('../objects/Node');
var databaseHandler = require('../databaseHandler.js');
var express = require('express');
var router = express.Router();

var dbHandler = new databaseHandler();

router.route('/nodes').get(function(req, res) {
   dbHandler.getNodes(function(obj){
      res.send(JSON.stringify(obj));
   });
});

router.route('/nodes/:id').get(function(req, res) {
   dbHandler.getNode(req.params.id, function(obj){
      res.send(JSON.stringify(obj));
   });
});

router.route('/dbHandler').get(function(req, res) {
   return res.send(dbHandler);
});

router.route('/testObject').post(function(req, res) {
  
});

module.exports = router;