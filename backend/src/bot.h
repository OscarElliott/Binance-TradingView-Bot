#ifndef BOT_H
#define BOT_H

#include <string>
#include <nlohmann/json.hpp>

class Bot 
{
public:
    // Constructor
    Bot(const std::string & id, const std::string & type, const std::string & tradingPair, int leverage, const std::string & apiKey, const std::string & apiSecret);
    
    // Public methods
    void handleWebhook(const nlohmann::json & webhookData, const std::string & apiSecret);

    // Public attributes
    std::string id;
    std::string type;
    std::string tradingPair;
    int leverage;
    std::string apiKey;
    std::string apiSecret;
    std::string getId();
    std::string getType();
    std::string getTradingPair();
    int getLeverage();

private:
    // Private methods
    void placeOrder(const std::string & action, double quantity, const std::string & orderType, double price, const std::string & apiSecret);
    std::string signRequest(const std::string & query, const std::string & secret);
};

#endif
