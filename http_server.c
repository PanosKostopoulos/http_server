#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <math.h>

#define PORT 6969
#define RESPONSE "HTTP/1.1 200 OK\r\nContent-Length: 13\r\nContent-Type: text/plain\r\n\r\nHello, World!"

/* should check IF incoming packet is actually a http packet */
int num_of_digits(int n){
    if(n<16) return 1;
    if(n<256) return 2;
    if(n<4096) return 3;
}


char * directory_parser(char *buffer, int read_counter){
    int i=0;
    while(*(buffer+i)!='/'){ i++;}
    int start = i+1;
    while(*(buffer+i)!=' '){ i++;}
    int end = i;
    char * directory = malloc((end-start)*sizeof(char)+1);
    for(int i=start;i<end;i++){
        *(directory+i-start) = *(buffer+i);
    }
    *(directory+i)= EOF;
    
    return directory;
        
}
/*some error should be handled with http 404.i will fix it soon*/
void send_file(char * directory, int socket){
    FILE *fp;
    fp = fopen(directory,"r");
    if (fp==NULL){
        perror("file could not be openned");
        exit(EXIT_FAILURE);
    }
    char * buffer = malloc(1024*sizeof(char));
    int bytes_read=0;
    int sc =0; 
    while ((bytes_read=fread(buffer,1,1024,fp))> 0){
        printf("i read %i characters\n",bytes_read);
        
        int hex_len = num_of_digits(bytes_read);
        char * chunk_size;
        chunk_size = malloc((hex_len+2)*sizeof(char));
        sprintf(chunk_size,"%x",bytes_read);
        chunk_size[hex_len]='\r';
        chunk_size[hex_len+1]='\n';
        if((sc = send(socket, chunk_size, hex_len+2,0))<0){
            perror("send of chunk size failed");
            exit(EXIT_FAILURE);
        }
        printf("sent %i characters\n",sc);
        if((sc = send(socket, buffer, bytes_read,0))<0){
            perror("send of chunk size failed");
            exit(EXIT_FAILURE);
        } 
        printf("sent %i characters\n",sc);
 
    }
    char * end_chunk = "0\r\n";
    if((sc = send(socket, end_chunk, 3,0))<0){
            perror("end of chunkfailed");
            exit(EXIT_FAILURE);
    } 
    printf("sent %i characters\n",sc);

    printf("file has been sent\n");

}

int main() {
	
    int server_fd, client_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
	char buffer[1024];
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 1) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }
	char * directory = NULL;
	while(1){
		int rc = 0;
        rc = read(client_socket,buffer,sizeof(buffer));
			
		if(rc < 1){
			perror("read failed");
        }
        else if(rc == 0){
            ;
        }
        else{
            for(int i=0;i<rc;i++){
                printf("%c",buffer[i]);
            }
            directory = directory_parser(buffer, rc);
            int i=0;
            printf("this is the directory: ");
            while(*(directory+i)!=EOF){
                printf("%c",*(directory+i));
                i++;
            }
            printf("\n");
            break;
        }
	}
    if(directory==NULL){	
 
        char *message = "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/plain\r\n"
                        "Content-Length: 13\r\n"
                        "\r\n"
                        "Hello, world!";
     
        if (send(client_socket, message, strlen(message), 0) < 0){
    	    perror("send failed");
	    exit(EXIT_FAILURE);
        }
    }
	else{
        char *message = "HTTP/1.1 200 OK\r\n"
                        "Content-Type: text/plain\r\n"
                        "Transfer-Encoding: chunked\r\n"
                        "\r\n";
        if (send(client_socket, message, strlen(message), 0) < 0){
    	    perror("send failed");
	    exit(EXIT_FAILURE);
        }

        send_file(directory, client_socket);
    }
    close(client_socket);

    return 0;
}
