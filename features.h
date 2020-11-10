
#include <string>
#include <unordered_map>
#include <vector>
#include <math.h>
#include <memory>

struct Tick
{
    std::string vt_symbol;
    size_t date;
    size_t time;
    double preclose;
    double last;
    size_t total_volume_trade;
    size_t total_value_trade;
};

struct EquityTick : Tick
{
    std::vector<double> bid_prices;
    std::vector<size_t> bid_volumes;
    std::vector<double> ask_prices;
    std::vector<size_t> ask_volumes;
};

struct Bar
{
    std::string vt_symbol;
    size_t date;
    size_t time;
    double open;
    double high;
    double low;
    double close;
    size_t volume;
};

struct EquityMinuteBar : Bar
{
    size_t minute;
    // trade
    size_t UptickVolume = 0;
    size_t DowntickVolume = 0;
    size_t RepeatUptickVolume = 0;
    size_t RepeatDowntickVolume = 0;
};

struct Trade
{
    std::string vt_symbol;
    size_t date;
    size_t time;
    double trade_price = 0;
    size_t trade_volume = 0;
    std::string trade_bs_flag;
    size_t trade_sell_no;
    size_t trade_buy_no;
};

struct EquityTrade : Trade
{
};

struct Order
{
};

struct FeatureParams
{
    char *freq = "1m";
};

class Features
{
public:
    virtual void onTrade(Trade &trade) = 0;
    // virtual void onTick(const Tick & tick);
    // virtual void onOrder(const Order & order);

    static size_t minuteByFreq(const char *freq, size_t time);
    
};

typedef std::shared_ptr<EquityMinuteBar> EquityMinuteBarPtr;

class EquityFeatures : public Features
{
public:
    void onTrade(Trade &trade) override;
    // void onTick(const Tick & tick);
    // void onOrder(const Order & order);
    std::vector<EquityMinuteBarPtr> getBars(std::string vt_symbol);
    void setParams(FeatureParams &params)
    {
        _params = &params;
    };

    EquityMinuteBarPtr findCurrBar(std::string vt_symbol, size_t min_num)
    {
        auto bar_map_idx = _currBars.find(vt_symbol);
        if (bar_map_idx == _currBars.end())
        {
            return NULL;
        }
        else
        {
            auto bar_map = bar_map_idx->second;
            auto idx = bar_map.find(min_num);
            if (idx == bar_map.end())
            {
                return NULL;
            }
            else
            {
                return idx->second;
            }
        }
    }

private:
    double _lastTradePrice = 0.;
    size_t _lastTradeDirection = 1; // 1 for up, -1 for down
    EquityMinuteBarPtr _preMBar = NULL;
    FeatureParams *_params;
    std::unordered_map<std::string, std::vector<EquityMinuteBarPtr>> _hisBars;
    std::unordered_map<std::string, std::unordered_map<size_t, EquityMinuteBarPtr>> _currBars;
};