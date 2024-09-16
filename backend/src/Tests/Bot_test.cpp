
#include "../bot.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>

using namespace std;

void test1() 
{
    Bot bot("123", "spot", "BTCUSDT", "BUY", "USD", "LIMIT", 1000, 10, "api_key", "api_secret");

    cout << (bot.getId() == "123") << endl;
    cout << (bot.getType() == "spot") << endl;
    cout << (bot.getTradingPair() == "BTCUSDT") << endl;
    cout << (bot.getSide() == "BUY") << endl;
    cout << (bot.getBaseSizeType() == "USD") << endl;
    cout << (bot.getOrderType() == "LIMIT") << endl;
    cout << (bot.getBaseOrderSize() == 1000) << endl;
    cout << (bot.getLeverage() == 10)<< endl;
    cout << (bot.getApiKey() == "api_key") << endl;
    cout << (bot.getApiSecret() == "api_secret")<< endl;
}

void test2() 
{
    Bot bot("123", "spot", "BTCUSDT", "BUY", "USD", "LIMIT", 1000, 10, "api_key", "api_secret");

    cout << bot.getId() << "==" << "123" << endl;
    cout << bot.getType() << "==" << "spot" << endl;
    cout << bot.getTradingPair() << "==" << "BTCUSDT" << endl;
    cout << bot.getSide() << "==" << "BUY" << endl;
    cout << bot.getBaseSizeType() << "==" << "USD" << endl;
    cout << bot.getOrderType() << "==" << "LIMIT" << endl;
    cout << bot.getBaseOrderSize() << "==" << "1000" << endl;
    cout << bot.getLeverage() << "==" << "10" << endl;
    cout << bot.getApiKey() << "==" << "api_key" << endl;
    cout << bot.getApiSecret() << "==" << "api_secret" << endl;
}

void test3()
{
    Bot bot("123", "spot", "BTC", "BUY", "USD", "LIMIT", 10, 1, "Cfu3hh8WUFtp28PvNNtAHalpZarxWPiYSw6bJOke2KiCFFoEQnpAIhWT5bbdVKlj", "Y7iMPFlgX4BuIzy8ZB7P5rYlEpFcHZzNMjN0PVKCoEz6BCIc7ed4PHpEWzPesOd0");
    nlohmann::json webhookData = {{"price", 100000.0}};

    bot.handleWebhook(webhookData);
}

int main()
{
    test1();
    test2();
    test3();
    return 0;
}

/*
TEST_CASE("Bot handleWebhook", "[Bot]") {
    
    Bot bot("123", "spot", "BTCUSDT", "BUY", "USD", "LIMIT", 10, 1, "Cfu3hh8WUFtp28PvNNtAHalpZarxWPiYSw6bJOke2KiCFFoEQnpAIhWT5bbdVKlj", "Y7iMPFlgX4BuIzy8ZB7P5rYlEpFcHZzNMjN0PVKCoEz6BCIc7ed4PHpEWzPesOd0");
    nlohmann::json webhookData = {{"price", 100000.0}};

    bot.handleWebhook(webhookData);
}

TEST_CASE("Bot placeSpotOrder", "[Bot]") {
    // Create a bot instance
    Bot bot("123", "spot", "BTCUSDT", "BUY", "USD", "LIMIT", 20, 1, "Cfu3hh8WUFtp28PvNNtAHalpZarxWPiYSw6bJOke2KiCFFoEQnpAIhWT5bbdVKlj", "Y7iMPFlgX4BuIzy8ZB7P5rYlEpFcHZzNMjN0PVKCoEz6BCIc7ed4PHpEWzPesOd0");

    // Set up expected values
    double price = 85000.0;
    bot.placeSpotOrder("BUY", "LIMIT", price);
}
*/
