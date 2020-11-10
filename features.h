
#include <string>
#include <unordered_map>
#include <vector>
#include <math.h>

struct Tick
{
    std::string vt_symbol;
    int date;
    int time;
    double preclose;
    double last;
    int total_volume_trade;
    int total_value_trade;
};

struct EquityTick : Tick
{
    std::vector<double> bid_prices;
    std::vector<int> bid_volumes;
    std::vector<double> ask_prices;
    std::vector<int> ask_volumes;

};

struct Bar
{
    int date;
    int time;
    double open;
    double high;
    double low;
    double close;
    int volume;
};

struct EquityMinuteBar : Bar
{
    int minute;
    // trade
    int UptickVolume = 0;
    int DowntickVolume = 0;
    int RepeatUptickVolume = 0;
    int RepeatDowntickVolume = 0;

};

struct Trade
{
    std::string vt_symbol;
    int date;
    int time; 
    double trade_price;
    double trade_volume;
    char* trade_bs_flag;
    int trade_sell_no;
    int trade_buy_no;
};

struct EquityTrade:Trade
{
    
};

struct Order
{
    
};


struct FeatureParams
{
  char* freq = "1m";  
};

int minuteByFreq(const char * freq, int time)
    {
        // 93143000 
        if(freq[strlen(freq)-1] == 'm')
        {
            std::string temp  = freq;
            auto n = std::atoi(temp.substr(0, strlen(freq)-1).c_str());
            auto minute = ceil(time/ 100000.0 / n)*n;
            return int(minute);
        };
        return 0;
    }

class Features
{
public:
    virtual void onTrade(const Trade & trade)=0;
    // virtual void onTick(const Tick & tick);
    // virtual void onOrder(const Order & order);

};


typedef std::shared_ptr<EquityMinuteBar> EquityMinuteBarPtr;

class EquityFeatures: public Features
{
public:
    void onTrade(const Trade & trade) override;
    // void onTick(const Tick & tick);
    // void onOrder(const Order & order);
    void setParams(FeatureParams & params)
    {
        _params = &params;
    };
    EquityMinuteBarPtr findCurrBar(std::string vt_symbol, int min_num)
    {
        auto bar_map_idx =_currBars.find(vt_symbol);
        if(bar_map_idx == _currBars.end()){
            return NULL;
        }else{
            auto bar_map = bar_map_idx->second;
            auto idx  = bar_map.find(min_num);
            if(idx == bar_map.end()){
                return NULL;
            }else{
                return idx->second;
            }
        }
    }
private:
    double _lastTradePrice = 0.;
    int _lastTradeDirection = 1; // 1 for up, -1 for down
    EquityMinuteBarPtr _preMBar = NULL;
    FeatureParams* _params;
    std::unordered_map<std::string, std::vector<EquityMinuteBarPtr>> _hisBars;
    std::unordered_map<std::string, std::unordered_map<int, EquityMinuteBarPtr>> _currBars;
};