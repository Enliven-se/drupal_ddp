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
  // host: "localhost",
  port: 3000,
  // autoReconnect: true,
  // autoReconnectTimer: 500,
  // ddpVersion: '1',
  // ssl: false,
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

  ddpclient.call(
    'DrupalSaveNode',
    [req.body]
  );

  res.send({ status: 'Post successfully sent to Meteor!' });
});

ddpclient.connect(function(err) {
  if (err) {
    throw new Error('DDP connection error!');
  }
  console.log('Connected!');
});

// Nodejs listener setup for port 8080.
app.listen(8080);
console.log('Listening for data on port: 8080');


// Monitor ddp connection for messages (for debugging purposes).
ddpclient.on('message', function (msg) {
  console.log("ddp message: " + msg);
});
