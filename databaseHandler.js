var pg = require('pg');
var Node = require('./objects/Node');
var SiteController = require('./objects/SiteController');

//connect to our database
var conString = "postgres://postgres:winderickx@localhost/ELLiDatabase"
//PostGres Connection String


var databaseHandler = function(){
   //pg.connect(conString, this.init);
   //this.client = new pg.Client(conString);
   //this.client.connect();
   //this.ready = true;
}

/** Node stuff **/

databaseHandler.prototype.getNodes = function(callback){
   pg.connect(conString, function (err, client, done) {
    // do some stuff
    client.query('SELECT * FROM "nodes"', function(err, result) {
         var objects;
         if (err) {
            callback(objects, err);
            return console.error('error running query', err);
         }
         objects = [];
         //new Node(result.rows[0]);
         result.rows.forEach(function(object){
            objects.push(new Node(object));
         });
         console.log(objects);
         callback(objects); //callback...
         //done();
      });
      done();
  });

}

databaseHandler.prototype.getNode = function(idNode, callback){
   pg.connect(conString, function (err, client, done) {
    // do some stuff
   client.query('SELECT * FROM "nodes" WHERE id = $1', [idNode.toString()], function(err, result) {
         var object;
         if (err) {
            callback(object, err);
            return console.error('error running query', err);
         }
         
         console.log("Amount of nodes for " + idNode + " from db: " + result.rows.length);
         //new Node(result.rows[0]);
         if(result.rows.length > 1 ){
            return console.error('Multiple nodes received');
         }else if( result.rows.length == 1){
            object = new Node(result.rows[0]);
            console.log(object);
            callback(object); //callback...
         }else{
            callback(object);
         }
      });
      done();
  });

}

databaseHandler.prototype.setNode = function(node, callback){
   pg.connect(conString, function (err, client, done) {
    // do some stuff
      if(node != undefined){
         
         client.query('UPDATE "nodes" SET idsc = $1 WHERE id = $2', [node.idSC.toString(), node.id.toString()], function(err, result) {
            //done();
            var object;
            if (err) {
               callback(object, err);
               return console.error('error running query', err);
            }
            //done();
            console.log("Amount of nodes changed for " + node.id + " from db: " + result.rows.length);
            object = result.rows[0];
            callback(object, err);
            
         });
      }
      done();
  });

}

databaseHandler.prototype.insertNode = function(node, callback){
   pg.connect(conString, function (err, client, done) {
    // do some stuff
      if(node != undefined){
         
         client.query('INSERT INTO "nodes" (id, pk, idsc) VALUES($1,$2,$3)', [node.id.toString(), node.pk.toString(), node.idSC.toString()], function(err, result) {
            var object;
            if (err) {
               callback(object, err);
               return console.error('error running query', err);
            }
            console.log("Node inserted: " + node.id + " rows changed: " + result.rows.length);
            
            callback(node);
            
         });
      }
      done();
  });

}

/** Site Controller stuff **/

databaseHandler.prototype.getSiteControllers = function(callback){
   pg.connect(conString, function (err, client, done) {
    // do some stuff
    client.query('SELECT * FROM "siteController"', function(err, result) {
         var objects;
         if (err) {
            callback(objects, err);
            return console.error('error running query', err);
         }
         objects = [];
         result.rows.forEach(function(object){
            objects.push(new SiteController(object));
         });
         console.log("All the Site controllers(getSiteControllers): " + objects);
         callback(objects); //callback...
      });
      done();
  });

}

databaseHandler.prototype.insertSiteController = function(siteController, callback){
   pg.connect(conString, function (err, client, done) {
    // do some stuff
      if(siteController != undefined){
         
         client.query('INSERT INTO "siteController" VALUES($1)', [siteController.id.toString()], function(err, result) {
            //done();
            if (err) {
               callback(err)
               return console.error('error running query', err);
            }
            //done();
            console.log("Site controller added: " + siteController.id + " rows changed: " + result.rows.length);
            
            callback(siteController);
            
         });
      }
      done();
  });

}


module.exports = databaseHandler;
