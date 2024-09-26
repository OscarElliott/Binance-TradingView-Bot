
#include "../bot.h"
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include<sstream>
#include <fstream>
#include <cpprest/http_client.h>
#include <cpprest/uri.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <chrono>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>

using namespace std;

//Test Credentials
string APIkey;
string APIsecret;

// TEST print scaffold
/*
    if (testSuccesful == true) cout << "\033[32m [NAME] : TESTS PASSED\033[0m" << endl;
    else cout << "\033[31m [NAME] : TESTS FAILED\033[0m" << endl;

*/

bool test1() 
{
    Bot bot("123", "spot", "BTCUSDT", "BUY", "USD", "LIMIT", 1000, 10, "api_key", "api_secret");

    bool testSuccesful = true;

    if (bot.getId() != "123") testSuccesful = false;
    if (bot.getType() != "spot")  testSuccesful = false;
    if (bot.getTradingPair() != "BTCUSDT") testSuccesful = false;
    if (bot.getSide() != "BUY") testSuccesful = false;
    if (bot.getBaseSizeType() != "USD") testSuccesful = false;
    if (bot.getOrderType() != "LIMIT") testSuccesful = false;
    if (bot.getBaseOrderSize() != 1000) testSuccesful = false;
    if (bot.getLeverage() != 10) testSuccesful = false;
    if (bot.getApiKey() != "api_key") testSuccesful = false;
    if (bot.getApiSecret() != "api_secret") testSuccesful = false;

    if (testSuccesful == true) cout << "\033[32mConstructor : TESTS PASSED\033[0m" << endl;
    else cout << "\033[31mConstructor : TESTS FAILED\033[0m" << endl;
    return testSuccesful;
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
    Bot bot("3", "spot", "BTCUSDT", "BUY", "USD", "MARKET", 100, 1, APIkey, APIsecret);
    nlohmann::json webhookData = {{"price", 60000.0}};

    if (bot.handleWebhook(webhookData))
        cout << "\033[32mspot USD MARKET BUY : TESTS PASSED \033[0m" << endl;
    else cout << "\033[31mspot USD MARKET BUY : TESTS FAILED \033[0m" << endl;
}

void test4()
{
    Bot bot("4", "spot", "BTCUSDT", "SELL", "USD", "MARKET", 100, 1, APIkey, APIsecret);
    nlohmann::json webhookData = {{"price", 60000.0}};

    if (bot.handleWebhook(webhookData))
        cout << "\033[32mspot USD MARKET SELL : TESTS PASSED \033[0m" << endl;
    else cout << "\033[31mspot USD MARKET SELL : TESTS FAILED \033[0m" << endl;
}

void test5()
{
    Bot bot("5", "spot", "BTCUSDT", "BUY", "Percentage", "MARKET", 100, 1, APIkey, APIsecret);
    nlohmann::json webhookData = {{"price", 60000.0}};

    if (bot.handleWebhook(webhookData))
        cout << "\033[32mspot Percentage MARKET BUY : TESTS PASSED \033[0m" << endl;
    else cout << "\033[31mspot Percentage MARKET BUY : TESTS FAILED \033[0m" << endl;
}

void test6()
{
    Bot bot("6", "spot", "BTCUSDT", "SELL", "Percentage", "MARKET", 100, 1, APIkey, APIsecret);
    nlohmann::json webhookData = {{"price", 60000.0}};

    if (bot.handleWebhook(webhookData))
        cout << "\033[32mspot Percentage MARKET SELL : TESTS PASSED \033[0m" << endl;
    else cout << "\033[31mspot Percentage MARKET SELL : TESTS FAILED \033[0m" << endl;
}

void test7()
{
    Bot bot("7", "spot", "BTCUSDT", "BUY", "USD", "LIMIT", 10, 1, APIkey, APIsecret);
    nlohmann::json webhookData = {{"price", 63492}};

    if (bot.handleWebhook(webhookData))
        cout << "\033[32mspot USD LIMIT BUY : TESTS PASSED \033[0m" << endl;
    else cout << "\033[31mspot USD LIMIT BUY : TESTS FAILED \033[0m" << endl;
}

void test8()
{
    Bot bot("8", "spot", "BTCUSDT", "SELL", "USD", "LIMIT", 10, 1, APIkey, APIsecret);
    nlohmann::json webhookData = {{"price", 63492}};

    if (bot.handleWebhook(webhookData))
        cout << "\033[32mspot USD LIMIT SELL : TESTS PASSED \033[0m" << endl;
    else cout << "\033[31mspot USD LIMIT SELL : TESTS FAILED \033[0m" << endl;
}

string sign(const string & query, const string & secret) 
{
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digest_len = 0;

    HMAC(EVP_sha256(), secret.c_str(), secret.length(), 
        reinterpret_cast<const unsigned char*>(query.c_str()), query.length(), digest, &digest_len);

    char mdString[SHA256_DIGEST_LENGTH * 2 + 1];
    for (unsigned int i = 0; i < digest_len; i++)
        snprintf(&mdString[i * 2], sizeof(mdString) - i * 2, "%02x", (unsigned int)digest[i]);

    return string(mdString);
}

void fetchUSDT()
{
    long timestamp = chrono::duration_cast<chrono::milliseconds>(
        chrono::system_clock::now().time_since_epoch()).count();
    
    string query_string = "timestamp=" + to_string(timestamp);
    
    string signature = sign(query_string, APIsecret);

    web::http::client::http_client client(U("https://testnet.binance.vision"));
    web::http::uri_builder builder(U("/api/v3/account"));
    builder.append_query(U("timestamp"), timestamp);
    builder.append_query(U("signature"), signature);

    web::http::http_request request(web::http::methods::GET);
    request.headers().add(U("X-MBX-APIKEY"), APIkey);
    request.set_request_uri(builder.to_uri());

    try {
        auto response = client.request(request).get();
        auto body = response.extract_json().get();
        cout << body.serialize() << endl;
    } catch (const exception &e) {
        cerr << "Error: " << e.what() << endl;
    }
}

void setAPIDetails()
{
    cout << "Running: setAPIDetails" << endl;
    ifstream file("Tests/pw_test.json");
    if (!file.is_open()) 
    {
        cout << "Could not open config.json file." << endl;
        return;
    }
    nlohmann::json config;
    try 
    {
        file >> config;
        file.close(); // Close the file after reading
    } 
    catch (const nlohmann::json::parse_error& e) 
    {
        cout << "Error parsing config.json: " << endl;
        return;
    }
    APIkey = config["binance_api_key"];
    APIsecret = config["binance_api_secret"];
    cout << "KEY: " << APIkey << " Secret: " << APIsecret << endl;
}

int main()
{
    setAPIDetails();
    //fetchUSDT();
    if (!test1())
        test2(); // will print verbose
    test3();
    test4();
    test5();
    test6();
    test7();
    test8();
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
