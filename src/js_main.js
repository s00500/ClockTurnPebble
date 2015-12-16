Pebble.addEventListener('ready',function(e) {
   console.log('JavaScript app for ClockTurn ready and running!');
   Pebble.addEventListener('appmessage',function(e) {
      //console.log('Received message: ' + JSON.stringify(e.payload));
      //Setup the pi request!    
      var req = new XMLHttpRequest();
      req.onload = function(e) {
         if(req.readyState == 4 && req.status == 200) {
            //console.log('Sent');
            //success, add acc for appmessage here 
            //var response = JSON.parse(req.responseText);
            //Pebble.sendAppMessage({ 'icon':icon, 'temperature':temperature + '\u00B0C'});
         } else {
            console.log('Error on sending Post request'); 
         }
      };
         //console.log('lol:' +e.payload[0]);

         switch (e.payload[0]) {
         case 0:
            console.log('command 0 - forward'); 
            req.open('POST', 'http://192.168.0.205:8080/forward', true);
				req.send();
            break;
         case 1:
            console.log('command 1 - normal'); 
            req.open('POST', 'http://192.168.0.205:8080/normal', true);
            req.send();
            break;
         case 2:
            console.log('command 2 - backward'); 
            req.open('POST', 'http://192.168.0.205:8080/backward', true);
            req.send();
            break;
         case 3:
            console.log('command 3 - ignite'); 
            req.open('POST', 'http://192.168.0.205:8080/ig3', true);
            req.send();
            break;
         default:
            break;
      }

   });	
});

