
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
    size_t min_idx = min_num + trade.date*10000;
    auto vt_symbol = trade.vt_symbol;
    EquityMinuteBarPtr bar = findCurrBar(vt_symbol,min_idx);
    if(bar == NULL){
        // new bar
        bar = std::make_shared<EquityMinuteBar>();
        if(_preMBar!=NULL){
            printf("|date: %zd|time: %zd|minute: %zd|UptickVolume:%zd|DowntickVolume:%zd|RepeatUptickVolume:%zd|RepeatDowntickVolume:%zd|total:%zd|\n",
                     _preMBar->date, _preMBar->time, _preMBar->minute,_preMBar->UptickVolume,_preMBar->DowntickVolume, 
                     _preMBar->RepeatUptickVolume,
                     _preMBar->RepeatDowntickVolume,
                     _preMBar->totalVolume
                     );
            _hisBars[vt_symbol].push_back(_preMBar);
            _preMBar = bar;
            // printf("|vec size:%d|\n",_hisBars[vt_symbol].size());
        }
        // bar = barPtr;
        bar->vt_symbol = vt_symbol;
        bar->date = trade.date;
        bar->minute = min_num;
        auto bar_map_idx = _currBars.find(vt_symbol);
        if(bar_map_idx ==_currBars.end()){
            std::unordered_map<size_t,EquityMinuteBarPtr> bar_kv;
            bar_kv.emplace(min_idx, bar);
            _currBars.emplace(vt_symbol, bar_kv);
        }else{
            auto bar_map = &bar_map_idx->second;
            // bar_map.emplace(min_idx, bar);
            (*bar_map)[min_idx] =  bar;
            // bar_map_idx->second = bar_map;
        }
        if(_preMBar==NULL) _preMBar = bar;  // init 
    }
    
    bar->time = trade.time;
    bar->totalVolume += trade.trade_volume;
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
            if (_lastTradeDirection == 1){
                bar->RepeatUptickVolume += trade.trade_volume;
            }else if(_lastTradeDirection == -1){
                bar->RepeatDowntickVolume += trade.trade_volume;
            }else{
                // open auction,no direction
            }
        }
    }
    _lastTradePrice = trade.trade_price;
};