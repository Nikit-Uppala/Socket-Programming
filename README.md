To run the server, in server folder run the commands - 
gcc server.c -o server
./server

To run the client, in client folder run the commands - 
gcc client.c -o client
./client

Buffer size chosen - 16000
If buffer is too high(eg: 100000), the data recieved by client is different and if the buffer size is too low then speed is very less.