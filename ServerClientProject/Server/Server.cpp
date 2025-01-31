#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <algorithm>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define PORT 55555
#define KEY 3

using namespace std;

string decryptWord(const string& str) {
    string normalStr = "";
    size_t len = str.length();
    normalStr.resize(len);

    for (size_t i = 0; i < len; i++) {
        int k = str[i] + KEY;
        if (isupper(str[i])) {
            if (k > 'Z') {
                k = 'A' + (k - 'Z' - 1);
            }
        }
        else {
            if (k > 'z') {
                k = 'a' + (k - 'z' - 1);
            }
        }
        normalStr[i] = (char)k;
    }
    return normalStr;
}

string encryptWord(const string& str) {
    string normalStr = "";
    size_t len = str.length();
    normalStr.resize(len);

    for (size_t i = 0; i < len; i++) {
        int k = str[i] - KEY;
        if (isupper(str[i])) {
            if (k < 'A') {
                k = 'Z' - ('A' - k - 1);
            }
        }
        else {
            if (k < 'a') {
                k = 'z' - ('a' - k - 1);
            }
        }
        normalStr[i] = (char)k;
    }
    return normalStr;
}

vector<string> splitStringBySpaces(const string& str) {
    vector<string> words;
    size_t start = 0;
    size_t end = str.find(' ');

    while (end != string::npos) {
        words.push_back(str.substr(start, end - start));
        start = end + 1;
        end = str.find(' ', start);
    }
    words.push_back(str.substr(start));

    return words;
}

string CeserEncrypt(const string& str) {
    string encrypted = "";
    vector<string> words = splitStringBySpaces(str);
    size_t count = words.size();
    for (int i = 0; i < count; i++) {
        encrypted.append(encryptWord(words[i]));
        encrypted.append(" ");
    }
    encrypted.pop_back();
    return encrypted;
}

string CeserDecrypt(const string& str) {
    string decrypted = "";
    vector<string> words = splitStringBySpaces(str);
    size_t count = words.size();
    for (int i = 0; i < count; i++) {
        decrypted.append(decryptWord(words[i]));
        decrypted.append(" ");
    }
    decrypted.pop_back();
    return decrypted;
}

// Function to handle a single client
void handleClient(SOCKET client_socket) {
    char buffer[1024] = { 0 };
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int valread = recv(client_socket, buffer, 1024, 0);
        if (valread <= 0) {
            cout << "Client disconnected." << endl;
            closesocket(client_socket);
            return;
        }

        string text(buffer);
        cout << "Received: " << text << endl;

        size_t pos = text.find(' ');
        string firstWord = text.substr(0, pos);
        text = text.substr(pos + 1);

        if (firstWord == "encr") {
            text = CeserEncrypt(text);
        }
        else if (firstWord == "decr") {
            text = CeserDecrypt(text);
        }
        else {
            text = "Operation not specified!";
        }

        send(client_socket, text.c_str(), text.size(), 0);
        cout << "Sent: " << text << endl;
    }
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed." << endl;
        return 1;
    }

    SOCKET server_fd;
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        cerr << "Socket creation failed" << endl;
        WSACleanup();
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Binding the socket
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        cerr << "Bind failed" << endl;
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    if (listen(server_fd, 3) == SOCKET_ERROR) {
        cerr << "Listen failed" << endl;
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    cout << "Server is listening on port " << PORT << "..." << endl;

    while (true) {
        SOCKET client_socket;
        struct sockaddr clientAddress;
        int len = sizeof(clientAddress);
        if ((client_socket = accept(server_fd, (struct sockaddr*)&clientAddress , (int*)&len)) == INVALID_SOCKET) {
            cerr << "Accept failed" << endl;
            closesocket(server_fd);
            WSACleanup();
            return 1;
        }

        cout << "Connection established with client." << endl;
        // Spawn a new thread to handle the client
        thread clientThread(handleClient, client_socket);
        clientThread.detach(); // Detach the thread to allow independent execution
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}
