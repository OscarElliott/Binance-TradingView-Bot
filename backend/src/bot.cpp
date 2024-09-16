#include "bot.h"
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <chrono>
#include <cpprest/http_client.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <nlohmann/json.hpp>
#include <iostream>

using namespace web;
using namespace web::http;
using namespace web::http::client;
using namespace std;
using nlohmann::json;

Bot::Bot(const std::string &id, const std::string &type, const std::string &tradingPair, const std::string &side, const std::string &baseSizeType, const std::string &orderType, int baseOrderSize, int leverage, const std::string &UserApiKey, const std::string &UserSecret)
    : id(id), type(type), tradingPair(tradingPair), side(side), baseSizeType(baseSizeType), orderType(orderType), baseOrderSize(baseOrderSize), leverage(leverage), apiKey(UserApiKey), apiSecret(UserSecret) {}


// THIS IS ONLY PARTIALLY IMPLEMENTED AT THIS POINT
void Bot::handleWebhook(const nlohmann::json& webhookData) {
    double price  = 0;
    if (webhookData["price"] > 0) price = webhookData["price"];
    if (type == "spot")
    {
        placeSpotOrder(this->getSide(), this->getOrderType(), price);
    }
    else if (type == "futures")
    {
        placeFuturesOrder(this->getSide(), this->getOrderType(), price);
    }
    
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

string Bot::getBaseSizeType() {
    return baseSizeType;
}

string Bot::getOrderType() {
    return orderType;
}

string Bot::getSide() {
    return side;
}

int Bot::getLeverage() {
    return leverage;
}

int Bot::getBaseOrderSize() {
    return baseOrderSize;
}

void Bot::placeSpotOrder(const string &side, const string &orderType, double price) {
    // Generate UUID for the request ID
    boost::uuids::random_generator generator;
    boost::uuids::uuid uuid = generator();

    // Get the current timestamp in milliseconds
    auto timestamp = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();

    float quantity;

    if (this->getBaseSizeType() == "USD")
        quantity = this->getBaseOrderSize()/price;
    else if (this->getBaseSizeType() == "Percentage")
        quantity = this->getBaseOrderSize()/price;


    // Create the JSON request body
    nlohmann::json params;
    params["symbol"] = tradingPair;
    params["side"] = side;
    params["type"] = orderType;

    if (orderType == "LIMIT") {
        params["price"] = to_string(price);
        params["timeInForce"] = "GTC"; // Good 'Til Canceled for limit orders
    }

    params["quantity"] = to_string(quantity);
    params["timestamp"] = timestamp;

    // Prepare the string to sign (for HMAC-SHA256)
    string query_string = "symbol=" + tradingPair +
                          "&side=" + side +
                          "&type=" + orderType +
                          "&quantity=" + to_string(quantity) +
                          "&timestamp=" + to_string(timestamp);

    if (orderType == "LIMIT") {
        query_string += "&price=" + to_string(price) + "&timeInForce=GTC";
    }

    // Sign the request
    string signature = signRequest(query_string, this->getApiSecret());
    params["signature"] = signature;

    // Construct the full JSON payload
    nlohmann::json payload;
    payload["id"] = boost::uuids::to_string(uuid);
    payload["method"] = "order.place";
    payload["params"] = params;

    // Set the API Key in the params (for authentication)
    params["apiKey"] = this->getApiKey();

    // Create the client and HTTP request
    http_client client(U("https://api.binance.com"));
    http_request request(methods::POST);
    request.set_request_uri(U("/api/v3/order"));

    // Set headers
    request.headers().add(U("X-MBX-APIKEY"), this->getApiKey());
    request.headers().add(U("Content-Type"), U("application/json"));

    // Set the JSON body as the request content
    request.set_body(payload.dump(), "application/json");

    // Send the request and get the response
    auto response = client.request(request).get();

    // Handle response
    if (response.status_code() == status_codes::OK) {
        cout << response.extract_json().get().serialize() << endl;
    } else {
        cout << "response hit error" << endl;
        cerr << "Error: " << response.status_code() << endl;
    }
}

void Bot::placeFuturesOrder(const string &side, const string &orderType, double price) {
    // Generate UUID for the request ID
    boost::uuids::random_generator generator;
    boost::uuids::uuid uuid = generator();

    // Get the current timestamp in milliseconds
    auto timestamp = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();


    float quantity;

    if (this->getBaseSizeType() == "USD")
        quantity = this->getBaseOrderSize()/price;
    else if (this->getBaseSizeType() == "Percentage")
        quantity = this->getBaseOrderSize()/price;

    // Create the JSON request body
    nlohmann::json params;
    params["symbol"] = tradingPair;
    params["side"] = side;
    params["type"] = orderType;

    if (orderType == "LIMIT") {
        params["price"] = to_string(price);
        params["timeInForce"] = "GTC"; // Good 'Til Canceled for limit orders
    }

    params["quantity"] = to_string(quantity);
    params["timestamp"] = timestamp;

    // Prepare the string to sign (for HMAC-SHA256)
    string query_string = "symbol=" + tradingPair +
                          "&side=" + side +
                          "&type=" + orderType +
                          "&quantity=" + to_string(quantity) +
                          "&timestamp=" + to_string(timestamp);

    if (orderType == "LIMIT") {
        query_string += "&price=" + to_string(price) + "&timeInForce=GTC";
    }

    // Sign the request
    string signature = signRequest(query_string, this->getApiSecret());
    params["signature"] = signature;

    // Construct the full JSON payload
    nlohmann::json payload;
    payload["id"] = boost::uuids::to_string(uuid);
    payload["method"] = "order.place";
    payload["params"] = params;

    // Set the API Key in the params (for authentication)
    params["apiKey"] = this->getApiKey();

    // Create the client and HTTP request
    http_client client(U("https://fapi.binance.com/fapi/v1/order"));
    http_request request(methods::POST);
    request.set_request_uri(U("/api/v3/order"));

    // Set headers
    request.headers().add(U("X-MBX-APIKEY"), this->getApiKey());
    request.headers().add(U("Content-Type"), U("application/json"));

    // Set the JSON body as the request content
    request.set_body(payload.dump(), "application/json");

    // Send the request and get the response
    auto response = client.request(request).get();

    // Handle response
    if (response.status_code() == status_codes::OK) {
        cout << response.extract_json().get().serialize() << endl;
    } else {
        cerr << "Error: " << response.status_code() << endl;
    }
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


void Bot::updateLeverage(const string& symbol, int leverage, const string& apiKey, const string& secretKey) {
    try {
        // Prepare the parameters
        string endpoint = "https://fapi.binance.com/fapi/v1/leverage";
        auto timestamp = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();

        // Create the query string
        string query = "symbol=" + symbol + "&leverage=" + to_string(leverage) + "&timestamp=" + to_string(timestamp);

        // Generate the signature using the secret key
        string signature = signRequest(query, secretKey);

        // Append the signature to the query
        query += "&signature=" + signature;

        // Create the full URL with the query string
        uri_builder uri(endpoint);
        uri.append_query(query);

        // Set up the HTTP client and the request
        http_client client(U(endpoint));
        http_request request(methods::POST);
        request.headers().add(U("X-MBX-APIKEY"), U(apiKey));
        request.headers().set_content_type(U("application/x-www-form-urlencoded"));

        // Send the request and get the response
        client.request(request).then([](http_response response) {
            if (response.status_code() == status_codes::OK) {
                return response.extract_json();
            }
            throw runtime_error("Failed to update leverage");
        }).then([](web::json::value jsonResponse) {
            cout << "Leverage updated successfully: " << jsonResponse.serialize() << endl;
        }).wait();
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
    }
}
