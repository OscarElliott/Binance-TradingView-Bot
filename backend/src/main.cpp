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
        httpEndpoint->setHandler(router.handler());
        httpEndpoint->serve();
    }

    void shutdown() 
    {
        httpEndpoint->shutdown();
    }

private:
    unordered_map<string, Bot> bots; // Store bots by their ID

    void setupRoutes() 
    {
        using namespace Rest;
        Routes::Post(router, "/webhook", Routes::bind(&BackendServer::handleWebhook, this));
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
