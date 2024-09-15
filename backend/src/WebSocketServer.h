#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>
#include <iostream>

using websocketpp::server;
using websocketpp::connection_hdl;
using namespace std;

class WebSocketServer {
public:
    // Constructor
    WebSocketServer();

    // Start the WebSocket server
    void start();

private:
    // Handler for incoming messages
    void on_message(connection_hdl hdl, server<websocketpp::config::asio>::message_ptr msg);

    // WebSocket server instance
    server<websocketpp::config::asio> m_server;
};

#endif // WEBSOCKETSERVER_H
