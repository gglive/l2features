#include <iterator>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
// #include "rapidcsv.h"

#include "csv.hpp"
#include "features.h"

using namespace csv;
using namespace std;

std::vector<int> parse_array(const string &strArray)
{
    auto substr = strArray.substr(1, strArray.size() - 2);
    stringstream s_stream(substr);
    vector<int> res;
    while (s_stream.good())
    {
        string valStr;
        getline(s_stream, valStr, ',');
        if (!valStr.empty())
            res.push_back(std::stoi(valStr));
    }
    return res;
}

std::string secId2vtSymbol(int secId){
    std::stringstream secStr;
    secStr << setw(6) << setfill('0') << secId;
    if(secStr.str().substr(0,1) == "0" || secStr.str().substr(0,1) == "3")
    {
        return secStr.str() + ".SZSE";
    }else
    {
        return secStr.str() + ".SSE";
    }
}

void parseEquityTrade(CSVRow &row, EquityTrade &trade)
{
    auto trade_bs_flag =  row["trade_bs_flag"].get<std::string>();
    if(trade_bs_flag =="B" || trade_bs_flag == "S")
    {
        auto securityid = row["securityid"].get<int>();
        trade.vt_symbol = secId2vtSymbol(securityid);
        trade.trade_price = row["trade_price"].get<int>() / 10000.;
        trade.trade_volume = row["trade_volume"].get<int>();
        trade.trade_bs_flag = trade_bs_flag;
        trade.date = row["date"].get<size_t>();
        trade.time = row["time"].get<size_t>();
    }
}

// int main(){
//     CSVReader reader("d:\\temp_data\\hq-szl2-000001-tick-20201012152258606.csv");

//     for (CSVRow& row: reader) { // Input iterator
//         // for (CSVField& field: row) {
//         //     // By default, get<>() produces a std::string.
//         //     // A more efficient get<string_view>() is also available, where the resulting
//         //     // string_view is valid as long as the parent CSVRow is alive
//         //     std::cout << field.get<string_view>() << endl;
//         // }
//         auto bid_prices = row["bid_prices"].get<string>();
//         auto bpArray = parse_array(bid_prices);
//         std::cout << bid_prices << endl;
//         std::cout << bpArray[0] << endl;
// }
//     return 0;
// }

// int main(){
//     auto n = minuteByFreq("5m",93143000);
//     cout << n << endl;
// }

int main()
{
    CSVReader reader("d:\\temp_data\\hq-szl2-300661-trade-20200731173248172.csv");

    EquityFeatures ef;
    FeatureParams fparams;
    fparams.freq = "1m";
    ef.setParams(fparams);
    int i = 0;
    for (CSVRow &row : reader)
    { // Input iterator
        // for (CSVField& field: row) {
        //     // By default, get<>() produces a std::string.
        //     // A more efficient get<string_view>() is also available, where the resulting
        //     // string_view is valid as long as the parent CSVRow is alive
        //     std::cout << field.get<string_view>() << endl;
        // }
        i+=1;
        EquityTrade trade;
        parseEquityTrade(row, trade);
        if(trade.trade_volume!=0)
        {
            ef.onTrade(trade);
        }
        if(i%100 == 0){
            auto vec = ef.getBars("300661.SZSE");
            std::cout << vec.size()<< std::endl;
            // std::cout << vec.back()->date<< std::endl;
            // auto bar = vec.back();
            auto pt = vec.end()-1;
            auto bar = *pt;
            printf("|date: %zd|time: %zd|minute: %zd|UptickVolume:%d|DowntickVolume:%d|\n", bar->date, bar->time, bar->minute,bar->UptickVolume,bar->DowntickVolume);
        }
    }
    for(EquityMinuteBarPtr ptr: ef.getBars("300661.SZSE"))
    {
        std::cout <<ptr->vt_symbol << ' ' << ptr->RepeatDowntickVolume << ' ' << std::endl;
    };
    
    return 0;
}