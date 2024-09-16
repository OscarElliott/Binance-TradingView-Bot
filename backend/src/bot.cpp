#include "bot.h"
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <chrono>
#include <cpprest/http_client.h>
#include <iostream>

using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace std;

Bot::Bot(const string & id, const string & type, const string & tradingPair, int leverage, const string & apiKey, const string & apiSecret)
    : id(id), type(type), tradingPair(tradingPair), leverage(leverage), apiKey(apiKey), apiSecret(apiSecret) {}

void Bot::handleWebhook(const nlohmann::json& webhookData) {
    string action = webhookData["action"];
    double quantity = webhookData["quantity"];
    string orderType = webhookData["orderType"];
    double price = webhookData["price"];
    string apiSecret = this->getApiSecret();
    placeOrder(action, quantity, orderType, price, apiSecret);
}

string Bot::getId() {
    return id;
}

string Bot::getType() {
    return type;
}

string Bot::getApiKey() {
    return apiKey;
}

string Bot::getApiSecret() {
    return apiSecret;
}

string Bot::getTradingPair() {
    return tradingPair;
}

int Bot::getLeverage() {
    return leverage;
}


void Bot::placeOrder(const string & action, double quantity, const string & orderType, double price, const string & apiSecret)
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
        cout << response.extract_json().get().serialize() << endl;
    else cerr << "Error: " << response.status_code() << endl;
}

string Bot::signRequest(const string & query, const string & secret) 
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

