Drone
**README:  Messaging System**

### Overview:

The provided code is a simple messaging system that enables communication between different nodes. Messages can be sent, received, forwarded, and acknowledged based on various conditions.

### Core Features:

1. **Distance Computation**: Computes the distance between the sender and the receiver using their coordinates.
2. **Message Handling**:
   - Checks if a message is destined for the current node.
   - If the message is for the current node and within a defined `MAXDISTANCE`, the message is processed. Otherwise, it is ignored.
   - Duplicate acknowledgments are tracked and identified.
3. **TTL (Time To Live)**: Messages have a TTL. If the TTL reaches 0, the message is discarded.
4. **Forwarding**: If a message is not destined for the current node, it may be forwarded to partners.
5. **Token Management**: Messages are tokenized for better management. Functions are available to find, print, and modify tokens.

### Utility Functions:

1. **Isdigit**: Verifies if the provided string represents a number.
2. **checkPort**: Validates if the provided port number is valid.
3. **checkip**: Verifies if the given string is a valid IP address.
4. **findDistance**: Computes the distance between two points on a grid.
5. **findXYCoordinates**: Converts a square number to its corresponding row and column.
6. **receiveData**: Receives data from a socket.
7. **clearmsg**: Cleans up the message for proper tokenization.
8. **sendData**: Sends data through a socket.
9. **forwardSend**: Forwards a message to partners.
10. **findTokens**: Tokenizes a message.
11. **findIntToken**: Retrieves an integer token value by its key.
12. **ChangeIntToken**: Modifies a token's value.
13. **combine**: Combines two strings.
14. **BuildAck**: Constructs an acknowledgment message.

### Usage:

Ensure you have a compatible C environment to compile and run the program. The code interacts with messages based on their type, TTL, and other parameters. It's important to adhere to the messaging format for the correct operation of the program.

### Notes:

- The code assumes the existence of some variables and constants like `MAXDISTANCE`, `COLUMNS`, `ROWS`, and others which aren't provided in the snippet.
- Make sure to provide a complete implementation, including header files, dependencies, and other necessary constants for the system to work.
- Ensure that the system is running in an environment that supports socket programming as it is essential for sending and receiving messages.
- The `...` at the end of the provided code indicates that there might be more code or functions that weren't shown. Ensure you integrate all components for the program to function correctly.

