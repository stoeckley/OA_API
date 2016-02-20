#include "OandA_API.hpp"


OA::OA_API::OA_API()
{
    RestClient::init();
    restConn = new RestClient::Connection("https://api-fxpractice.oanda.com");
}

void OA::OA_API::useAccount(string localAccount)
{
    account = localAccount;
}

void OA::OA_API::useInstrument(string localInstrument)
{
    instrument = localInstrument;
}

bool OA::OA_API::refreshOrders()
{
    //bool returnValue = false;
    vector<OA::Order> localOrders;
    Json::Value currentOrders;
    Json::Reader reader;

    if (account == "")
    {
        return false;
    }

    string url = "/v1/accounts/" + account + "/orders";

    RestClient::HeaderFields headers;
    headers["Authorization"] = "Bearer " + authCode;
    headers["X-Accept-Datetime-Format"] = "RFC3339";
    headers["Content-Type"] = "application/x-www-form-urlencoded";
    restConn->SetHeaders(headers);
    RestClient::Response r = restConn->get(url);

    reader.parse(r.body, currentOrders);

    cout << "Current Orders:--------------------------------------" << endl << currentOrders << endl;

    for (Json::ValueIterator itr = currentOrders["orders"].begin() ; itr != currentOrders["orders"].end() ; itr++)
    {
        Json::Value singleOrderJSON = *itr;
        //cout << "This is the single orders: " << endl << singleOrderJSON << endl;

        OA::Order singleOrder;
        singleOrder.id = singleOrderJSON["id"].asInt64();
        singleOrder.symbol = singleOrderJSON["instrument"].asString();
        singleOrder.units = singleOrderJSON["units"].asDouble();

        localOrders.push_back(singleOrder);
    }

    cout << "There are " << localOrders.size() << " orders in the vector" << endl;

    orders = localOrders;

    return true;
}

bool OA::OA_API::refreshTrades()
{
    //bool returnValue = false;
    vector<OA::Trade> localTrades;

    Json::Value currentTrades;
    Json::Reader reader;

    if (account == "")
    {
        return false;
    }

    string url = "/v1/accounts/" + account + "/trades";

    RestClient::HeaderFields headers;
    headers["Authorization"] = "Bearer " + authCode;
    headers["X-Accept-Datetime-Format"] = "RFC3339";
    headers["Content-Type"] = "application/x-www-form-urlencoded";
    restConn->SetHeaders(headers);
    RestClient::Response r = restConn->get(url);
    //RestClient::response r = RestClient::get(url, headers);

    reader.parse(r.body, currentTrades);
    cout << "Current Trades: -----------------------------------------------" << endl << currentTrades << endl;

    for (Json::ValueIterator itr = currentTrades["trades"].begin() ; itr != currentTrades["trades"].end() ; itr++)
    {
        Json::Value singleTradeJSON = *itr;
        //cout << "This is the single orders: " << endl << singleTradeJSON << endl;

        OA::Trade singleTrade;
        singleTrade.id = singleTradeJSON["id"].asInt64();
        singleTrade.symbol = singleTradeJSON["instrument"].asString();
        singleTrade.units = singleTradeJSON["units"].asDouble();

        localTrades.push_back(singleTrade);
    }

    cout << "There are " << localTrades.size() << " trades in the vector" << endl;

    trades = localTrades;

    return true;

}

bool OA::OA_API::refreshPositions()
{

    return false;
}

bool OA::OA_API::createOrder(string instrument, int units, string side, string type, string ex, double price, double takeProfit)
{
    //boost::posix_time::ptime time_now(second_clock::local_time());
    if (account.length() == 0)
    {
        return false;
    }
    bool returnValue;
    string url = "https://api-fxpractice.oanda.com/v1/accounts/" + account + "/orders/";
    string data = "instrument=" + instrument;
    data += "&units=" + to_string(units);
    data += "&side=" + side;
    data += "&type=" + type;
    data += "&expiry=" + ex;
    data += "2016-04-01T00%3A00%3A00Z";
    data += "&price=" + to_string(price);
    data += "&takeProfit=" + to_string(takeProfit);
    
    RestClient::HeaderFields headers;
    headers["Authorization"] = "Bearer ca2624a2fcb1374847a802ed7bb38295-813cd7c17bdf044a90ebcd274b1414c4";
    headers["X-Accept-Datetime-Format"] = "RFC3339";
    headers["Content-Type"] = "application/x-www-form-urlencoded";
    restConn->SetHeaders(headers);
    //cout << "Data: " << data << endl << endl;

    RestClient::Response r = restConn->post(url, data);

    cout << "Return code: " << r.code << ", " << r.body << endl;

    if (r.code == 201)
    {
        returnValue = true;
    }
    else
    {
        returnValue = false;
    }

    if (returnValue)
    {
        refreshOrders();    
    }
        
    return returnValue;
}

