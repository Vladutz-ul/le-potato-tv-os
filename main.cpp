#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
using namespace std;

const int PORT = 8080;

//determine MIME type based on file extension
string getMimeType(const string& filepath){
    if(filepath.length() >= 4 && filepath.substr(filepath.length() - 4) == ".css") return "text/css";
    if(filepath.length() >= 3 && filepath.substr(filepath.length() - 3) == ".js") return "application/javascript";
    if(filepath.length() >= 4 && filepath.substr(filepath.length() - 4) == ".png") return "image/png";
    return "text/html"; //default
}


//read file content
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
        char buffer[2048] = {0};
        read(new_socket, buffer, 2048);
        
        string request(buffer);
        if (request.empty()) { close(new_socket); continue; }

        string path = "index.html"; 
        size_t get_pos = request.find("GET ");
        size_t http_pos = request.find(" HTTP");
        
        if (get_pos != string::npos && http_pos != string::npos) {
            string req_path = request.substr(get_pos + 4, http_pos - (get_pos + 4));
            if (req_path != "/" && req_path.length() > 1) {
                path = req_path.substr(1); 
            }
        }

        string response;

        // API Endpoint for Hardware Temperature
        if (path == "api/temp") {
            string tempStr = readFile("/sys/class/thermal/thermal_zone0/temp");
            if (!tempStr.empty()) {
                float tempC = stof(tempStr) / 1000.0;
                char tempBuf[50];
                snprintf(tempBuf, sizeof(tempBuf), "{\"temp\": %.1f}", tempC);
                string json = tempBuf;
                response = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\nContent-Length: " + to_string(json.length()) + "\r\n\r\n" + json;
            } else {
                response = "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 0\r\n\r\n";
            }
        } 
        // Serve Standard Web Files
        else {
            string content = readFile(path);
            if (content.empty()) {
                response = "HTTP/1.1 404 Not Found\r\nContent-Length: 9\r\n\r\nNot Found";
            } else {
                response = "HTTP/1.1 200 OK\r\nContent-Type: " + getMimeType(path) + 
                           "\r\nContent-Length: " + to_string(content.length()) + 
                           "\r\n\r\n" + content;
            }
        }

        write(new_socket, response.c_str(), response.length());
        close(new_socket);
    }
    return 0;
}
