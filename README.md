#Drupal DDP#
This module acts as a DDP (Distributed Data Protocol) client,
allowing Drupal to send new and updated node data to a Meteor application. 

Setup allows configuration of which content types push to Meteor apps.

Drupal DDP requires a node server using Node DDP Client.

Development of this module is sponsored by hb5

----

###DDP Magic###
Sends Nodes, Taxonomies, and Users (Coming soon) to DDP 
server on Insert, Update, and Delete.

The entire node object is sent to Node Server as a JSON Object.
