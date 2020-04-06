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
- Word Counter:
	1. Uses a FileReader to read in a given file
		- splits this file into smaller dataframes and stores it as values with different keys
		- keeps track of the keys and the ordering (so we can rebuild the dataframe if necessary)
	2. Counts the number of words in each Node and stores that as a Value (when deserialized: first column being the word and the second column being the word count)
	3. Merges the counts into the master Node (by retrieving Key's Value) and prints each word and its count
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
- KVstore: 
	- fields: Key** (keys), Value** (values), size_ (number of pairs in the store), memory_size_ (max capacity of array)
	- function:
		- get(Key k) => Value, Gets a value from a KV-store based on the key
		- getAndWait(Key k) => Value, Gets a value from a KV-store based on the key and waits for that key to exist (to be implemented fully next week alongside networking layer)
		- put(Key k, Value v) => void, putting key-value pair into the KV-store
- DataFrame:
	- this is not a comprehensive list of all fields/methods but rather methods which incoporate the KV-store layer
	- function:
		- fromArray(Key *, kvstore *, size_t, float *) => DataFrame, constructs DataFrame with 1 column containing the given size_t number of given floats; adds this DataFrame (serialized) as a Value into the given kvstore with the given key
		-fromScalar(Key *, kvstore *, float) => DataFrame, constructs a DataFrame with 1 column containing the given float; adds this DataFrame (serialized) as a Value into the given kvstore with the given key
### Networking Layer (Nodes)
- Node: (is a Thread)
	- fields: network_ (PseudoNetwork communication layer), kvstore, index_, lock_, reply_ (Reply *)
    - function:
		- run() => void, continues to process messages until it receives a kill message (then node terminates via join())
		- add(Key *, Value *) => void, adds the given Key/Value into its kvstore if the Key belongs to this node; otherwise, sends a message to the node with the key/value to add through the network layer
		- get(Key *) => Value *, gets the given Value from the kvstore if the Key belongs to this node; otherwise, sends a message to the node with the key/value and waits for a Reply message
		- waitAndGet(Key *) => Value *, waits for the given Key to exist then returns the Value associated if the Key belonds to this node; otherwise, sends a message to the node with the key/value and wait for a Reply message
- PseudoNetwork:
	- fields: FastArray of MessageQueues (stores the MessageQueues for each node), StringSztMap (maps Thread Ids (from String) to a size_t), current_node_idx (stores which node_idx we are connected with/to)
	- function:
		- register_node() => void, creates and adds a new node to this communication layer
		- check_next_node_idx() => size_t, checks what the index of the next node would be if there was one added
		- send_m(Message *m) => void, sends Message *m by adding it to the appropriate MessageQueue (node idx corresponds with MessageQueue index)
		- recv_m() => Message *, pops a Message off of the appropriate MessageQueue and returns it
### Utility/Helper classes
- MessageQueue: functions as a Queue of Messages with a Lock
- StringSztMap: Hashmap for String and size_t	

## Use Cases
- WordCounter: Load a text file of words into the Application layer, KVstore layer will distribute the words among all available nodes, obtain the number of characters in each word (this work is distributed across multiple nodes), and aggregate the results to show to the application layer (i.e. the client)
- We will be adding more specific use cases (with code examples) for **next week's** submission

## Open Questions
- How does fopen work? (getting error: unable to read file)
	- We attempted to find the cwd and change our input file to work with this directory >> seg faulted because file could not be found
	- We attempted to input the absolute path to the file >> seg faulted again because file could not be found

## Status
### Outstanding Issue
We are **very aware** that we need to do refactoring of our code which we are using from previous assignments. Due to the time we're using to implement new features (such as the networking (pseudonetwork) layer), we have decided to hold off on refactoring more than necessary until Milestone 5. In the case of our implementations from now going forward, we have refactored/lowered the amount of duplicated code as we are implementing these new features.
### General Update
(From this week)
We tried to implement the example application, WordCounter, for this week's milestone. We mostly fitted Jan's code to ours and tried to format the application such that each application holds a node, and each application is its own thread. That way, each application can do work on it's paired Node (and KVstore) simultaneously, making the application run faster. Unfortunately, we are running into an error with FileReader (see: Open Questions).

(From last week)
We decided to implement a pseudonetworking layer instead of a true networking layer. We feel that given the timeframe of these assignments and our lack of prior knowledge/general understanding of networks, this is going to be the best approach for us. We were able to mostly complete a basic implementation of the pseudonetwork for this week, however, we are a little behind in that we have not thoroughly tested it yet, nor have we implemented dividing the file into chunks and spreading those chunks across the network, although we _have_ implemented a way for the network to store these chunks. We have also included a KDstore in the application layer, which is a Map that stores Keys mapped to Keys, the idea being that the value of the KDstore will actually be a Key that represents a DataFrame, but is actually an array of more Keys. The reasoning behind this is that the Key array points to other keys spread across the nodes in the network that contain a chunk of the desired DataFrame. So, when the Keys in the Key array are put together, we get the whole DataFrame.

Another goal we reached this week was the inclusion of C++ templates in our code to reduce duplication, especially in our `utils` classes. Queue and FastArray are now templates, as well as Map which is actually 2 templates (one for primitive->`Object*` mapping, and another for `Object*`->`Object*`). The reason that the primitive FastArrays are still there is because it would be a rather large effort to refactor everywhere our current codebase uses the primitive FastArrays to use a primitive template, however this is an effort we will undergo in the future (see Outstanding Issue, above). 
