#include "MerkelMain.h"
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>
#include "OrderBookEntry.h"
#include "CSVReader.h"

MerkelMain::MerkelMain()
{
}

void MerkelMain::init()
{
    // Task 2: Force login before entering the app
    handleAuthentication();

    currentTime = orderBook.getEarliestTime();
    while(true)
    {
        printMenu();
        int input = getUserOption();
        processUserOption(input);
    }
}

void MerkelMain::handleAuthentication()
{
    while (currentUser == nullptr)
    {
        std::cout << "\n--- Welcome to MerkelRex ---\n";
        std::cout << "1: Login\n2: Register\n3: Forgot ID\nChoose option: ";
        int choice = getUserOption();

        if (choice == 1)
        {
            std::string id, pass;
            std::cout << "Enter 10-digit ID: "; 
            std::getline(std::cin, id);
            std::cout << "Enter Password: "; 
            std::getline(std::cin, pass);
            
            currentUser = userManager.login(id, pass);
            if (!currentUser) std::cout << "Invalid credentials!\n";
        }
        else if (choice == 2)
        {
            std::string name, email, pass;
            std::cout << "Full Name: "; std::getline(std::cin, name);
            std::cout << "Email: "; std::getline(std::cin, email);
            std::cout << "Password: "; std::getline(std::cin, pass);
            
            std::string id = userManager.registerUser(name, email, pass);
            if (id == "EXISTS") std::cout << "User with this email already exists!\n";
            else std::cout << "Registration successful! YOUR LOGIN ID IS: " << id << "\n";
        }
        else if (choice == 3)
        {
            std::string email;
            std::cout << "Enter Registered Email: "; std::getline(std::cin, email);
            std::string id = userManager.findUsernameByEmail(email);
            if (id != "") std::cout << "Your Login ID is: " << id << "\n";
            else std::cout << "Email not found.\n";
        }
    }
    std::cout << "Successfully logged in as: " << currentUser->fullName << "\n";
}

void MerkelMain::printMenu()
{
    std::cout << "1: Print help" << std::endl;
    std::cout << "2: Print exchange stats" << std::endl;
    std::cout << "3: Make an offer" << std::endl;
    std::cout << "4: Make a bid" << std::endl;
    std::cout << "5: Print wallet" << std::endl;
    std::cout << "6: Continue" << std::endl;
    std::cout << "7: Show candlestick data" << std::endl;
    std::cout << "8: Manage Funds (Deposit/Withdraw)" << std::endl;
    std::cout << "9: View Recent Transactions" << std::endl;
    std::cout << "10: View Trading Statistics" << std::endl;
    std::cout << "============== " << std::endl;
    std::cout << "Current time is: " << currentTime << std::endl;
}

void MerkelMain::printHelp()
{
    std::cout << "Help - Choose options from the menu and follow on-screen instructions." << std::endl;
}

void MerkelMain::printMarketStats()
{
    for (std::string const& p : orderBook.getKnownProducts())
    {
        std::cout << "Product: " << p << std::endl;
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask, p, currentTime);
        std::cout << "Asks seen: " << entries.size() << std::endl;
        if (entries.size() > 0)
        {
            std::cout << "Max ask: " << OrderBook::getHighPrice(entries) << std::endl;
            std::cout << "Min ask: " << OrderBook::getLowPrice(entries) << std::endl;
        }
    }
}

void MerkelMain::enterAsk()
{
    std::cout << "Make an offer - enter the amount: product,price,amount, eg ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterAsk Bad input! " << input << std::endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1], tokens[2], currentTime, tokens[0], OrderBookType::ask
            );
            obe.username = currentUser->username;

            if (currentUser->wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet helps fulfill order " << std::endl;
                orderBook.insertOrder(obe);
                currentUser->addTransaction(currentTime, tokens[0], "ask", std::stod(tokens[2]), std::stod(tokens[1]));
            }
            else {
                std::cout << "Wallet has insufficient funds " << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << " MerkelMain::enterAsk Bad input " << std::endl;
        }
    }
}

void MerkelMain::enterBid()
{
    std::cout << "Make a bid - enter the amount: product,price,amount, eg ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterBid Bad input! " << input << std::endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1], tokens[2], currentTime, tokens[0], OrderBookType::bid
            );
            obe.username = currentUser->username;

            if (currentUser->wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet helps fulfill order " << std::endl;
                orderBook.insertOrder(obe);
                currentUser->addTransaction(currentTime, tokens[0], "bid", std::stod(tokens[2]), std::stod(tokens[1]));
            }
            else {
                std::cout << "Wallet has insufficient funds " << std::endl;
            }
        } catch (const std::exception& e) {
            std::cout << " MerkelMain::enterBid Bad input " << std::endl;
        }
    }
}

void MerkelMain::printWallet()
{
    std::cout << "User: " << currentUser->fullName << " | Wallet: " << std::endl;
    std::cout << currentUser->wallet.toString() << std::endl;
}

void MerkelMain::gotoNextTimeframe()
{
    std::cout << "Going to next time frame." << std::endl;
    
    // Task 4: Simulate activity for the new timeframe
    simulateTrading();

    // Matching logic
    for (std::string p : orderBook.getKnownProducts())
    {
        std::vector<OrderBookEntry> sales = orderBook.matchAsksToBids(p, currentTime);
        for (OrderBookEntry& sale : sales)
        {
            if (sale.username == currentUser->username)
            {
                currentUser->wallet.processSale(sale);
            }
        }
    }
    currentTime = orderBook.getNextTime(currentTime);
}

