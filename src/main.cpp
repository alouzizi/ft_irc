/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hboumahd <hboumahd@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/24 08:46:07 by hboumahd          #+#    #+#             */
/*   Updated: 2023/02/27 12:58:25 by hboumahd         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ircserv.hpp"

void doProcessing(int newSocket)
{
    int n;
    char buffer[1024];

    // receives client message 
    bzero(&buffer, sizeof(buffer));
    n = recv(newSocket, buffer, sizeof(buffer), 0);
    if (n < 0)
        fun_error("Error receiving from client socket", 1, -2);

    std::cout << "client: " << buffer;
}

int main(int ac, char **av)
{
    int serverSocket, newSocket, portno;
    struct sockaddr_in server_addr, client_addr;
    std::vector<pollfd> pollfds;
    int rc;
    
    if (ac == 3)
    {
        //  create server socket and initialize it
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket < 0)
            fun_error("Error building socket", 1, serverSocket);
        
        bzero((char *) &server_addr, sizeof(server_addr));
        portno = atoi(av[1]);
        
        server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(portno);

        // here we allow the server socket fd to be reusable
        int optval = 1;
        if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) 
            fun_error("Error on setsockopt.", 1, serverSocket);

        // here we set the server socket to be nonbloking
        if (fcntl(serverSocket, F_SETFL, O_NONBLOCK) == -1)
            fun_error("Error on fcntl", 1, serverSocket);

        // bind the server host address
        if (bind(serverSocket, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
            fun_error("Error on binding host adress.", 1, serverSocket);
        

        // listening for the clients (wait for incoming connection from the client)
        if (listen(serverSocket, 2) < 0)
            fun_error("Error on binding host adress.", 1, serverSocket);
        
        pollfds.push_back({serverSocket, POLLIN, 0});
        int timeout = (3 * 1000);
        // establish connections and start communication
        do
        {
            // call poll() and wait
            rc = poll(pollfds.data(), pollfds.size(), timeout);

            // check if poll() call failed
            if (rc < 0)
            {
                std::cout << "poll() call failed!\n";
                break;
            }
            
            // check to see if time out
            if (rc == 0)
            {
                std::cout << "poll() call time out!\n";
                break;
            }
            
            // determine the readable fds
            for (size_t i = 0; i < pollfds.size(); i++)
            {
                if (pollfds[i].revents == 0)
                    continue;

                if (pollfds)
                
            }
            
            

        } while (newSocket != -1);
        

    }
    else
        std::cout << "=> please enter: ./ircserv: <port> <password>\n";
    return 0;
}