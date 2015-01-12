# SQLite3UE4
SQLite3 Database Plugin for Unreal Engine 4

## What is it?
A plugin for UE4 that provides code and blueprint functionality that enables you to use SQLite3 databases in your projects.

It heavily uses reflection, which makes it possible to eg. create blueprints with member properties (variables) and then populate their values directly from the database, provided that the table field names match the UObject property names and have compatible data types.

Currently heavily under construction, not really worthy of anything else than testing. Use at your own risk.