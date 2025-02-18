# SQLite Plugin for Unreal Engine 5.5

A lightweight plugin that provides SQLite database functionality in Blueprint.

## Features
- Asynchronous database operations
- Blueprint support
- Error handling
- Prepared statements support
- Support for all basic SQL operations (SELECT, INSERT, UPDATE, DELETE)
- Results as key-value pairs
- Thread-safe operations

## Requirements

- Unreal Engine 5.5
- SQLiteCore plugin must be enabled in your project (Edit -> Plugins -> Database -> SQLite Core)

If SQLite Core plugin is not available in your project:
1. Open Epic Games Launcher
2. Go to Unreal Engine -> Library
3. Find your engine version (5.5)
4. Click on the '+' icon next to the engine version
5. Select 'Add Components'
6. Enable 'Database Support' component
7. Click 'Install'
8. Restart the Unreal Editor

## Installation

1. Create `Plugins` folder in your project root if it doesn't exist
2. Copy `BP_SQLitePlugin` folder into `Plugins`
3. Rebuild the project
4. Enable the plugin in Edit -> Plugins -> Custom -> BP SQLite Plugin

## Working with SQLite Node Component

### Adding Component
1. In your Blueprint, click "Add Component" button
2. Search for "BP SQLite Node"
3. Add it to your actor (it's a non-scene component, so it won't have physical representation)

### Connecting to Database

The `Connect to Database` node has following pins:
- **Target**: Your BP SQLite Node component reference (automatically connected when you drag from component)
- **Database Path**: String with path to your database file (e.g., "Content/Database/your_database.db")
- **Callback**: Event that will be called after connection attempt

You can use `Get Project Content Directory` in development mode for construct path.


### Setting up Callback

1. Right click on empty space in your Blueprint
2. Create Custom Event (name it something like "OnDatabaseConnected")
3. Add input parameter to this event:
  - Name: Success
  - Type: Boolean
4. Connect this Custom Event to the Callback pin of Connect to Database node

### Important Notes
- Always check Success parameter in callback
- Connection is asynchronous, so any database operations should be done after successful connection
- Keep reference to your BP SQLite Node component if you need to use it in other events
- Connection callback will be executed on the game thread, so it's safe to update UI or game state

## Working with Query Execution

### Processing Connection and Executing Queries

After adding SQLite Node component and setting up connection, handle the connection result:
[Custom Event: OnDatabaseConnected]
Input: Success (Boolean)
|
[Branch] <- Success
|
True -> [Execute Query] (See Query Execution below)
False -> [Print String] "Database connection failed"

### Execute Query Node

The `Execute Query` node has following pins:
- **Target**: Your BP SQLite Node component reference
- **Query**: SQL query string (e.g., "SELECT * FROM Players")
- **Parameters**: Map of String, String for query parameters (may be an empty map)
- **Is Insert or Update**: Boolean, true for INSERT/UPDATE/DELETE, false for SELECT
- **Callback**: Event that will be called after query execution

### Setting up Query Callback

1. Create Custom Event for query result
2. Add input parameters:
   - Result (String): Operation result or error message
   - Rows (Array of Query Result Row): Query results
   - Affected Rows (Integer): Number of affected rows

### Processing Query Results

For SELECT queries (Is Insert or Update = false):
[Custom Event: OnQueryExecuted]
Inputs:

Result (String)
Rows (Array of Query Result Row)
Affected Rows (Integer)
|
[ForEach Loop] <- Rows
|
[Get Values] <- Array Element
|
[For Each Loop (Map)]
- Array Element Key (column name)
- Array Element Value (column value)

For INSERT/UPDATE queries (Is Insert or Update = true):
[Custom Event: OnQueryExecuted]
|
[Branch] <- Affected Rows > 0
|
True -> Operation successful
False -> Check Result string for error

![Example](https://github.com/HomeRobot/BP_SQLite3/blob/main/blue_print.png)

### Important Notes
- Always provide Parameters Map (use empty Map for queries without parameters)
- Check Result string for error messages
- Rows array will be empty for INSERT/UPDATE operations
- Affected Rows will be 0 for SELECT operations
- All operations are asynchronous
