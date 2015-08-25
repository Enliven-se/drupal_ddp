INTRODUCTION
------------
This module acts as a DDP (Distributed Data Protocol) client,
allowing Drupal to send new and updated node, taxonomy, and
user data to a Meteor application.


REQUIREMENTS
------------
* A server running Node (http://nodejs.org/).
* A Meteor application
  * Drupal DDP Meteor Package (https://github.com/hb5co/drupal-ddp).
* RestWS


INSTALLATION
------------
* Download and install module. See Instructions for Installing modules
  (https://www.drupal.org/documentation/install/modules-themes/modules-7).
* Navigate to module folder and run `node install`.
  * This will install all node dependencies
* Start the node server by running `node ddp.js`


CONFIGURATION
-------------
Setup allows configuration of which types of content push to Meteor apps.

* Navigate to `admin/config/development/drupal-ddp`
  * Specify what port your node server should be listening on
    * Defaults to Port 8080
  * Optionally set a Simple Security Token that enables simple authentication of
    ddp calls to your Meteor application.

* You may edit the `ddp.js` file to change settings to match what is in the UI.
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
    ...


MAINTAINERS
-----------
Current maintainers:
 * Bayo Fodeke (bfodeke) - https://www.drupal.org/u/bfodeke
 * Mark Shropshire (shrop) - https://www.drupal.org/u/shrop
 * Mike Barkas (mikebarkas) - https://www.drupal.org/u/mikebarkas


This project has been sponsored by:
 * hb5
   Development of this module is sponsored by hb5 (http://hb5.co).