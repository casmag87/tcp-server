#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <thread>
#include <fcntl.h>
constexpr int MAX_EVENTS = 10;
constexpr int MAX_CLIENTS = 10;
constexpr int PORT = 9001;

void handleClient(int client_fd) {
    char buf[4096];

    while (true) {
        int bytesReceived = read(client_fd, buf, sizeof(buf));
        
        if (bytesReceived > 0) {
            // Successfully received data
            std::cout << std::string(buf, 0, bytesReceived) << std::endl;

            // Echo message back to client
            int bytesSent = send(client_fd, buf, bytesReceived, 0);
            if (bytesSent == -1) {
                std::cerr << "Error sending to client: " << std::strerror(errno) << std::endl;
                close(client_fd);
                return;
            }

        } else if (bytesReceived == 0) {
            // Client disconnected
            std::cout << "Client disconnected" << std::endl;
            close(client_fd);
            return;

        } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // No more data to read right now
                break;
            } else {
                // Some other error
                std::cerr << "Error reading from client: " << std::strerror(errno) << std::endl;
                close(client_fd);
                return;
            }
        }
    }

    
int main(){

///Create a socket
int server_socket_fd = socket(AF_INET,SOCK_STREAM,0);
if (server_socket_fd == -1)
    {
	
	std::cerr << "Failed to create socket: " << std::strerror(errno) << std::endl;
	return EXIT_FAILURE;	
    
    }     

int reuse = 1;
setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));    

sockaddr_in server_addr{};
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(PORT);
inet_pton(AF_INET,"0.0.0.0", &server_addr.sin_addr);


///Bind the socket to ip port 
if (bind(server_socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
    std::cerr << "Failed to bind port: " << std::strerror(errno) << std::endl;
    close(server_socket_fd);
    return EXIT_FAILURE;
    }


///mark the socket for listning 
if (listen(server_socket_fd,SOMAXCONN) == -1){
    std::cerr << "Failed listen to: " << std::strerror(errno) << std::endl;
    close(server_socket_fd);
    return EXIT_FAILURE;
}

int epollFd = epoll_create1(0);
if (epollFd == -1) {
    std::cerr << "Failed to create epoll: " << std::strerror(errno) << std::endl;
    close(server_socket_fd);
    return EXIT_FAILURE;
}

struct epoll_event ev, events[MAX_EVENTS];
///add server
ev.events = EPOLLIN;
ev.data.fd = server_socket_fd;
if(epoll_ctl(epollFd, EPOLL_CTL_ADD,server_socket_fd,&ev) == -1){
    std::cerr << "failed to add server: " << std::strerror(errno) << std::endl;
    return EXIT_FAILURE;
}

for(;;){
    int nfds= epoll_wait(epollFd, events, MAX_EVENTS, -1);
    if (nfds == -1){
        std::cerr << "failed to add server: " << std::strerror(errno) << std::endl;
        return EXIT_FAILURE;        
    }

    for(int i = 0; i < nfds; ++i){
        if(events[i].data.fd == server_socket_fd){
        //Accept call
            sockaddr_in client_addr{};
            socklen_t client_len = sizeof(client_addr);
            int client_fd = accept(server_socket_fd, (struct sockaddr*)&client_addr, &client_len);  
            if (client_fd == -1)
            {
                std::cerr << "Failed client connecting: " << std::strerror(errno) << std::endl;
      
                continue;
            }

            int flags = fcntl(client_fd, F_GETFL, 0);
            fcntl(client_fd, F_SETFL, flags | O_NONBLOCK);
            
            ev.events = EPOLLIN | EPOLLET;
            ev.data.fd = client_fd;
            if (epoll_ctl(epollFd, EPOLL_CTL_ADD, client_fd,
                        &ev) == -1){
                 std::cerr << "Failed: " << std::strerror(errno) << std::endl;
                 exit(EXIT_FAILURE);           
            }
        }              
        else {
            handleClient(events[i].data.fd);
        }

            ///While receving- display message, exho message
            ///handleClient(client_fd);
            
    
    }

}


close(epollFd);
///Close the listning socket
close(server_socket_fd);







    }



