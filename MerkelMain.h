#pragma once

#include <vector>
#include <string>

#include "OrderBookEntry.h"
#include "OrderBook.h"
#include "Wallet.h"
#include "Candlestick.h"
#include "UserManager.h"

class MerkelMain
{
    public:
        MerkelMain();
        /** Call this to start the sim */
        void init();

    private:
        // Core Menu Functions
        void printMenu();
        void printHelp();
        void printMarketStats();
        void enterAsk();
        void enterBid();
        void printWallet();
        void gotoNextTimeframe();
        int getUserOption();
        void processUserOption(int userOption);
        // Add to private section of MerkelMain class:
void manageFunds();            // Task 3.1
void printRecentTransactions(); // Task 3.3
void printTradingStats();      // Task 3.4
void simulateTrading();

        // Task 1: Candlestick Functions
        void showCandlestickData();
        std::vector<Candlestick> computeCandlesticks(const std::vector<OrderBookEntry>& entries, 
                                                    const std::string& timeframe);

        // Task 2: Authentication Functions
        void handleAuthentication();
        
        // Member Variables
        std::string currentTime;
        OrderBook orderBook{"20200601.csv"};
        Wallet wallet; // Default wallet for system stats
        
        UserManager userManager;
        User* currentUser = nullptr; // Tracks the logged-in trading user
};