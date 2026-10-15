#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
using namespace std;

const int PORT = 8080;

string readFile(const string& filepath){
    ifstream file(filepath);
    if(!file.is_open()) return "<h1>Error 404: File Not Found</h1>";
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(){
    //create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));//prevent addres in use error 

    //conf port and address
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    //bind & listen
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 3);
    
    cout<<"TV server is running on port "<<PORT<< "..." <<endl;

    //infinte loop for UI
    while(true){
        int new_socket = accept(server_fd, nullptr, nullptr);

        char buffer[1024] = {0};
        read(new_socket, buffer, 1024);//read request

        string html = readFile("index.html");//read html file
        string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " + 
                        to_string(html.length()) + "\r\n\r\n" + html;

        write(new_socket, response.c_str(), response.length());//send response
        close(new_socket);
    }
    return 0;
}
