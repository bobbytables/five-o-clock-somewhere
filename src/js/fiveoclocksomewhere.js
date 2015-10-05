Pebble.addEventListener('ready',
  function(e) {
      var timeZoneOffset = new Date().getTimezoneOffset()/-60;
      console.log('offset: ' + timeZoneOffset);
      var dictionary = {
        'tzOffset': timeZoneOffset
      };
      Pebble.sendAppMessage(dictionary,
        function(e) {
            console.log('Zone info sent to Pebble successfully!');
        },
        function(e) {
            console.log('Error sending TZ info to Pebble!');
        }
      );
  }
);
