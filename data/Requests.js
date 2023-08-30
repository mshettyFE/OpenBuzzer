// Server will periodically send Update request refresh the rankings
// Hitting Clear! will send a Clear request to clear the rankings
// Hitting Rescan will send a Rescan request to check for new alive candidates

// Reference: https://randomnerdtutorials.com/esp8266-nodemcu-websocket-server-arduino/

const Update = 1;
const Clear = 2;
const Rescan = 3;

const debug = true;

var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

initWebSocket();

function initWebSocket() {
   if(debug){
      console.log('Trying to open a WebSocket connection...');
  }
  websocket = new WebSocket(gateway);
  websocket.onopen    = onOpen;
  websocket.onclose   = onClose;
  websocket.onmessage = onMessage;
}

function onOpen(event) {
   if(debug){
      console.log('Connection opened');

   }
}

function onClose(event) {
   if(debug){
      console.log('Connection closed');
      }
   setTimeout(initWebSocket, 2000);
 }

 function onMessage(event) {
   if(debug){
      console.log("Msg Received: "+event.data);
   }
   var data =JSON.parse(event.data);
   if(!data.hasOwnProperty("MSG")){
      return;
   }
   switch(data["MSG"]){
      case Rescan:
// Make sure Alive is supplied
         if (data.hasOwnProperty("Alive")){
            var oldAlive = document.getElementById("Alive");
            var newAlive = document.createElement('tbody');
            newAlive.setAttribute("id", "Alive");
            for(let i = 0; i < data.Alive.length; ++i){
               var cur_player = data.Alive[i];
               if(cur_player==0){
                  add_row(newAlive,i,"Not Connected");
               }
               else{
                  add_row(newAlive,i,"Connected");
               }
            }
            oldAlive.parentNode.replaceChild(newAlive, oldAlive);
         }
      case Update:
      // Make sure Rank is supplied
      if (data.hasOwnProperty("Rank")){
         var oldRanking = document.getElementById("Ranking");
         var newRanking = document.createElement('tbody');
         newRanking.setAttribute("id", "Ranking");
         for(let i = 0; i < data.Rank.length; ++i){
            var cur_player = data.Rank[i];
            if(cur_player!=0){
               add_row(newRanking,cur_player,"Connected");
            }
         }
         oldRanking.parentNode.replaceChild(newRanking, oldRanking);
      }
      break;
      case Clear:
         var oldRanking = document.getElementById("Ranking");
         var newRanking = document.createElement('tbody');
         newRanking.setAttribute("id", "Ranking");
         newRanking.innerHTML = "";
         oldRanking.parentNode.replaceChild(newRanking, oldRanking);
      }
   document.getElementById("Rescan").disabled = false;
}

function SendRequest(MessageType){
   if(debug){
      console.log(MessageType);
   }
   switch(MessageType){
      case Update:
      case Clear:
         websocket.send(MessageType);
         break;
      case Rescan:
         document.getElementById("Rescan").disabled = true;
         websocket.send(MessageType);
         break;
      default:
         break;
   }
}

function add_row(body_tag, left, right){
   body_tag.insertRow().innerHTML += "<th>"+left+" <th>"+right+"\n";
//   body_tag.insertRow().innerHTML += "<th>"+left+"<\\th>"+" <th>"+right+"<\\th>\n";
}