#include <websocketpp/server.hpp>
#include <websocketpp/config/asio_no_tls.hpp>

using websocketpp::server;
using websocketpp::connection_hdl;
using namespace std;

class WebSocketServer {
public:
    WebSocketServer() {
        // Initialize Asio
        m_server.init_asio();

        // Set the message handler
        m_server.set_message_handler(bind(&WebSocketServer::on_message, this, _1, _2));
    }

    void start() {
        m_server.listen(9002); // Listen on port 9002
        m_server.start_accept();
        m_server.run();
    }

private:
    void on_message(connection_hdl hdl, server<websocketpp::config::asio> ::message_ptr msg) {
        // Handle incoming messages
        cout << "Received message: " << msg->get_payload() << endl;
    }

    server<websocketpp::config::asio> m_server;
};

void startWebSocket() {
    WebSocketServer wsServer;
    wsServer.start();
}
