#ifndef _OandA_API_h
#define _OandA_API_h

#include <sstream>
#include <string>

//#include <curlpp/cURLpp.hpp>
//#include <curlpp/Easy.hpp>
//#include <curlpp/Options.hpp>

#include <json/json.h>
#include <restclient-cpp/connection.h>
//#include <restclient-cpp/restclient.h>
#include <boost/date_time/posix_time/posix_time.hpp>

//#include "f"
#include "../../functions.hpp"

using namespace std;

namespace OA
{
    
    struct Order
    {
        string id;
        string symbol;
        int units;
        string direction;
        string type;
        string timeStamp;
        double price;
        double takeProfit;
        double stopLoss;
        string expiry;
        double upperBound;
        double lowerBound;
        double trailingStop;        
    };
        
    struct Trade
    {
        string id;
        string symbol;
        int units;
        string direction;
        string timeStamp;
        double price;
        double takeProfit;
        double stopLoss;
        
        
        
    };
        
    struct Position
    {
        
        
    };
    
    struct Bar
    {
        double open;
        double close;
        double high;
        double low;
        string timeStamp;

        bool operator == (const string& test) const
        {
            return  test == timeStamp;
        }
        Bar()
        {
        }
        Bar(double tempOpen, double tempClose, double tempHigh, double tempLow, string tempTimeStamp)
        {
            open = tempOpen;
            close = tempClose;
            high = tempHigh;
            low = tempLow;
            timeStamp = tempTimeStamp;
        }
    };

/*
    struct Instrument
    {
        string symbol;
        string granularity;
        double pipValue;
        vector<Bar> theBars;
    };
*/


    class OA_API;
    class Instrument;
        
}

class OA::OA_API
{

    public:
    
        OA_API();
        ~OA_API();
    
        void useAccount(string localAccount);
        void useInstrument(string localInstrument);
        
        bool createOrder(string instrument, int units, string side, string type, string ex, double price, double takeProfit);
        bool getOrderInfo(string id);
        bool modifyOrder(string id);
        bool deleteOrder(unsigned long long id);
        
        bool refreshOrders();
        bool refreshTrades();
        bool refreshPositions();

        bool initInstrument(OA::Instrument * localInstrument);
        vector<OA::Bar> getBars(string instrument, string granularity, boost::posix_time::ptime start, boost::posix_time::ptime end, int count = 0);
        bool refreshBars(OA::Instrument * localInstrument);
        


        vector<Instrument> theInstruments;
    
        vector<OA::Order> orders;
        vector<OA::Trade> trades;
        vector<OA::Position> positions;
    
    
    private:
    
        RestClient::Connection *restConn;
        string account = "";
        string instrument = "";
        string authCode = "ca2624a2fcb1374847a802ed7bb38295-813cd7c17bdf044a90ebcd274b1414c4";
        
        
        //OA::Order createOrder_JSON(Json::Value);


};

class OA::Instrument
{
    public:

        string symbol;
        string granularity;
        double pipValue;
        vector<Bar> theBars;



        Instrument();
        ~Instrument();

        void setSymbol(string theSymbol);
        void setGranularity(string theGranularity);
        void setPipValue(double value);

        void addBars(vector<OA::Bar> localBars);
        int getBarCount();
        vector<OA::Bar> getBars();
        vector<OA::Bar> getBars(int count);

private:









};




//Json::Value getInstrument(string theInstrument, string account);
//Json::Value getCurrentPrices(string theInstrument);
//Json::Value getCurrentOrders(string account);
//Json::Value getOpenTrades(string account);
//Json::Value getHistory(string instrument, string granularity, boost::posix_time::ptime start, boost::posix_time::ptime end, int count = 0);
//bool deleteOrder(unsigned long long int id, string account);

//bool createOrder(string instrument, int units, string side, string type, string ex, double price, double takeProfit, string account);



#endif
