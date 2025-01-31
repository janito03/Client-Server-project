#include <iostream>
#include <string>
#include <cstring>
#include <winsock2.h> //за използване на разшитените възможности на Winsock 2
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

#define PORT 55555 //free ports: 49152-65353

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { //Winsock must be initialised explicitly for the program to be able to use the functions of the Winsock libriry
        cerr << "WSAStartup failed." << endl;
        return 1;
    }

    SOCKET sock = 0;
    struct sockaddr_in clientAddr;
    memset(&clientAddr, 0, sizeof(clientAddr));
    char buffer[1024] = { 0 };

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) { //creates a socket; TCP cnnection
        cerr << "Socket creation error" << endl;
        WSACleanup();
        return 1;
    }

    clientAddr.sin_family = AF_INET; //IPv4
    clientAddr.sin_port = htons(PORT); //converts the port number; host byte order -> network byte order (big-endian)

    if (inet_pton(AF_INET, "127.0.0.1", &clientAddr.sin_addr) <= 0) { //converts the IP into binary
        cerr << "Invalid address/ Address not supported" << endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }


    if (connect(sock, (struct sockaddr*)&clientAddr, sizeof(clientAddr)) == SOCKET_ERROR) {
        cerr << "Connection failed" << endl;
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    cout << "Connected to server. Type text to send (type 'exit' to quit):" << std::endl;

    cout << "Hello to Ceser Cipher. Choose what you want: to encrypt the text (\"encr\" followed by the text) or to decrypt the text(\"decr\" followed by the text)" << endl;


    while (true) {
        string input;
        getline(cin, input);

        if (input == "exit") {
            break;
        }

        send(sock, input.c_str(), input.size(), 0);

        memset(buffer, 0, sizeof(buffer)); //clears the buffer
        int valread = recv(sock, buffer, 1024, 0);

        if (valread > 0) {
            std::cout << "Server response: " << buffer << std::endl;
        }
        else {
            std::cout << "Server disconnected." << std::endl;
            break;
        }
    }

    closesocket(sock);
    WSACleanup(); //cleans up resourses used by Winsock
    return 0;
}
