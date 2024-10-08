#include <pistache/endpoint.h>
#include <pistache/router.h>
#include <pistache/http.h>
#include <pistache/net.h>
#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>
#include <unordered_map>

#include "bot.h"
#include "WebSocketServer.h"

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
        //wsThread = thread(&BackendServer::startWebSocket, this);

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
    unordered_map<string, Bot*> bots; // Store bots via ptr by their ID
    WebSocketServer wsHandler;      // WebSocket handler
    thread wsThread;
    string UserApiKey;
    string UserSecret;
    string Email;
    int PORT_NUMBER;


    void setupRoutes() 
    {
        using namespace Rest;
        Routes::Post(router, "/webhook", Routes::bind(&BackendServer::handleWebhook, this));
        Routes::Post(router, "/addbot", Routes::bind(&BackendServer::handleAddBot, this));
        Routes::Options(router, "/addbot", Routes::bind(&BackendServer::handleOptionsRequest, this));
        Routes::Get(router, "/getbots", Routes::bind(&BackendServer::handleFetchBots, this));
        Routes::Options(router, "/getbots", Routes::bind(&BackendServer::handleOptionsRequest, this));
        Routes::Get(router, "/getSettings", Routes::bind(&BackendServer::fetchUserConfig, this));
        Routes::Options(router, "/getSettings", Routes::bind(&BackendServer::handleOptionsRequest, this));
        Routes::Post(router, "/postSettings", Routes::bind(&BackendServer::updateUserConfig, this));
        Routes::Options(router, "/postSettings", Routes::bind(&BackendServer::handleOptionsRequest, this));
    }

    void loadBotsFromConfig() 
    {
        ifstream configFile("../config.json");
        if (configFile.is_open()) 
        {
            json configData;
            configFile >> configData;
            UserApiKey = configData.at("binance_api_key");
            UserSecret = configData.at("binance_api_secret");
            Email = configData.at("Email");
            PORT_NUMBER = configData.at("webhook_port");


            // Load each bot from the config
            for (const auto& botConfig : configData.at("bots")) 
            {
                string id = botConfig.at("id");
                string type = botConfig.at("type");
                string tradingPair = botConfig.at("tradingPair");
                string side = botConfig.at("side");
                string baseSizeType = botConfig.at("baseSizeType");
                string orderType = botConfig.at("orderType");
                int baseOrderSize = botConfig.value("baseOrderSize", 1);  
                int leverage = botConfig.value("leverage", 1);  // Default to 1 if not provided

                Bot* load_bot = new Bot(id, type, tradingPair, side, baseSizeType, orderType, baseOrderSize, leverage, UserApiKey, UserSecret);
                bots[id] = load_bot;
            }

            cout << "Bots loaded successfully." << endl;
        } 
        else 
        {
            cerr << "Could not read config file" << endl;
        }
    }

    void addCORSHeaders(Http::ResponseWriter& writer) 
    {
        writer.headers().add<Http::Header::AccessControlAllowOrigin>("*"); // Allow all origins
        writer.headers().add<Http::Header::AccessControlAllowMethods>("GET, POST, OPTIONS"); // Allow specific methods
        writer.headers().add<Http::Header::AccessControlAllowHeaders>("Content-Type, Authorization"); // Allow specific headers
    }

    void handleOptionsRequest(const Rest::Request&, Http::ResponseWriter writer) 
    {
        addCORSHeaders(writer);
        cout << "OPTIONS CONFIGURED" << endl;
        writer.send(Http::Code::Ok, ""); // Respond to OPTIONS request
    }

    void handleWebhook(const Rest::Request &request, Http::ResponseWriter response) 
    {
        auto webhookData = json::parse(request.body());
        string botId = webhookData["bot_id"];
        // if bot exists get it to handle the incoming request
        if (bots.find(botId) != bots.end()) 
        {
            bots[botId]->handleWebhook(webhookData);
            response.send(Http::Code::Ok, "Webhook handled");
        } 
        else 
        {
            response.send(Http::Code::Not_Found, "Bot not found");
        }
    }

    void handleAddBot(const Rest::Request& request, Http::ResponseWriter response) 
    {
        addCORSHeaders(response);

        cout << "Received request body: " << request.body() << endl;

        if (request.body().empty()) 
        {
            cout << "ERROR: Empty body for Bot" << endl;
            response.send(Http::Code::Bad_Request, "Request body is empty");
            return;
        }

        try 
        {
            // Parse the JSON body
            auto body = request.body();
            json botData = json::parse(body);

            // Extract the bot details
            string id = botData["id"];
            string type = botData["type"];
            string tradingPair = botData["tradingPair"];
            string side = botData["side"];
            string baseSizeType = botData["baseSizeType"];
            string orderType = botData["orderType"];
            int leverage = botData["leverage"];
            int baseOrderSize = botData["baseOrderSize"];
            string apiKey = UserApiKey;
            string apiSecret = UserSecret;                       

            Bot* new_bot = new Bot(id, type, tradingPair, side, baseSizeType, orderType, baseOrderSize, leverage, UserApiKey, UserSecret);
            bots[id] = new_bot;

            // add bot to static config file
            // Open the JSON file to read current configuration
            ifstream file("../config.json");
            if (!file.is_open()) {
                json errorResponse;
                errorResponse["status"] = "error";
                errorResponse["message"] = "Could not open config.json file.";
                response.send(Http::Code::Internal_Server_Error, errorResponse.dump());
                return;
            }

            json config;
            try {
                file >> config;
                file.close(); // Close the file after reading
            } catch (const json::parse_error& e) {
                json errorResponse;
                errorResponse["status"] = "error";
                errorResponse["message"] = "Error parsing config.json: " + string(e.what());
                response.send(Http::Code::Internal_Server_Error, errorResponse.dump());
                return;
            }

            if (!config.contains("bots") || !config["bots"].is_array()) 
            {
                config["bots"] = json::array();
            }
            json& bots = config["bots"];
            
            string newBotId = id;
            bool botFound = false;

            // Update existing bot or add new bot
            for (auto& bot : bots) 
            {
                if (bot["id"] == newBotId) 
                {
                    bot = botData;
                    botFound = true;
                    break;
                }
            }

            // If bot was not found in the existing list, add it
            if (!botFound) {
                bots.push_back(botData);
            }

            cout << config << endl;

            // Write the updated configuration back to the file
            ofstream outFile("../config.json");
            if (!outFile.is_open()) 
            {
                cout << "file open errer" << endl;
                json errorResponse;
                errorResponse["status"] = "error";
                errorResponse["message"] = "Could not open config.json file for writing.";
                response.send(Http::Code::Internal_Server_Error, errorResponse.dump());
            }
            try {
                // Write the JSON data to the file
                outFile << config.dump(4); // Pretty print with 4 spaces
                outFile.close(); // Close the file after writing
            } catch (const std::exception& e) {
                cout << "file writing errer" << endl;
                cerr << "Error writing to config.json: " << e.what() << endl;
            }
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
            cout << errorData << endl;
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
                botData["id"] = bot->getId();  
                botData["type"] = bot->getType();  
                botData["tradingPair"] = bot->getTradingPair(); 
                botData["side"] = bot->getSide(); 
                botData["baseSizeType"] = bot->getBaseSizeType();
                botData["orderType"] = bot->getOrderType();
                botData["baseOrderSize"] = bot->getBaseOrderSize();  
                botData["leverage"] = bot->getLeverage();  

                botsList.push_back(botData);
            }

            json responseData;
            responseData["status"] = "success";
            responseData["bots"] = botsList;

            // Send response back to client
            addCORSHeaders(response);
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

    void fetchUserConfig(const Rest::Request& request, Http::ResponseWriter response) 
    {
        try
        {
            json responseData;
            responseData["Email"] = Email;
            responseData["apiKey"] = UserApiKey;
            responseData["status"] = "success";
            responseData["message"] = "Bot configuration saved successfully.";

            addCORSHeaders(response);
            response.send(Http::Code::Ok, responseData.dump());
        }
        catch(const exception& e)
        {
            cerr << "Couldn't fetch user settings" << '\n';
        }
    }

    void updateUserConfig(const Rest::Request& request, Http::ResponseWriter response) 
    {
        // Read the request body to get the new configuration details
        json newConfig;
        try {
            newConfig = json::parse(request.body());
        } catch (const json::parse_error& e) {
            json errorResponse;
            errorResponse["status"] = "error";
            errorResponse["message"] = "Invalid JSON format: " + string(e.what());
            response.send(Http::Code::Bad_Request, errorResponse.dump());
            return;
        }

        // Open the JSON file to read current configuration
        ifstream file("../config.json");
        if (!file.is_open()) {
            json errorResponse;
            errorResponse["status"] = "error";
            errorResponse["message"] = "Could not open config.json file.";
            response.send(Http::Code::Internal_Server_Error, errorResponse.dump());
            return;
        }

        json config;
        try {
            file >> config;
            file.close(); // Close the file after reading
        } catch (const json::parse_error& e) {
            json errorResponse;
            errorResponse["status"] = "error";
            errorResponse["message"] = "Error parsing config.json: " + string(e.what());
            response.send(Http::Code::Internal_Server_Error, errorResponse.dump());
            return;
        }

        // Update the configuration with new values
        if (newConfig.contains("Email")) {
            config["Email"] = newConfig["Email"];
            Email = newConfig["Email"];
        }
        if (newConfig.contains("binance_api_key")) {
            config["binance_api_key"] = newConfig["binance_api_key"];
            UserApiKey = newConfig["binance_api_key"];
        }
        if (newConfig.contains("binance_api_secret") && newConfig["binance_api_secret"] != "***************") {
            config["binance_api_secret"] = newConfig["binance_api_secret"];
            UserSecret = newConfig["binance_api_secret"];
        }

        // Write the updated configuration back to the file
        ofstream outFile("../config.json");
        if (!outFile.is_open()) {
            json errorResponse;
            errorResponse["status"] = "error";
            errorResponse["message"] = "Could not open config.json file for writing.";
            response.send(Http::Code::Internal_Server_Error, errorResponse.dump());
            return;
        }

        try {
            outFile << config.dump(4); // Pretty-print with 4-space indentation
            outFile.close(); // Close the file after writing
        } catch (const exception& e) {
            json errorResponse;
            errorResponse["status"] = "error";
            errorResponse["message"] = "Error writing to config.json: " + string(e.what());
            response.send(Http::Code::Internal_Server_Error, errorResponse.dump());
            return;
        }

        // Send success response
        json successResponse;
        successResponse["status"] = "success";
        successResponse["message"] = "Configuration updated successfully.";
        addCORSHeaders(response);
        response.send(Http::Code::Ok, successResponse.dump());
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
