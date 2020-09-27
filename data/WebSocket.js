var connection;

(function wsconnect() {
    connection = new WebSocket('ws://'+location.hostname+':81/', ['arduino']);

    connection.onopen = function () {
        console.log('WebSocket Opened ');
    };
    connection.onerror = function (error) {
        console.log('WebSocket Error ', error);
    };
    connection.onmessage = function (e) {  
        console.log('Server: ', e.data);
    };
    connection.onclose = function(){
        console.log('WebSocket connection closed');
        console.log ('Attempting to reconnect...');
        setTimeout(function() {
            wsconnect();
        }, 1000);
    };

})()


//wsconnect();

// Async HTTP Request
// let request = obj => {
//     return new Promise((resolve, reject) => {
//         let xhr = new XMLHttpRequest();
//         xhr.open(obj.method || "GET", obj.url);
//         if (obj.headers) {
//             Object.keys(obj.headers).forEach(key => {
//                 xhr.setRequestHeader(key, obj.headers[key]);
//             });
//         }
//         xhr.onload = () => {
//             if (xhr.status >= 200 && xhr.status < 300) {
//                 resolve(xhr.response);
//             } else {
//                 reject(xhr.statusText);
//             }
//         };
//         xhr.onerror = () => reject(xhr.statusText);
//         xhr.send(obj.body);
//     });
// };

// var data = {
//     deviceID: 1,
//     // color: "#0000ff",
//     // hsv: {
//     //     h: 0,
//     //     s: 50,
//     //     v: 100
//     // },
//     brightness: 32,
//     spark: 50,
//     cooling: 50,
//     text: " ",
// };

// function $(id) {
//     return document.getElementById(id);
// }

// function sendRGB() {
//     var r = $('r').value**2/255;
//     var g = $('g').value**2/255;
//     var b = $('b').value**2/255;
    
//     var rgb = r << 16 | g << 8 | b;
//     var rgbstr = '#'+ rgb.toString(16);  
//     data.color = rgbstr;  
//     console.log('RGB: ' + rgbstr); 
//     connection.send(rgbstr);
// }

// function sendBrightness() {
//     data.brightness = $('brightness').value;
//     sendDataAsJSON();
// }

// function setBrightness() {
//     $('brightness').value = Number(data.brightness);
// }

// function sendSpeed() {
//     data.speed = Number($('speed').max) - Number($('speed').value) + Number($('speed').min);
//     sendDataAsJSON();
// }

// function setSpeed() {
//     $('speed').value = Number($('speed').max) - Number(data.speed) + Number($('speed').min);
//     sendDataAsJSON();
// }

// function sendSpark() {
//     data.spark = $('spark').value;
//     sendDataAsJSON();
// }

// function setSpark() {
//     $('spark').value = Number(data.spark);
// }

// function sendCool() {
//     data.cooling = $('cooling').value;
//     sendDataAsJSON();
// }

// function setCool() {
//     $('cooling').value = Number(data.cooling);
// }

// function sendText() {
//     var txt = $('message').value;
//     if (txt.length==0) {
//         txt = "...";
//     }
//     data.text = txt;
//     sendDataAsJSON();
// }

// function sendDataAsJSON() {
//     data.timeStamp = ' ' + new Date();
//     // $("dataField").value=(JSON.stringify(data," ",4));
//     console.log(data);
//     connection.send(JSON.stringify(data));
// }
