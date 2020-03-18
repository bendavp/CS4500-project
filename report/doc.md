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
- Value: 
	- field: char * serialized_information (can be anything)
- KV classes: 
	- field: Map * (maps keys to values)
	- function:
		- get(Key k) => Value, Gets a value from a KV-store based on the key
		- getAndWait(Key k) => Value, Gets a value from a KV-store based on the key and waits for that key to exist
		- put(Key k, Value v) => void, putting key-value pair into the KV-store
### Networking Layer (Nodes)
- we plan to start to work on troubleshooting the previous networking assignments in preparation for this; API will is TBD

## Use Cases
TBD

## Open Questions
- User interaction with the application layer?
- Where does the serialization/deserialization happen?

## Status
We discussed very high-level design decisions. We need to eventually implement the various layers and communication between the layers for the eau2 system.

For this milestone, we aim to implement functionality that will allow us to process a SOR data file into a DataFrame, and be able to use some basic Rowers on the DataFrame without issue. 

We will start to troubleshoot and fix the networking and serialization assignments from the past to prepare to integrate these into the system.
