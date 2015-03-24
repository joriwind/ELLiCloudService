
function Node(params){
   this.id = params.id;
   this.pk = params.pk;
   this.idSC = params.idSC||"undefinded";
}

Node.prototype.changePk = function (newPk){
   this.pk = newPk;
}

Node.prototype.changeIdSC = function (newIdSC){
   this.idSC = newIdSC;
}


var testNode = function (){
   var obj  = new Node({id:"5", pk:"6", idSC:"test"});
   obj.changePk("7");
   console.log(obj.id + " " + obj.pk +" connected to: " + obj.idSC);
}

//testNode();
module.exports = Node;