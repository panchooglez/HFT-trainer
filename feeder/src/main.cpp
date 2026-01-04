#include <iostream>
#include <vector>
#include <queue>
#include <random>
#include <cstring>
#include <sstream>
#include <map>
#include "protocol.hpp"

struct FastRNG {
    uint64_t s[2] = {0x42, 0xDEADBEEF};
    inline uint64_t next() {
        uint64_t x = s[0], y = s[1];
        s[0] = y; x ^= x << 23; s[1] = x ^ y ^ (x >> 17) ^ (y >> 26);
        return s[1] + y;
    }
    inline double uniform() { 
        return (next() >> 11) * 0x1.0p-53; 
    }
    inline double normal() {
        double u1 = uniform();
        double u2 = uniform();
        return sqrt(-2.0 * log(u1)) * cos(2.0 * 3.1415926535 * u2);
    }
};

struct InstrumentState {
    std::string ticker;
    double price;
    double volatility;
    double trend;
    bool is_crypto;
    int decimals;
    
    uint64_t next_event_time;
    double arrival_rate;
};

struct Event {
    uint64_t timestamp;
    int instrument_idx;

    bool operator>(const Event& other) const {
        return timestamp > other.timestamp;
    }
};

class MarketSimulator {
    std::vector<InstrumentState> instruments;
    std::priority_queue<Event, std::vector<Event>, std::greater<Event>> event_queue;
    FastRNG rng;
    uint64_t current_ts = 1600000000000;

public:
    void add_instrument(std::string ticker, double price, double vol, bool crypto) {
        InstrumentState ins;
        ins.ticker = ticker;
        ins.price = price;
        ins.volatility = vol;
        ins.trend = 0.0;
        ins.is_crypto = crypto;
        ins.decimals = crypto ? 2 : 2;
        
        ins.arrival_rate = 1000.0 + (rng.next() % 9000);
        
        ins.next_event_time = current_ts + next_interarrival_time(ins.arrival_rate);
        
        instruments.push_back(ins);
        event_queue.push({ins.next_event_time, (int)instruments.size() - 1});
    }

    // Calcula tiempo hasta el siguiente evento (Proceso de Poisson)
    uint64_t next_interarrival_time(double rate) {
        // -ln(U) / lambda
        double u = 1.0 - rng.uniform(); 
        double dt_seconds = -log(u) / rate;
        return (uint64_t)(dt_seconds * 1e9); 
    }

    template <typename T>
    void send(MsgType type, T& payload) {
        MsgHeader header { type, sizeof(T) };
        std::cout.write((const char*)&header, sizeof(MsgHeader));
        std::cout.write((const char*)&payload, sizeof(T));
    }

    void run(size_t total_messages) {
        for (size_t i = 0; i < total_messages; ++i) {
            if (event_queue.empty()) break;

            Event ev = event_queue.top();
            event_queue.pop();

            InstrumentState& ins = instruments[ev.instrument_idx];
            current_ts = ev.timestamp;

            double shock = ins.volatility * rng.normal();
            ins.price *= (1.0 + ins.trend + shock);
            if (ins.price < 0.01) ins.price = 0.01;

            bool is_quote = (rng.next() % 100) < 80;

            if (is_quote) {
                QuoteMsg q;
                q.timestamp = current_ts;
                std::strncpy(q.ticker, ins.ticker.c_str(), 8);
                
                double spread = ins.price * 0.0005;
                q.bid_price = ins.price - (spread/2);
                q.ask_price = ins.price + (spread/2);
                q.bid_size = (rng.next() % 10 + 1) * 100;
                q.ask_size = (rng.next() % 10 + 1) * 100;
                
                send(MsgType::QUOTE, q);
            } else {
                TradeMsg t;
                t.timestamp = current_ts;
                std::strncpy(t.ticker, ins.ticker.c_str(), 8);
                t.price = ins.price;
                t.quantity = (rng.next() % 5 + 1) * 100;
                t.side = (rng.next() % 2) ? 'B' : 'S';
                
                send(MsgType::TRADE, t);
            }

            // 4. Reprogramar este instrumento para el futuro
            ins.next_event_time = current_ts + next_interarrival_time(ins.arrival_rate);
            event_queue.push({ins.next_event_time, ev.instrument_idx});
        }
    }
};

int main(int argc, char* argv[]) {
    std::ios::sync_with_stdio(false);
    std::cin.tie(NULL);

    MarketSimulator sim;
    size_t count = 10000;
    
    std::vector<std::string> ticker_list;
    
    for(int i=1; i<argc; ++i) {
        std::string arg = argv[i];
        if (arg.find("--count=") == 0) count = std::stoull(arg.substr(8));
        if (arg.find("--tickers=") == 0) {
            std::string list = arg.substr(10);
            std::stringstream ss(list);
            std::string segment;
            while(std::getline(ss, segment, ',')) {
                ticker_list.push_back(segment);
            }
        }
    }

    if (ticker_list.empty()) {
        ticker_list = {"BTCUSD", "ETHUSD", "AAPL", "TSLA", "EURUSD"};
    }

    FastRNG init_rng;
    for (const auto& t : ticker_list) {
        double start_price = 100.0 + (init_rng.next() % 50000) / 100.0;
        double vol = 0.0001 + (init_rng.next() % 100) / 10000.0;
        bool is_crypto = (t.find("USD") != std::string::npos && t.length() > 4); 
        
        sim.add_instrument(t, start_price, vol, is_crypto);
    }

    sim.run(count);
    return 0;
}