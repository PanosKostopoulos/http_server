This is a custom http server. It is created without the usage of any external libraries for http just built upon pure tcp sockets. Its sole usage is to experiment with low-level c and networking concepts (sockets signals etc)
Usage of html,css,javascript etc are avoided due to danger of falling asleep on the keyboard.

For now this http server is able to serve only one client and only a simple get to get a text file

Future features:
    - support multiple clients probably using epolling
    - support more http features like post etc
    - connection to a database
    - support https (probably too complex)
    - make it as a service running in the background 

Usage: 
    - download the repo or just the http_server executable
    - run the executable like this ./http_server
    - in your browser go to localhost:6969/ for just a hello world response and /secret.txt to get a text file
