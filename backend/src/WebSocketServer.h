#ifndef WEBSOCKETSERVER_H
#define WEBSOCKETSERVER_H

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <iostream>
#include <string>
#include <functional>

// Define a type for the client
typedef websocketpp::client<websocketpp::config::asio_client> ws_client;

class WebSocketServer 
{
public:
    // Constructor to initialize the WebSocket client
    WebSocketServer();

    // Method to connect to a WebSocket server
    void connect(const std::string& uri);

private:
    // WebSocket client instance
    ws_client m_client;

    // Handlers for WebSocket events
    void on_open(websocketpp::connection_hdl hdl);
    void on_fail(websocketpp::connection_hdl hdl);
    void on_close(websocketpp::connection_hdl hdl);
    void on_message(websocketpp::connection_hdl hdl, ws_client::message_ptr msg);
};

#endif // WEBSOCKETSERVER_H
