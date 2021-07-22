# Socket programming
### Description
Client connects to the server and send commands to download files present in server directory in the client's directory.  
Commands format -  
get _filename_ - downloads the file _filename_ if present in server directory.  
get _file1_ _file2 ... - downloads the files _file1 _file2 .. if present in server directory.  
exit - to close the connection and stop the execution of the client.
### Instructions to compile and run the files
To run the server, in server directory run the commands -  
gcc server.c -o server  
./server
  
To run the client, in client directory run the commands -  
gcc client.c -o client  
./client  

Buffer size chosen - 16000
