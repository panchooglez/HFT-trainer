#include <iostream>
#include <iomanip>
#include "protocol.hpp"

void print_header() {
    std::cout << std::left << std::setw(10) << "TYPE" 
              << std::setw(10) << "TICKER" 
              << std::setw(15) << "TIME" 
              << std::setw(25) << "DETAILS" << "\n";
    std::cout << std::string(60, '-') << "\n";
}

void handle_trade(MsgHeader& h) {
    TradeMsg t;
    std::cin.read(reinterpret_cast<char*>(&t), sizeof(TradeMsg));
    
    std::cout << std::left << std::setw(10) << "TRADE" 
              << std::setw(10) << t.ticker 
              << std::setw(15) << t.timestamp 
              << "Px:" << t.price << " Qty:" << t.quantity << " [" << t.side << "]\n";
}

void handle_quote(MsgHeader& h) {
    QuoteMsg q;
    std::cin.read(reinterpret_cast<char*>(&q), sizeof(QuoteMsg));
    
    std::cout << std::left << std::setw(10) << "QUOTE" 
              << std::setw(10) << q.ticker 
              << std::setw(15) << q.timestamp 
              << "Bid:" << q.bid_price << " (" << q.bid_size << ") | "
              << "Ask:" << q.ask_price << " (" << q.ask_size << ")\n";
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(NULL);

    print_header();

    MsgHeader header;
    while (std::cin.read(reinterpret_cast<char*>(&header), sizeof(MsgHeader))) {
        
        switch (header.type) {
            case MsgType::TRADE:
                handle_trade(header);
                break;
            case MsgType::QUOTE:
                handle_quote(header);
                break;
            default:
                std::cin.ignore(header.size);
                std::cout << "UNKNOWN MSG (" << header.size << " bytes)\n";
                break;
        }
    }
    return 0;
}