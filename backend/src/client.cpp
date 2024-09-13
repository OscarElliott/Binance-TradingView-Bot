#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <iostream>
#include <string>
#include <functional>

using namespace websocketpp;
using namespace std;

// Define a type for the client
typedef client<config::asio_client> client;

class WebSocketHandler 
{
public:
    WebSocketHandler() 
    {
        m_client.init_asio();
        m_client.set_message_handler(bind(&WebSocketHandler::on_message, this, placeholders::_1, placeholders::_2));
    }

    void connect(const string& uri) 
    {
        try 
        {
            m_client.set_open_handler(bind(&WebSocketHandler::on_open, this, placeholders::_1));
            m_client.set_fail_handler(bind(&WebSocketHandler::on_fail, this, placeholders::_1));
            m_client.set_close_handler(bind(&WebSocketHandler::on_close, this, placeholders::_1));

            m_client.connect(m_client.get_adapter()->get_connection(uri, nullptr));
            m_client.run();
        } 
        catch (const exception& e) 
        {
            cerr << "Error: " << e.what() << endl;
        }
    }

private:
    client m_client;

    void on_open(connection_hdl hdl) 
    {
        cout << "Connected!" << endl;
    }

    void on_fail(connection_hdl hdl) 
    {
        cout << "Connection failed!" << endl;
    }

    void on_close(connection_hdl hdl) 
    {
        cout << "Connection closed!" << endl;
    }

    void on_message(connection_hdl hdl, client::message_ptr msg) 
    {
        cout << "Received message: " << msg->get_payload() << endl;
        // parse the message and use the Bot class to place orders
        // For example:
        json webhookData = json::parse(msg->get_payload());
        bot.handleWebhook(webhookData);
    }
};

int main() 
{
    WebSocketHandler wsHandler;
    // Connect to the local WebSocket server
    wsHandler.connect("ws://localhost:9002");  // "ws" instead of "wss" for unencrypted connections
    return 0;
}