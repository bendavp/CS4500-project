# CS4500-project

## Introduction
On a high level, the eau2 system is capable of storing data in a distributed fashion, across a fixed number of nodes. The end goal is for an application to be run by the user which can access this data in a read-only manner.

## Architecture
The structure of the eau2 system is organized into three layers: application layer, KV-layer (KV-store), and the networking layer (nodes). 
### Application
- lives on a node
- can communicate with the KV-layer to pull values with keys
- the level that user will access to and can use to interact with the data (in KV-store/nodes)
### KV-store
- lives on a node
- can communicate with the nodes
- if user requests value from key not found in this KV-store (via application layer), then this KV-store will communicate with its node to request the relevant information from the correct node based on the key
### Node
- nodes are started up by a singular master node (alike to a server)
- fixed number of nodes which can communicate (alike to clients, connecting to the server)
- if there is any error in communication between nodes, system will shut down

## Implementation
### Main Utility Classes
- Object, String, Map, DataFrame, Sorer
### Application
- due to lack of specific needs from the client, this is TBD
### KV-store
- Key:	
	- fields: home node, unique identifier of a value on that value’s home node
	- function: 
		- getNode() => node that this key resides on
		- encode() => char*, converts key into serialized char* so it can be sent between nodes
		- decode(char*) => void, takes a serialized key (char*) and fills in the information from the serialized key into the key
		- hash() => int, from object
		- equals(Object*) => bool, from object
- Value: 
	- field: char * serialized_information (can be anything)
	- function:
		- encode(DataFrame*) => void, serializes the DataFrame* into char* and sets the field (serialized_) to this char*
		- decode() => Dataframe*, deserializes the field (serialized_) back into a DataFrame*
		- equals(Object*) => bool, from object class (checks if field is equivalent)
		- hash() => int, from object
- KV classes: 
	- fields: Key** (keys), Value** (values), size_ (number of pairs in the store), memory_size_ (max capacity of array)
	- function:
		- get(Key k) => Value, Gets a value from a KV-store based on the key
		- getAndWait(Key k) => Value, Gets a value from a KV-store based on the key and waits for that key to exist (to be implemented fully next week alongside networking layer)
		- put(Key k, Value v) => void, putting key-value pair into the KV-store
### Networking Layer (Nodes)
- we plan to start to work on troubleshooting the previous networking assignments in preparation for this; API will is TBD

## Use Cases
- WordCounter: Load a text file of words into the Application layer, KVstore layer will distribute the words among all available nodes, obtain the number of characters in each word (this work is distributed across multiple nodes), and aggregate the results to show to the application layer (i.e. the client)

## Open Questions
- User interaction with the application layer?

## Status
We discussed the classes that will make up the kvstore layer, and focused mostly on this layer for this milestone. We decided to make our Value and Key class be able to serialize/deserialize themselves (in the case of Value, it represents a serialized DataFrame, and when deserialized, it will be a DataFrame)

For this milestone, we aimed to implement functionality that will allow us to encode/decode DataFrames into a char* as well as store these char* with Keys in the KV-store. Our goal is to get this working as if there is only one node. More specifically, we aim to be able to pass in these serialized DataFrames into the KV-store with a Key and retrieve out the Value/DataFrame with the Key. 

We will start to troubleshoot and fix the networking from the past to prepare to integrate this into the system.

# pls gib 100 ty :-)

![Hosico](https://www.catster.com/wp-content/uploads/2017/02/Hosico1.png)