bool OA::OA_API::deleteOrder(unsigned long long int id)
{
    bool returnValue = false;
    if (account.length() == 0)
    {
        return false;
    }

    string url = "/v1/accounts/" + account + "/orders/" + to_string(id);

    RestClient::HeaderFields headers;
    headers["Authorization"] = "Bearer ca2624a2fcb1374847a802ed7bb38295-813cd7c17bdf044a90ebcd274b1414c4";
    headers["X-Accept-Datetime-Format"] = "RFC3339";
    headers["Content-Type"] = "application/x-www-form-urlencoded";

    restConn->SetHeaders(headers);

    RestClient::Response r = restConn->del(url);
    
    
    if (r.code == 200)
    {
        returnValue = true;
        //cout << "Order " << to_string(id) <<  " successfully deleted" << endl;
    }
    else
    {
        returnValue = false;
        cout << "There was a problem deleting the order: " << r.code << endl;
        cout << "Response body: " << r.body << endl;
    }
    
    if (returnValue)
    {
        refreshOrders();    
    }

    return returnValue;
}

bool OA::OA_API::initInstrument(OA::Instrument *localInstrument)
{
    if (localInstrument->getBarCount() != 0)
    {
        cout << "Looks like its ready to go!" << endl;
        return false;
    }

    //Get initial bar load up

    //boost::posix_time::ptime(boost::)

    vector<OA::Bar> starterBars = getBars(localInstrument->symbol, localInstrument->granularity, boost::posix_time::ptime(boost::posix_time::not_a_date_time), boost::posix_time::ptime(boost::posix_time::not_a_date_time), 100);

    localInstrument->addBars(starterBars);

    return true;
}

vector<OA::Bar> OA::OA_API::getBars(string instrument, string granularity, boost::posix_time::ptime start, boost::posix_time::ptime end, int count)
{

    vector<OA::Bar> localBars;
    Json::Value historyBars;
    Json::Reader reader;

    RestClient::HeaderFields headers;
    headers["Authorization"] = "Bearer ca2624a2fcb1374847a802ed7bb38295-813cd7c17bdf044a90ebcd274b1414c4";
    headers["X-Accept-Datetime-Format"] = "RFC3339";
    headers["Content-Type"] = "application/x-www-form-urlencoded";
    restConn->SetHeaders(headers);

    string url = "/v1/candles?instrument=" + instrument + "&granularity=" + granularity + "&candleFormat=midpoint";

    cout << "Testing start and end: " << boost::posix_time::to_iso_extended_string(start) << ", End: " << boost::posix_time::to_iso_extended_string(end) << endl;

    if (!start.is_not_a_date_time() && !end.is_not_a_date_time())
    {
        cout << "Getting history for Start: " << boost::posix_time::to_iso_extended_string(start) << ", End: " << boost::posix_time::to_iso_extended_string(end) << endl;

        string startString = urlEncode(to_iso_extended_string(start) + "Z" );
        string endString = urlEncode(to_iso_extended_string(end) + "Z" );

        url += "&start=";
        url += startString;

        url += "&end=";
        url += endString;

        cout << "URL: " << url << endl;
    }
    else if (!start.is_not_a_date_time() && end.is_not_a_date_time() && count != 0)
    {
        string startString = urlEncode(to_iso_extended_string(start) + "Z" );

        url += "&start=";
        url += startString;

        url += "&count=";
        url += to_string(count);

        cout << "URL: " << url << endl;
    }
    else if (start.is_not_a_date_time() && !end.is_not_a_date_time() && count != 0)
    {
        string endString = urlEncode(to_iso_extended_string(end) + "Z" );

        url += "&end=";
        url += endString;

        url += "&count=";
        url += to_string(count);

        cout << "URL: " << url << endl;
    }
    else if (count != 0)
    {
        url += "&count=";
        url += to_string(count);
    }


    RestClient::Response r = restConn->get(url);
    if (r.code != 200)
    {
        cout << r.body << endl;
    }

    reader.parse(r.body, historyBars);

    for (Json::ValueIterator itr = historyBars["candles"].begin() ; itr != historyBars["candles"].end() ; itr++)
    {

        Json::Value singleCandleJSON = *itr;
        //cout << "This is the single orders: " << endl << singleOrderJSON << endl;

        if (singleCandleJSON["complete"].asBool())
        {
            OA::Bar singleCandle;

            singleCandle.open = singleCandleJSON["openMid"].asDouble();
            singleCandle.high = singleCandleJSON["highMid"].asDouble();
            singleCandle.low = singleCandleJSON["lowMid"].asDouble();
            singleCandle.close = singleCandleJSON["closeMid"].asDouble();
            singleCandle.timeStamp = singleCandleJSON["time"].asString();

            localBars.push_back(singleCandle);
        }
    }


    return localBars;
}

