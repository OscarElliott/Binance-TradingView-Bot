#include <iostream>
#include <string>
#include <fstream>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <chrono>
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <nlohmann/json.hpp>

using namespace web;
using namespace std;
using namespace web::http;
using namespace web::http::client;
using json = nlohmann::json;

class Bot 
{
public:
    string id;
    string type; // "spot" or "futures"
    string tradingPair; // ticker symbol pair
    int leverage;
    string apiKey;
    string apiSecret;

    Bot(const string & id, const string & type, const string & tradingPair, int leverage, const string & apiKey, string & apiSecret)
        : id(id), type(type), tradingPair(tradingPair), leverage(leverage), apiKey(apiKey), apiSecret(apiSecret) {}
    
    void handleWebhook(const json& webhookData, string & apiSecret)
    {
        string action = webhookData["action"];
        double quantity = webhookData["quantity"];
        string orderType = webhookData["orderType"];
        double price = webhookData["price"];
        placeOrder(action, quantity, orderType, price, apiSecret);
    }

private:
    void placeOrder(const string & action, double quantity, string & orderType, double price, string apiSecret)
    {
        uri_builder builder(U("https://api.binance.com/api/v3/order"));
        builder.append_query(U("symbol"), U(tradingPair));
        builder.append_query(U("side"), U(action));
        builder.append_query(U("type"), U(orderType));
        builder.append_query(U("quantity"), U(to_string(quantity)));
        builder.append_query(U("price"), U(to_string(price)));
        // Get the current timestamp in milliseconds
        auto timestamp = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
        builder.append_query(U("timestamp"), U(to_string(timestamp)));
    

        string query = builder.query();
        string signature = signRequest(query, apiSecret);
        builder.append_query(U("signature"), U(signature));

        http_client client(U("https://api.binance.com"));
        http_request request(methods::POST);
        request.set_request_uri(builder.to_uri());

        request.headers().add(U("X-MBX-APIKEY"), apiKey);
        request.headers().add(U("Content-Type"), U("application/x-www-form-urlencoded"));

        auto response = client.request(request).get();
        if (response.status_code() == status_codes::OK) 
            wcout << response.extract_json().get().serialize() << endl;
        else wcerr << "Error: " << response.status_code() << endl;
    }

    string signRequest(const string & query, const string & secret) 
    {
        unsigned char digest[EVP_MAX_MD_SIZE];
        unsigned int digest_len = 0;

        HMAC(EVP_sha256(), secret.c_str(), secret.length(), 
            reinterpret_cast<const unsigned char*>(query.c_str()), query.length(), digest, &digest_len);

        char mdString[SHA256_DIGEST_LENGTH * 2 + 1];
        for (unsigned int i = 0; i < digest_len; i++)
            sprintf(&mdString[i * 2], "%02x", (unsigned int)digest[i]);

        return string(mdString);
    }
}