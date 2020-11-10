
#include "features.h"

void EquityFeatures::onTrade(const Trade& trade)
{
    auto min_num = minuteByFreq(_params->freq, trade.time) + trade.date*10000;
    auto vt_symbol = trade.vt_symbol;
    EquityMinuteBarPtr bar = findCurrBar(vt_symbol,min_num);
    if(bar == NULL){
        // new bar
        EquityMinuteBarPtr barPtr = std::make_shared<EquityMinuteBar>();
        if(_preMBar!=NULL){
            _hisBars[vt_symbol].push_back(_preMBar);
        }
        bar = barPtr;
        auto bar_map_idx = _currBars.find(vt_symbol);
        if(bar_map_idx ==_currBars.end()){
            std::unordered_map<int,EquityMinuteBarPtr> bar_kv;
            bar_kv.emplace(min_num, barPtr);
            _currBars.emplace(vt_symbol, bar_kv);
        }else{
            auto bar_map = bar_map_idx->second;
            bar_map.emplace(min_num, barPtr);
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