bool OA::OA_API::refreshBars(OA::Instrument * localInstrument)
{
    cout << "Number of bars: " << localInstrument->theBars.size() << endl;

    if (localInstrument->theBars.size() == 0)
    {
        cout << "No bars, we should add 100+" << endl;

    }
    else
    {
        cout << "There are bars, add to them" << endl;

    }

    return false;
}



//Instrument Implementation

OA::Instrument::Instrument()
{
    symbol = "";
    granularity = "";
    pipValue = 0;

}

void OA::Instrument::addBars(vector<OA::Bar> localBars)
{
    for (auto &iter : localBars)
    {
        bool test = false;
        vector<OA::Bar> localBar = iter;
        for (auto &it : theBars)
        {
            if (*it == localBar)
            {
                test = true;
            }
        }

        if (test)
        {
            theBars.push_back(localBar);
        }
    }

}

int OA::Instrument::getBarCount()
{
    return theBars.size();
}

vector<OA::Bar> OA::Instrument::getBars()
{
    return theBars;
}

void OA::Instrument::setGranularity(string theGranularity)
{
    granularity = theGranularity;
}

void OA::Instrument::setPipValue(double value)
{
    pipValue = value;
}

void OA::Instrument::setSymbol(string theSymbol)
{
    symbol = theSymbol;
}




















