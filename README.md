# simple-message-board-server
A message board server in C with multiplexing using select.

# Commands
"![message]/n" to overwrite the server message and "?/n" to get the most recent message

# Testing
To test this server on one machine:
  1. Compile the C code
  2. Run the .out file using PORT as an argument
  3. Using "screen" create a new window
  4. Using "nc _server-ip_ _PORT_" connect to the server as a client
  5. Repeat steps 3 and 4 for up to 30 clients
  
  
