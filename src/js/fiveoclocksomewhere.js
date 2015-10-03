Pebble.addEventListener('ready',
  function(e) {
      var timeZoneOffset = new Date().getTimezoneOffset() / -60;
      var dictionary = {
        'KEY_TEMPERATURE': timeZoneOffset
      };
      Pebble.sendAppMessage(dictionary,
        function(e) {
            console.log('TZ info sent to Pebble successfully!');
        },
        function(e) {
            console.log('Error sending TZ info to Pebble!');
        }
      );
  }
);
