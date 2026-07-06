#include <WiFi.h>
#include <WebServer.h>

// ---------------- WiFi ----------------
const char* ssid = "NSUT_WIFI";
const char* password = "";

// ---------------- Joystick ----------------
const int joyX = 34;
const int joySW = 27;

WebServer server(80);

// -------------------------------------------------

void handleJoystick() {
  int x = analogRead(joyX);
  int sw = digitalRead(joySW);

  String json = "{\"x\":";
  json += x;
  json += ",\"sw\":";
  json += sw;
  json += "}";

  server.send(200, "application/json", json);
}

// -------------------------------------------------

void handleRoot() {

server.send(200,"text/html",R"rawliteral(

<!DOCTYPE html>
<html>

<head>

<meta charset="UTF-8">

<title>ESP32 Dodge Game</title>

<style>

body{

margin:0;
background:#000;
overflow:hidden;
font-family:Arial;
}

canvas{

display:block;
margin:auto;
background:#111;
}

button{

position:absolute;
left:50%;
top:60%;
transform:translateX(-50%);
padding:10px 20px;
font-size:18px;
display:none;
}

</style>

</head>

<body>

<canvas id="game" width="320" height="480"></canvas>

<button id="restart">Restart</button>

<script>

const canvas=document.getElementById("game");
const ctx=canvas.getContext("2d");

let player={
x:140,
y:440,
w:40,
h:12
};

let blocks=[];

let score=0;

let gameOver=false;

const restart=document.getElementById("restart");

restart.onclick=function(){

location.reload();

}

function spawnBlock(){

blocks.push({

x:Math.random()*280,
y:-15,
w:40,
h:12

});

}

function draw(){

ctx.clearRect(0,0,320,480);

ctx.fillStyle="lime";

ctx.fillRect(player.x,player.y,player.w,player.h);

ctx.fillStyle="red";

for(let b of blocks){

ctx.fillRect(b.x,b.y,b.w,b.h);

}

ctx.fillStyle="white";

ctx.font="18px Arial";

ctx.fillText("Score : "+score,10,25);

if(gameOver){

ctx.fillStyle="yellow";

ctx.font="30px Arial";

ctx.fillText("GAME OVER",55,200);

}

}

function update(){

if(gameOver)return;

let speed=Math.min(12,4+score*0.1);

for(let i=blocks.length-1;i>=0;i--){

blocks[i].y+=speed;

if(

blocks[i].x<player.x+player.w &&

blocks[i].x+blocks[i].w>player.x &&

blocks[i].y<player.y+player.h &&

blocks[i].y+blocks[i].h>player.y

){

gameOver=true;

restart.style.display="block";

}

if(blocks[i].y>480){

blocks.splice(i,1);

score++;

}

}

}

function render(){

update();

draw();

requestAnimationFrame(render);

}

render();

setInterval(function(){

if(!gameOver)

spawnBlock();

},900);

function map(x,inmin,inmax,outmin,outmax){

return (x-inmin)*(outmax-outmin)/(inmax-inmin)+outmin;

}

setInterval(function(){

fetch("/data")

.then(r=>r.json())

.then(data=>{

let px=map(data.x,0,4095,0,280);

px=Math.max(0,Math.min(280,px));

player.x=px;

if(gameOver && data.sw==0){

location.reload();

}

});

},60);

</script>

</body>

</html>

)rawliteral");

}

// -------------------------------------------------

void setup() {

Serial.begin(115200);

pinMode(joySW, INPUT_PULLUP);

WiFi.begin(ssid,password);

Serial.print("Connecting");

int tries=0;

while(WiFi.status()!=WL_CONNECTED && tries<30){

delay(500);

Serial.print(".");

tries++;

}

if(WiFi.status()==WL_CONNECTED){

Serial.println();

Serial.println("Connected!");

Serial.print("IP Address: ");

Serial.println(WiFi.localIP());

}
else{

Serial.println();

Serial.println("WiFi Failed!");

return;

}

server.on("/",handleRoot);

server.on("/data",handleJoystick);

server.begin();

Serial.println("Server Started");

}

// -------------------------------------------------

void loop() {

server.handleClient();

}