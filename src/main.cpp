#include <iostream>
#include <cstdlib>
#include <string>
#include <functional>
#include <chrono>
#include <asio.hpp>
#include <asio/steady_timer.hpp>

/* simple count down class showing use of asio service
 *
 * This is based on a tutorial sample for an asynchronous timer
 * as in the documentation for asio, but updated to use C++11
 * constructs instead of boost.
 */
class CountDown
{
public:
    CountDown(asio::io_service &io, int startvalue, std::string msg) :
        timer_{io, std::chrono::seconds{1}},
        count_{startvalue},
        message_{msg}
    {
        timer_.async_wait(std::bind(&CountDown::print, this));
    }
    ~CountDown() {
        std::cout << message_ << "\n";
    }
    void print() {
        if (count_) {
            std::cout << count_ << std::endl;
            --count_;
            timer_.expires_from_now(std::chrono::seconds{1});
            timer_.async_wait(std::bind(&CountDown::print, this));
        }
    }
    
private:
    asio::steady_timer timer_;
    int count_;
    std::string message_;
};

// simple test program
int main(int argc, char *argv[])
{
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " countdownseconds message\n";
        return 1;
    }
    asio::io_service io;
    CountDown cd(io, std::atoi(argv[1]), argv[2]);
    io.run();
}
