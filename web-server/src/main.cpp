#include "../include/handlers.hpp"
#include <iostream>
#include <string>
#include <sstream>

using namespace std;

Server* Server::instance = nullptr;

int main() {
    string info, address;
    int port;
    getline(cin, info);
    istringstream iss(info);
    getline(iss, address, ':');
    iss >> port;

    try {
        Server *server = Server::get_instance(port, address);
        server->set_notfound("page/404.html");
        server->get("/index.html", new ShowPage("page/index.html"));
        server->get("/", new DefaultHandler());
        server->get("/billiejean.mp3", new ShowAudio("audio/billiejean.mp3"));
        server->get("/michaeljackson.pdf", new ShowDocument("doc/michaeljackson.pdf"));
        server->get("/smoothcriminal.jpg", new ShowImage("img/smoothcriminal.jpg"));
        server->get("/moonwalk.gif", new ShowImage("img/moonwalk.gif"));
        server->run();
    } catch (const Server::Exception& e) {
        cerr << e.get_message() << endl;
    }
    return 0;
}