void MerkelMain::manageFunds()
{
    std::cout << "1: Deposit Money\n2: Withdraw Money\nChoose option: ";
    int choice = getUserOption();

    std::cout << "Enter currency (e.g. BTC): ";
    std::string currency; std::getline(std::cin, currency);
    std::cout << "Enter amount: ";
    std::string amtStr; std::getline(std::cin, amtStr);
    
    try {
        double amount = std::stod(amtStr);
        if (choice == 1) {
            currentUser->wallet.insertCurrency(currency, amount);
            currentUser->addTransaction(currentTime, currency, "deposit", amount, 1.0);
            std::cout << "Deposit successful." << std::endl;
        } else if (choice == 2) {
            if (currentUser->wallet.removeCurrency(currency, amount)) {
                currentUser->addTransaction(currentTime, currency, "withdraw", amount, 1.0);
                std::cout << "Withdrawal successful." << std::endl;
            } else {
                std::cout << "Insufficient funds!" << std::endl;
            }
        }
    } catch (...) { std::cout << "Invalid amount." << std::endl; }
}

void MerkelMain::simulateTrading()
{
    for (std::string const& p : orderBook.getKnownProducts())
    {
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask, p, currentTime);
        double baselinePrice = (entries.size() > 0) ? entries[0].price : 100.0;

        for (int i = 0; i < 5; ++i)
        {
            double askPrice = baselinePrice * (1.0 + (rand() % 50 / 1000.0)); 
            double bidPrice = baselinePrice * (1.0 - (rand() % 50 / 1000.0)); 

            OrderBookEntry ask{askPrice, 1.0, currentTime, p, OrderBookType::ask};
            OrderBookEntry bid{bidPrice, 1.0, currentTime, p, OrderBookType::bid};
            
            orderBook.insertOrder(ask);
            orderBook.insertOrder(bid);
        }
        std::cout << "Simulated 5 asks/bids for: " << p << std::endl;
    }
}

void MerkelMain::showCandlestickData()
{
    std::string product, typeInput, timeframe;
    std::cout << "Enter product (e.g. ETH/BTC): "; std::getline(std::cin, product);
    std::cout << "Enter order type (ask/bid): "; std::getline(std::cin, typeInput);
    std::cout << "Enter timeframe (daily/monthly/yearly): "; std::getline(std::cin, timeframe);

    OrderBookType type = OrderBookEntry::stringToOrderBookType(typeInput);
    
    std::vector<OrderBookEntry> allEntries;
    for (const OrderBookEntry& e : orderBook.getOrders(type, product, currentTime))
    {
        allEntries.push_back(e);
    }

    if (allEntries.empty()) {
        std::cout << "No data found for the current timeframe." << std::endl;
        return;
    }

    std::vector<Candlestick> candles = computeCandlesticks(allEntries, timeframe);
    
    std::cout << "\nDate       | Open     | High     | Low      | Close" << std::endl;
    for (const Candlestick& c : candles) {
        c.display();
    }
}

std::vector<Candlestick> MerkelMain::computeCandlesticks(const std::vector<OrderBookEntry>& entries, const std::string& timeframe)
{
    std::map<std::string, std::vector<OrderBookEntry>> grouped;
    for (const OrderBookEntry& e : entries)
    {
        std::string key = (timeframe == "daily") ? e.timestamp.substr(0, 10) :
                         (timeframe == "monthly") ? e.timestamp.substr(0, 7) : 
                                                    e.timestamp.substr(0, 4);
        grouped[key].push_back(e);
    }

    std::vector<Candlestick> result;
    for (auto const& [date, group] : grouped)
    {
        std::vector<OrderBookEntry> sorted = group;
        std::sort(sorted.begin(), sorted.end(), OrderBookEntry::compareByTimestamp);
        
        double open = sorted.front().price;
        double close = sorted.back().price;
        double high = sorted[0].price;
        double low = sorted[0].price;

        for (const auto& e : sorted) {
            if (e.price > high) high = e.price;
            if (e.price < low) low = e.price;
        }
        result.push_back(Candlestick(date, open, close, high, low));
    }
    return result;
}

void MerkelMain::printRecentTransactions()
{
    std::cout << "--- Last 5 Transactions ---" << std::endl;
    int start = (currentUser->transactions.size() > 5) ? (int)currentUser->transactions.size() - 5 : 0;
    for (size_t i = (size_t)start; i < currentUser->transactions.size(); ++i)
    {
        Transaction& t = currentUser->transactions[i];
        std::cout << t.timestamp << " | " << t.type << " | " << t.product << " | Amt: " << t.amount << std::endl;
    }
}

void MerkelMain::printTradingStats()
{
    int asks = 0, bids = 0;
    double totalSpent = 0;
    for (const auto& t : currentUser->transactions)
    {
        if (t.type == "ask") asks++;
        if (t.type == "bid") {
            bids++;
            totalSpent += (t.amount * t.price);
        }
    }
    std::cout << "--- User Statistics ---" << std::endl;
    std::cout << "Total Asks placed: " << asks << std::endl;
    std::cout << "Total Bids placed: " << bids << std::endl;
    std::cout << "Total money spent on bids: " << totalSpent << std::endl;
}

int MerkelMain::getUserOption()
{
    int userOption = 0;
    std::string line;
    std::getline(std::cin, line);
    try {
        userOption = std::stoi(line);
    } catch (...) {}
    return userOption;
}

void MerkelMain::processUserOption(int userOption)
{
    if (userOption == 1) printHelp();
    else if (userOption == 2) printMarketStats();
    else if (userOption == 3) enterAsk();
    else if (userOption == 4) enterBid();
    else if (userOption == 5) printWallet();
    else if (userOption == 6) gotoNextTimeframe();
    else if (userOption == 7) showCandlestickData();
    else if (userOption == 8) manageFunds();
    else if (userOption == 9) printRecentTransactions();
    else if (userOption == 10) printTradingStats();
    else std::cout << "Invalid choice." << std::endl;
}