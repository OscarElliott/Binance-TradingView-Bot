#ifndef BOT_H
#define BOT_H

#include <string>
#include <nlohmann/json.hpp>

class Bot 
{
public:
    // Constructor
    Bot(const std::string &  id, const std::string & type, const std::string & tradingPair, const std::string &  side, const std::string & baseSizeType, const std::string & orderType, int baseOrderSize, int leverage, const std::string &  UserApiKey, const std::string &  UserSecret);
    // Public methods
    void handleWebhook(const nlohmann::json & webhookData);
    void placeSpotOrder(const std::string & side, const std::string & orderType, double price);
    void placeFuturesOrder(const std::string & side, const std::string & orderType, double price);
    void updateLeverage(const std::string& symbol, int leverage, const std::string& apiKey, const std::string& secretKey);

    // Public attributes
    std::string getId();
    std::string getType();
    std::string getApiKey();
    std::string getApiSecret();
    std::string getTradingPair();
    std::string getSide();
    std::string getBaseSizeType();
    std::string getOrderType();
    int getBaseOrderSize();
    int getLeverage();

private:
    // Private methods
    std::string signRequest(const std::string & query, const std::string & secret);

    // Attributes
    std::string id;
    std::string type; // "spot" or "futures"
    std::string tradingPair;
    std::string side;
    std::string baseSizeType; // "USD" or "Percentage"
    std::string orderType; // "LIMIT" or "MARKET"
    int baseOrderSize; // IN USD or Percentage
    int leverage;
    std::string apiKey;
    std::string apiSecret;
};

#endif
