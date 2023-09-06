// Server will periodically send Update request refresh the rankings
// Hitting Clear! will send a Clear request to clear the rankings
// Hitting Rescan will send a Rescan request to check for new alive candidates

// Reference: https://randomnerdtutorials.com/esp8266-nodemcu-websocket-server-arduino/

// Possible response codes from server 
const Update = 1;
const Clear = 2;
const Rescan = 3;

// flag to toggle debugging
const debug = false;

// flag to determine if sound should play
var should_play = true;

// Gateway address
var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

initWebSocket();

function initWebSocket() {
// Sets up websocket. Binds functions to Open,Close, and Message
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
// Try to reconnect connection if you disconnect. Try every 2 seconds
   if(debug){
      console.log('Connection closed');
      }
   setTimeout(initWebSocket, 2000);
 }

 function onMessage(event) {
   if(debug){
      console.log("Msg Received: "+event.data);
   }
// Get JSON data. Check for MSG attribute
   var data =JSON.parse(event.data);
   if(!data.hasOwnProperty("MSG")){
      return;
   }
   switch(data["MSG"]){
      case Rescan:
// Make sure Alive is supplied
         if (data.hasOwnProperty("Alive")){
// create new tbody with correct Alive attribute
            var oldAlive = document.getElementById("Alive");
            var newAlive = document.createElement('tbody');
            newAlive.setAttribute("id", "Alive");
// Run through players and add 1 row to new tbody for each possible player
            for(let i = 1; i < data.Alive.length; ++i){
               var cur_player = data.Alive[i];
               if(cur_player==0){
                  add_row(newAlive,i,"Not Connected");
               }
               else{
                  add_row(newAlive,i,"Connected");
               }
            }
// Reassign tbody
            oldAlive.parentNode.replaceChild(newAlive, oldAlive);
         }
      case Update:
      // Make sure Rank is supplied
      if (data.hasOwnProperty("Rank")){
         var oldRanking = document.getElementById("Ranking");
// Create new tbody with correct Ranking attribute
         var newRanking = document.createElement('tbody');
         newRanking.setAttribute("id", "Ranking");
// Add a row if someone buzzed in
         for(let i = 0; i < data.Rank.length; ++i){
            var cur_player = data.Rank[i];
            if(cur_player!=0){
               add_row(newRanking,i+1,cur_player);
            }
         }
// Replace old tbody
         oldRanking.parentNode.replaceChild(newRanking, oldRanking);
         if(debug){
            console.log("ShouldPlayUpdate: ")
            console.log(should_play);
         }
// Play sound for first player who buzzed in
         if(data.Rank[0]!= 0 && should_play){
            should_play = false;
            play();
         }
      }
      break;
      case Clear:
// Purge Ranking of all entries and reset state
         var oldRanking = document.getElementById("Ranking");
         var newRanking = document.createElement('tbody');
         newRanking.setAttribute("id", "Ranking");
         newRanking.innerHTML = "";
         oldRanking.parentNode.replaceChild(newRanking, oldRanking);
         currentWinner = 0;
         should_play = true;
         if(debug){
            console.log("ShouldPlayClear: ")
            console.log(should_play);
         }
      }
   document.getElementById("Rescan").disabled = false;
}

function SendRequest(MessageType){
// Function to ask for specific message types
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

function play(){
   var sound = new Audio('A41Sec.mp3');
   sound.play();
}