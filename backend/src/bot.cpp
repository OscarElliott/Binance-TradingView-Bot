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
bool Bot::handleWebhook(const nlohmann::json& webhookData) {
    double price  = 0;
    if (webhookData["price"] > 0) price = webhookData["price"];
    if (type == "spot")
    {
        return placeSpotOrder(this->getSide(), this->getOrderType(), price);
    }
    else if (type == "futures")
    {
        return placeFuturesOrder(this->getSide(), this->getOrderType(), price);
    }
    return false;
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

string Bot::shortenToNDecimals(float number, int n) 
{
    std::stringstream stream;
    stream << fixed << setprecision(n) << number;
    return stream.str();
}

bool Bot::placeSpotOrder(const string &side, const string &orderType, double price) {
    // Generate UUID for the request ID
    boost::uuids::random_generator generator;
    boost::uuids::uuid uuid = generator();

    // Get the current timestamp in milliseconds
    long long timestamp = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();

    string quantity;
    double q_value = 0;
    string symbol = this->getTradingPair();
    int baseOrderSize = this->getBaseOrderSize();
    exchangeInfo exInfo = getExchangeInfoFilterForSymbol(symbol);
    int precision = 0;
    while(exInfo.stepSize < 1) 
    {
        exInfo.stepSize*=10;
        precision++;
    }

    if (this->getBaseSizeType() == "USD")
    {
        q_value = (baseOrderSize/price);
        quantity = shortenToNDecimals(q_value, precision);
    }
    else if (this->getBaseSizeType() == "Percentage")
    {
        float balance;
        // if SIDE is BUY get USDT balance
        if (this->getSide() == "BUY")
        {
            string asset = "USDT";
            balance = getBalance(asset);
            q_value = baseOrderSize*balance;
            quantity = shortenToNDecimals(q_value, precision);
        }
        else if (this->getSide() == "SELL")
        {
            q_value = getBalance(symbol);
            quantity = shortenToNDecimals(q_value, precision);
        }
    }
    cout << q_value << endl; 

    if (q_value < exInfo.minQty) 
    {
        std::cout << "Quantity is below minimum allowed: " << exInfo.minQty << " tried to order: " << q_value << std::endl;
        q_value = exInfo.minQty; // Set to minQty if below
    } else if (q_value > exInfo.maxQty) {
        std::cout << "Quantity exceeds maximum allowed: " << exInfo.maxQty << std::endl;
        q_value = exInfo.maxQty; // Set to maxQty if above
    }


    // Create the JSON request body
    nlohmann::json params;
    params["symbol"] = symbol;
    params["side"] = side;
    params["type"] = orderType;


    if (orderType == "LIMIT") 
    {
        params["price"] = to_string(price);
        params["timeInForce"] = "GTC"; // Good 'Til Canceled for limit orders
        params["quantity"] = quantity;
    }
    else
        params["quoteOrderQty"] = baseOrderSize;
    params["timestamp"] = timestamp;

    // Prepare the string to sign (for HMAC-SHA256)
    string query_string = "symbol=" + symbol +
                          "&side=" + side +
                          "&type=" + orderType +
                          "&timestamp=" + to_string(timestamp);

    if (orderType == "LIMIT") 
        query_string += "&price=" + to_string(price) + "&timeInForce=GTC" + "&quantity=" + quantity;
    else query_string += "&quoteOrderQty=" + to_string(baseOrderSize);

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
    http_client client(U("https://testnet.binance.vision"));
    http_request request(methods::POST);
    string url = "/api/v3/order?" + query_string + "&signature=" + signature;
    request.set_request_uri(U(url));

    // Set headers
    string header_key = this->getApiKey();
    request.headers().add(U("X-MBX-APIKEY"), U(header_key));
    request.headers().add(U("Content-Type"), U("application/x-www-form-urlencoded"));

    // Set the JSON body as the request content
    //string requestBody = payload.dump();
    //request.set_body("", "application/x-www-form-urlencoded");

    // Send the request and get the response
    auto response = client.request(request).get();

    // Handle response
    if (response.status_code() == status_codes::OK) {
        cout << response.extract_json().get().serialize() << endl;
        return true;
    } 
    else 
    {
        response.extract_string().then([=](const std::string &body) {
            cout << "Error: " << response.status_code() << " - " << response.extract_string().get() << endl;

            cout << "Request failed with status: " << response.status_code() << endl;
            cout << "Response body: " << body << endl;
            // Print the request that was sent
            cout << "Sent request details:" << endl;
            cout << "URI: " << request.request_uri().to_string() << endl;
            cout << "Headers: " << endl;
            for (const auto &header : request.headers()) {
                cout << header.first << ": " << header.second << endl;
            }
            //cout << "Body: " << requestBody << endl;
        }).wait();
    }
    return false;
}

bool Bot::placeFuturesOrder(const string &side, const string &orderType, double price) {
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
    if (response.status_code() == status_codes::OK) 
    {
        cout << response.extract_json().get().serialize() << endl;
        return true;
    } 
    else 
    {
        cerr << "Error: " << response.status_code() << endl;
    }
    return false;
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

float Bot::getBalance(string & asset)
{
    float balance = 0.0f; // Variable to store the balance

    http_client client(U("https://api.binance.com/"));
    string full_request = "/api/v3/ticker/price?symbol=" + asset;

    // Perform the GET request
    client.request(methods::GET, full_request).then([&balance](http_response response) -> pplx::task<web::json::value>
    {
        // Check if the response is OK
        if (response.status_code() == status_codes::OK)
        {
            return response.extract_json(); // Extract the JSON data
        }
        return pplx::task_from_result(web::json::value()); // Return an empty JSON object if the request failed
    })
    .then([&balance](pplx::task<web::json::value> previousTask) 
    {
        try
        {
            web::json::value const & v = previousTask.get(); // Get the JSON result

            // Assuming the JSON object has a field "price" with the balance value
            if (v.has_field(U("price")))
            {
                balance = std::stof(v.at(U("price")).as_string()); // Convert the price value to float
            }
        }
        catch (http_exception const & e)
        {
            wcout << e.what() << endl;
        }
    }).wait();

    return balance;
}

exchangeInfo Bot::getExchangeInfoFilterForSymbol(const std::string& asset)
{
    exchangeInfo exInfo = {0, 0, 0, 0, 0, 0};
    
    http_client client(U("https://api.binance.com"));
    string request_uri = "/api/v3/exchangeInfo?symbol=" + asset;

    client.request(methods::GET, request_uri).then([](http_response response) -> pplx::task<web::json::value>
    {
        if (response.status_code() == status_codes::OK)
        {
            return response.extract_json();
        }
        return pplx::task_from_result(web::json::value());
    })
    .then([&exInfo, asset](pplx::task<web::json::value> previousTask) // Capture asset by value
    {
        try
        {
            web::json::value const& v = previousTask.get();

            if (v.has_field(U("symbols")))
            {
                auto symbols = v.at(U("symbols")).as_array();
                
                for (auto symbolInfo : symbols)
                {
                    // Check if the symbol matches
                    if (symbolInfo.at(U("symbol")).as_string() == asset) // asset is captured here
                    {
                        if (symbolInfo.has_field(U("filters")))
                        {
                            auto filters = symbolInfo.at(U("filters")).as_array();

                            for (auto filter : filters)
                            {
                                // Look for LOT_SIZE filter
                                if (filter.at(U("filterType")).as_string() == U("LOT_SIZE"))
                                {
                                    exInfo.minQty = std::stod(filter.at(U("minQty")).as_string());
                                    exInfo.maxQty = std::stod(filter.at(U("maxQty")).as_string());
                                    exInfo.stepSize = std::stod(filter.at(U("stepSize")).as_string());
                                }
                                // Look for PRICE_FILTER filter
                                if (filter.at(U("filterType")).as_string() == U("PRICE_FILTER"))
                                {
                                    exInfo.minPrice = std::stod(filter.at(U("minPrice")).as_string());
                                    exInfo.maxPrice = std::stod(filter.at(U("maxPrice")).as_string());
                                    exInfo.tickSize = std::stod(filter.at(U("tickSize")).as_string());
                                }
                            }
                        }
                    }
                }
            }
        }
        catch (http_exception const& e)
        {
            std::wcout << e.what() << std::endl;
        }
        catch (const std::exception& e)
        {
            std::cerr << "Error parsing response: " << e.what() << std::endl;
        }
    }).wait();
    
    return exInfo;
}

