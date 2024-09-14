#include <pistache/endpoint.h>
#include <pistache/router.h>
#include <pistache/http.h>
#include <pistache/net.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "bot.h"

using namespace Pistache;
using namespace std;
using json = nlohmann::json;

class BackendServer 
{
public:
    explicit BackendServer(Address addr)
        : httpEndpoint(make_shared<Http::Endpoint>(addr)) {}

    void init(size_t threads = 2) 
    {
        auto opts = Http::Endpoint::options().threads(static_cast<int>(threads));
        httpEndpoint->init(opts);
        setupRoutes();
        loadBotsFromConfig(); // Load bot configurations during initialization
    }

    void start() 
    {
        // Start WebSocket connection in a separate thread
        wsThread = thread(&BackendServer::startWebSocket, this);

        // Start the HTTP server
        httpEndpoint->setHandler(router.handler());
        httpEndpoint->serve();
    }

    void shutdown() 
    {
        httpEndpoint->shutdown();
        if (wsThread.joinable()) {
            wsThread.join();
        }
    }

private:
    unordered_map<string, Bot> bots; // Store bots by their ID
    WebSocketHandler wsHandler;      // WebSocket handler
    thread wsThread; 

    void setupRoutes() 
    {
        using namespace Rest;
        Routes::Post(router, "/webhook", Routes::bind(&BackendServer::handleWebhook, this));
        Routes::Post(router, "/addbot", Routes::bind(&BackendServer::handleAddBot, this));
        Routes::Post(router, "/getbots", Routes::bind(&BackendServer::handleFetchBots, this));
    }

    void loadBotsFromConfig() 
    {
        ifstream configFile("../config.json");
        if (configFile.is_open()) 
        {
            json configData;
            configFile >> configData;
            string apiKey = configData["binance_api_key"];
            string apiSecret = configData["binance_api_secret"];

            // Load each bot from the config
            for (const auto& botConfig : configData["bots"]) 
            {
                string id = botConfig["id"];
                string type = botConfig["type"];
                string tradingPair = botConfig["trading_pair"];
                int leverage = botConfig.value("leverage", 1);  // Default to 1 if not provided

                Bot bot(id, type, tradingPair, leverage, apiKey, apiSecret);
                bots[id] = bot;
            }

            cout << "Bots loaded successfully." << endl;
        } 
        else 
        {
            cerr << "Could not read config file" << endl;
        }
    }

    void handleWebhook(const Rest::Request &request, Http::ResponseWriter response) 
    {
        auto webhookData = json::parse(request.body());
        string botId = webhookData["bot_id"];
        // if bot exists get it to handle the incoming request
        if (bots.find(botId) != bots.end()) 
        {
            bots[botId].handleWebhook(webhookData, bots[botId].apiSecret);
            response.send(Http::Code::Ok, "Webhook handled");
        } 
        else 
        {
            response.send(Http::Code::Not_Found, "Bot not found");
        }
    }

    void handleAddBot(const Rest::Request& request, Http::ResponseWriter response) 
    {
        try 
        {
            // Parse the JSON body
            auto body = request.body();
            json botData = json::parse(body);

            // Extract the bot details
            string id = botData["id"];
            string botType = botData["type"];
            string tradingPair = botData["tradingPair"];
            int leverage = botData["leverage"];
            string apiKey = botData["apiKey"];
            string apiSecret = botData["apiSecret"];

            // Print or process the bot data for testing                            <-----------TESTING OUTPUT------------>
            cout << "Received new bot configuration:" << endl;
            cout << "id: " << id << endl;
            cout << "Type: " << botType << endl;
            cout << "Trading Pair: " << tradingPair << endl;
            cout << "Leverage: " << leverage << endl;
            cout << "API Key: " << apiKey << endl;
            cout << "API Secret: " << apiSecret << endl;   //                       <-----------END TEST OUTPUT------------>                         

            Bot new_bot = new Bot(id, botType, tradingPair, leverage, apiKey, apiSecret);
            bots[id] = new_bot;

            // Send a successful response back to the client
            json responseData;
            responseData["status"] = "success";
            responseData["message"] = "Bot configuration saved successfully.";

            response.send(Http::Code::Ok, responseData.dump());
        } 
        catch (const exception& ex) 
        {
            json errorData;
            errorData["status"] = "error";
            errorData["message"] = ex.what();

            response.send(Http::Code::Bad_Request, errorData.dump());
        }
    }

    void handleFetchBots(const Rest::Request& request, Http::ResponseWriter response)
    {
        try 
        {
            json botsList = json::array();

            // Iterate over the map of bots and add their data to the JSON response
            for (const auto& [id, bot] : bots) 
            {
                json botData;
                botData["id"] = bot.getId();  // Assuming you have a getId() method in Bot class
                botData["type"] = bot.getType();  // Assuming you have a getType() method
                botData["trading_pair"] = bot.getTradingPair();  // Assuming you have a getTradingPair() method
                botData["leverage"] = bot.getLeverage();  // Assuming you have a getLeverage() method

                botsList.push_back(botData);
            }

            json responseData;
            responseData["status"] = "success";
            responseData["bots"] = botsList;

            // Send response back to client
            response.send(Http::Code::Ok, responseData.dump());
        } 
        catch (const exception& ex) 
        {
            json errorData;
            errorData["status"] = "error";
            errorData["message"] = ex.what();

            response.send(Http::Code::Internal_Server_Error, errorData.dump());
        }
    }


    void startWebSocket() 
    {
        // Connect to the WebSocket server 
        wsHandler.connect("ws://localhost:9002");
    }

    shared_ptr<Http::Endpoint> httpEndpoint;
    Rest::Router router;
};

int main() 
{
    Port port(9080);
    Address addr(Ipv4::any(), port);

    BackendServer server(addr);

    server.init();
    cout << "Server is running on port " << port << endl;
    server.start();

    return 0;
}
