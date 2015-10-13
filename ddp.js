/**
 * @file
 * DDP Node Server.
 */

// Setup server to respond to Drupal post submissions.
var express = require('express');
var bodyParser = require('body-parser');
var DDPClient = require("ddp");

var app = express();

var ddpclient = new DDPClient({
  host: "localhost",
  port: 3000,
  autoReconnect: true,
  autoReconnectTimer: 500,
  ddpVersion: '1',
  ssl: false,
  maintainCollections: false,
  useSockJs: true,
});

app.use(bodyParser.json());

// Setup routes for our nodejs listener.
app.get('/', function(req, res) {
  res.send('Ready for ddp connections!');
});

app.post('/', function(req, res, next) {
  console.log(req.headers);
  console.log(req.body);

  ddpclient.call('DrupalSaveNode', [req.body], function(err, response) {
    if (!err) {
      res.send(response);
    }
    else {
      res.send(err);
    }
  });

});

ddpclient.connect(function(error, wasReconnect) {
  // If autoReconnect is true, this callback will be invoked each time
  // a server connection is re-established
  if (error) {
    console.log('DDP connection error!');
    return;
  }

  if (wasReconnect) {
    console.log('Re-establishment of a connection.');
  }

  console.log('connected!');
});

// Monitor ddp connection for messages (for debugging purposes).
ddpclient.on('message', function (msg) {
  console.log("ddp message: " + msg);
});

// Nodejs listener setup for port 8080.
app.listen(8080);
console.log('Listening for data on port: 8080');

// Display when socket is closed.
ddpclient.on('socket-close', function(code, message) {
  console.log("Close: %s %s", code, message);
});
