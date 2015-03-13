var pg = require('pg');
var Node = require('./objects/Node');

//connect to our database
var conString = "postgres://postgres:winderickx@localhost/ELLiDatabase"
//PostGres Connection String
/*
pg.connect(conString, function(err, client, done) {

  if (err) {
    return console.error('error fetching client from pool', err);
  }
  client.query('SELECT * FROM nodes', function(err, result) {
    done();
    if (err) {
      return console.error('error running query', err);
    }
    console.log(result.rows[0]);
    
    client.end();
  });

});*/

//pg.connect(conString, databaseHandler)
/*
var databaseHandler = function(err, client, done){
   if (err) {
    return console.error('error fetching client from pool', err);
  }
  this.client = client;
   
}
  */
  
var databaseHandler = function(){
   //pg.connect(conString, this.init);
   this.client = new pg.Client(conString);
   this.client.connect();
   this.ready = true;
}

databaseHandler.prototype.init = function(err,client,done){
   if (err) {
      return console.error('error fetching client from pool', err);
   }else{
      this.client = client;
      
      this.ready = true;
      console.log("Connected to database");  
   }
}

databaseHandler.prototype.getNodes = function(callback){
   if(this.ready){
      this.client.query('SELECT * FROM nodes', function(err, result) {
      //done();
      if (err) {
        return console.error('error running query', err);
      }
         
         var objects = [];
         //new Node(result.rows[0]);
         result.rows.forEach(function(object){
            objects.push(new Node(object));
         });
         console.log(objects);
         callback(objects); //callback...
      });
   }else{
      console.log(this.client);
      console.log("Client does not exist");
   }
}

databaseHandler.prototype.getNode = function(idNode, callback){
   if(this.ready){
      this.client.query('SELECT * FROM nodes WHERE id = $1', [idNode.toString()], function(err, result) {
      //done();
      if (err) {
        return console.error('error running query', err);
      }
         console.log("Amount of nodes for " + idNode + " from db: " + result.rows.length);
         var object = undefined;
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
   }else{
      console.log(this.client);
      console.log("Client does not exist");
   }
}

databaseHandler.prototype.closeConnection = function(){
   if(this.ready){ 
      this.client.end();
   }else{
      console.log("Client does not exist");
   }
}
/*
databaseHandler.prototype.testHandler = function(err, client, done) {
   handler.init(err, client,done);
   //console.log(handler.client);
   handler.getNodes();
   //handler.closeConnection();
}*/

//var handler = new databaseHandler();

module.exports = databaseHandler;