/*

Json::Value getInstrument(string theInstrument, string account)
{
    Json::Value instrumentList;
    Json::Reader reader;

    string url = "https://api-fxpractice.oanda.com/v1/instruments?accountId=" + account + "&instruments=" + theInstrument;

    RestClient::headermap headers;
    headers["Authorization"] = "Bearer ca2624a2fcb1374847a802ed7bb38295-813cd7c17bdf044a90ebcd274b1414c4";
    headers["X-Accept-Datetime-Format"] = "RFC3339";
    headers["Content-Type"] = "application/x-www-form-urlencoded";

    RestClient::response r = RestClient::get(url, headers);
    
    bool success = reader.parse(r.body, instrumentList);
    if (!success)
    {
        cout << "There was an error parsing" << endl;
    }   
    
    //cout << "getInstrumentList return value: " << instrumentList << endl;

    return instrumentList;
}
*/
/*
Json::Value getCurrentPrices(string theInstrument)
{
    Json::Value currentPrices;
    Json::Reader reader;

    string url = "https://api-fxpractice.oanda.com/v1/prices?instruments=" + theInstrument;

    RestClient::headermap headers;
    headers["Authorization"] = "Bearer ca2624a2fcb1374847a802ed7bb38295-813cd7c17bdf044a90ebcd274b1414c4";
    headers["X-Accept-Datetime-Format"] = "RFC3339";
    headers["Content-Type"] = "application/x-www-form-urlencoded";

    RestClient::response r = RestClient::get(url, headers);
    
    bool success = reader.parse(r.body, currentPrices);
    cout << "Success? " << success << endl;
    
    //cout << "getCurrentPrices return value: " << currentPrices["prices"] << endl;

    return currentPrices["prices"];

}
*/
/*
Json::Value getCurrentOrders(string account)
{

    Json::Value currentOrders;
    Json::Reader reader;

    string url = "https://api-fxpractice.oanda.com/v1/accounts/" + account + "/orders";

    RestClient::headermap headers;
    headers["Authorization"] = "Bearer ca2624a2fcb1374847a802ed7bb38295-813cd7c17bdf044a90ebcd274b1414c4";
    headers["X-Accept-Datetime-Format"] = "RFC3339";
    headers["Content-Type"] = "application/x-www-form-urlencoded";

    RestClient::response r = RestClient::get(url, headers);

    reader.parse(r.body, currentOrders);
    //cout << "Success? " << success << endl;
    
    //cout << "getCurrentOrders return value: " << currentOrders << endl;

    return currentOrders["orders"];

}
*/
/*
Json::Value getOpenTrades(string account)
{

    Json::Value currentTrades;
    Json::Reader reader;

    string url = "https://api-fxpractice.oanda.com/v1/accounts/" + account + "/trades";

    RestClient::headermap headers;
    headers["Authorization"] = "Bearer ca2624a2fcb1374847a802ed7bb38295-813cd7c17bdf044a90ebcd274b1414c4";
    headers["X-Accept-Datetime-Format"] = "RFC3339";
    headers["Content-Type"] = "application/x-www-form-urlencoded";

    RestClient::response r = RestClient::get(url, headers);
    
    reader.parse(r.body, currentTrades);
    //cout << "Success? " << success << endl;
    
    //cout << "getOpenTrades return value: " << currentTrades << endl;

    return currentTrades["trades"];

}
*/
/*

Json::Value getHistory(string instrument, string granularity, boost::posix_time::ptime start, boost::posix_time::ptime end, int count)
{
    RestClient::headermap headers;
    headers["Authorization"] = "Bearer ca2624a2fcb1374847a802ed7bb38295-813cd7c17bdf044a90ebcd274b1414c4";
    headers["X-Accept-Datetime-Format"] = "RFC3339";
    headers["Content-Type"] = "application/x-www-form-urlencoded";
    
    Json::Value historyBars;
    Json::Reader reader;
    string url = "https://api-fxpractice.oanda.com/v1/candles?instrument=" + instrument + "&granularity=" + granularity + "&candleFormat=midpoint";

    cout << "Testing start and end: " << boost::posix_time::to_iso_extended_string(start) << ", End: " << boost::posix_time::to_iso_extended_string(end) << endl;



    if (!start.is_not_a_date_time() && !end.is_not_a_date_time())
    {
        cout << "Getting history for Start: " << boost::posix_time::to_iso_extended_string(start) << ", End: " << boost::posix_time::to_iso_extended_string(end) << endl;
        
        string startString = urlEncode(to_iso_extended_string(start) + "Z" );
        string endString = urlEncode(to_iso_extended_string(end) + "Z" );
        
        url += "&start=";
        url += startString;
        
        url += "&end=";
        url += endString;
        
        cout << "URL: " << url << endl;
    }
    else if (!start.is_not_a_date_time() && end.is_not_a_date_time() && count != 0)
    {
        string startString = urlEncode(to_iso_extended_string(start) + "Z" );
        
        url += "&start=";
        url += startString;
        
        url += "&count=";
        url += to_string(count);
        
        cout << "URL: " << url << endl;
    }
    else if (start.is_not_a_date_time() && !end.is_not_a_date_time() && count != 0)
    {
        string endString = urlEncode(to_iso_extended_string(end) + "Z" );
        
        url += "&end=";
        url += endString;
        
        url += "&count=";
        url += to_string(count);
        
        cout << "URL: " << url << endl;
    }
    else if (count != 0)
    {
        url += "&count=";
        url += to_string(count);
    
    
    }

    

    //cout << "This is the history URL: " << url << endl;

    RestClient::response r = RestClient::get(url, headers);
    if (r.code != 200)
    {
        cout << r.body << endl;
    }
        
    reader.parse(r.body, historyBars);
    //cout << "Success? " << success << endl;
    
    //cout << "getHistory return value: " << historyBars << endl;

    return historyBars;
    
}

*/









