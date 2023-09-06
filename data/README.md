Here are website files.

* game_index.html: html skeleton
  * Rows get added to Ranking tbody as players buzz in
  * When Rescan gets pressed, Alive tbody gets populated with the current players connected
  * When Clear gets pressed, both Rranking and Alive are cleared, and all buzzers are reset to unlocked
* index.css: Bare bones css file
* Requests.jss: Opens up a WebSocket between client (say, a mobile device) and ESP8266 server. Reads JSON response from server and updates display accordingly. Also plays A4 for 1 second when someone first buzzes in
* A41Sec.mp3: tone that plays when player buzzes in