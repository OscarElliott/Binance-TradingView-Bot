#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <iostream>
#include <string>
#include <functional>
#include <nlohmann/json.hpp>
#include "WebSocketServer.h"

using namespace websocketpp;
using namespace std;

// Define a type for the websocket client
typedef websocketpp::client<config::asio_client> websocket_client;

WebSocketServer::WebSocketServer() 
{
    m_client.init_asio();
    m_client.set_message_handler(bind(&WebSocketServer::on_message, this, placeholders::_1, placeholders::_2));
}

void WebSocketServer::connect(const string& uri) 
{
    try 
    {
        m_client.set_open_handler(bind(&WebSocketServer::on_open, this, placeholders::_1));
        m_client.set_fail_handler(bind(&WebSocketServer::on_fail, this, placeholders::_1));
        m_client.set_close_handler(bind(&WebSocketServer::on_close, this, placeholders::_1));

        websocketpp::lib::error_code ec;
        websocket_client::connection_ptr con = m_client.get_connection(uri, ec); // Explicitly refer to websocket_client::connection_ptr
        if (ec) {
            cerr << "Connection error: " << ec.message() << endl;
            return;
        }

        m_client.connect(con);
        m_client.run();
    } 
    catch (const std::exception& e) 
    {
        cerr << "Error: " << e.what() << endl;
    }
}

void WebSocketServer::on_open(connection_hdl hdl) 
{
    cout << "Connected!" << endl;
}

void WebSocketServer::on_fail(connection_hdl hdl) 
{
    cout << "Connection failed!" << endl;
}

void WebSocketServer::on_close(connection_hdl hdl) 
{
    cout << "Connection closed!" << endl;
}

void WebSocketServer::on_message(connection_hdl hdl, websocket_client::message_ptr msg) 
{
    cout << "Received message: " << msg->get_payload() << endl;
    // Placeholder for handling message
}
