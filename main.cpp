#include <iterator>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
// #include "rapidcsv.h"

# include "csv.hpp"
# include "features.h"

using namespace csv;
using namespace std;

std::vector<int> parse_array(const string& strArray){
    auto substr = strArray.substr(1,strArray.size() -2);
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

EquityTrade parseEquityTrade(CSVRow & row)
{   
    EquityTrade et;
    et.trade_price = row["trade_price"].get<int>()/10000.;
    et.trade_volume = row["trade_volume"].get<int>();
    et.date = row["date"].get<int>();
    et.time = row["time"].get<int>();
    return et;
}


int main(){
    CSVReader reader("d:\\temp_data\\hq-szl2-300122-trade-20200805141001722.csv");

    EquityFeatures ef;

    for (CSVRow& row: reader) { // Input iterator
        // for (CSVField& field: row) {
        //     // By default, get<>() produces a std::string.
        //     // A more efficient get<string_view>() is also available, where the resulting
        //     // string_view is valid as long as the parent CSVRow is alive
        //     std::cout << field.get<string_view>() << endl;
        // }
        auto trade = parseEquityTrade(row);
        ef.onTrade(trade);
}
    return 0;
}