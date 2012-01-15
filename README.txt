Remote import - Hostmaster
==========================

This Drupal module provides a UI for fetching sites from remote Aegir servers.

Installation
------------

Install this module like any other, and enable in the usual way.

You also need to install the backend portion of this extension for this to work
correctly.

Usage
-----

You'll need to add the remote Aegir server as any other server in the frontend,
selecting the 'hostmaster' remote service as you do so. This means of course
that you'll need to add your ssh key to this server and set it up like other
remote servers. Note that you don't need to install anything other than the SSH
key on this server.

A general remote server setup guide can be found here:
http://community.aegirproject.org/node/30

Once you've set up your server in the frontend you should get a new menu item
called: 'Import remote sites', click on that link and follow the instructions
there.

See also
--------

You probably want to install the Remote import - Provision project into your
Aegir backend.
