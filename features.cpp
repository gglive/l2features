
#include "features.h"


size_t Features::minuteByFreq(const char *freq, size_t time)
{
    // 93143000
    if (freq[strlen(freq) - 1] == 'm')
    {
        std::string temp = freq;
        auto n = std::atoi(temp.substr(0, strlen(freq) - 1).c_str());
        auto minute = ceil(time / 100000.0 / n) * n;
        return size_t(minute);
    };
    return 0;
}

std::vector<EquityMinuteBarPtr> EquityFeatures::getBars(std::string vt_symbol)
{
    auto idx = _hisBars.find(vt_symbol);
    if(idx==_hisBars.end())
    {
        return std::vector<EquityMinuteBarPtr>();
    }else{
        return idx->second;
    }
}

 void EquityFeatures::onTrade(Trade& trade)
{
    auto min_num = Features::minuteByFreq(_params->freq, trade.time);
    auto min_idx = min_num + trade.date*10000;
    auto vt_symbol = trade.vt_symbol;
    EquityMinuteBarPtr bar = findCurrBar(vt_symbol,min_num);
    if(bar == NULL){
        // new bar
        EquityMinuteBarPtr barPtr = std::make_shared<EquityMinuteBar>();
        if(_preMBar!=NULL){
            _hisBars[vt_symbol].push_back(_preMBar);
        }
        bar = barPtr;
        bar->vt_symbol = vt_symbol;
        bar->date = trade.date;
        bar->time = min_num;
        auto bar_map_idx = _currBars.find(vt_symbol);
        if(bar_map_idx ==_currBars.end()){
            std::unordered_map<size_t,EquityMinuteBarPtr> bar_kv;
            bar_kv.emplace(min_idx, barPtr);
            _currBars.emplace(vt_symbol, bar_kv);
        }else{
            auto bar_map = bar_map_idx->second;
            bar_map.emplace(min_idx, barPtr);
        }
        if(_preMBar==NULL) _preMBar = bar;
    }
    if (_lastTradePrice != 0.){
        if(trade.trade_price > _lastTradePrice)
        {
            bar->UptickVolume += trade.trade_volume;
            _lastTradeDirection = 1;
        }else if(trade.trade_price < _lastTradePrice)
        {
            bar->DowntickVolume += trade.trade_volume;
            _lastTradeDirection = -1;
        }else{
            if (_lastTradePrice == 1){
                bar->RepeatUptickVolume += trade.trade_volume;
            }else{
                bar->RepeatDowntickVolume += trade.trade_volume;
            }
        }
    }
    _lastTradePrice = trade.trade_price;
};