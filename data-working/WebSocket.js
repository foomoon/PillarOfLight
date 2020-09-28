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