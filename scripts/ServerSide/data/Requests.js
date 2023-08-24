var count = 0;
var btn    = document.getElementsByClassName('in');
var clicked = document.getElementsByClassName("Clicked");
console.log(clicked.length);
function Clicked(){
   console.log("button was clicked " + (++count) + " times");
   document.getElementById("clicked").innerHTML = count;
};