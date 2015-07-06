#Drupal DDP#
This module acts as a DDP (Distributed Data Protocol) client,
allowing Drupal to send new and updated node, taxonomy, and
user data to a Meteor application.

Setup allows configuration of which types of content push to Meteor apps.

Drupal DDP requires a node server using Node DDP Client.

Development of this module is sponsored by [hb5](http://hb5.co)

----

###Requirements###
* A server running [Node.js](http://nodejs.org/).
* A meteor application
* [Drupal DDP Meteor Package](https://github.com/hb5co/drupal-ddp).
*not necessarily required*

###Installation###
* Download and install module. See [Instructions for Installing modules](https://www.drupal.org/documentation/install/modules-themes/modules-7)
* Navigate to module folder and run `node install`.
 * This will install all node dependencies
* Run `node ddp.js`

###Setup###
* Navigate to `admin/config/development/drupal-ddp`
* Specify what port your node server should be listening on
  ...
  // Nodejs listener setup for port 8080.
  app.listen(8080);
  console.log('Listening for data on port: 8080');
  ...
* Also specifiy the URL of your meteor app
  ...
  var ddpclient = new DDPClient({
    // host: "localhost",
    port: 3000,
    ...
    ...
  });
* Optionally set a Simple Security Token that enables simple authentication of ddp calls to your Meteor application.
