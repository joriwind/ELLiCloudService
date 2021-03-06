var Node = require('../objects/Node');
var SiteController = require('../objects/SiteController');
var databaseHandler = require('../databaseHandler.js');
var express = require('express');
//var bodyParser = require('body-parser');
var router = express.Router();

var dbHandler = new databaseHandler();

router.route('/nodes').get(function(req, res) {
   console.log("GET all");
   dbHandler.getNodes(function(obj,err){
      if(!err){
         res.send(JSON.stringify(obj));
      }else{
         res.send(err);
      }
   });
});

router.route('/nodes/:id').get(function(req, res) {
   console.log("Get one node: " + req.params.id);
   dbHandler.getNode(req.params.id, function(obj,err){
      if(!err){
         res.send(JSON.stringify(obj));
      }else{
         res.send(err);
      }
   });
});

router.route('/nodes/').post(function(req, res) {
   var var1 = req.body.node;
   var node = new Node(var1);
   console.log("node: " + JSON.stringify(node));
   dbHandler.insertNode(node, function(obj, err){
      if(!err){
         res.send(JSON.stringify(obj));
      }else{
         res.send(err);
      }
   });
});

router.route('/sitecontrollers/').post(function(req, res) {
   console.log("/sitecontrollers/ POST body: " + JSON.stringify(req.body));
   var var1 = req.body.siteController;
   var siteController = new SiteController(var1);
   console.log("New Site Controller: " + JSON.stringify(siteController));
   dbHandler.insertSiteController(siteController, function(obj, err){
      if(!err){
         res.send(JSON.stringify(obj));
      }else{
         res.send(err);
      }
   });
});

router.route('/sitecontrollers/').get(function(req, res) {
   console.log("Get all the siteControllers");
   dbHandler.getSiteControllers(function(obj, err){
      if(!err){
         res.send(JSON.stringify(obj));
      }else{
         res.send(err);
      }
   });
});


module.exports = router;