#ifndef __PROGRAM_HPP__
#define __PROGRAM_HPP__
#include <iostream>
#include<string>
#include <fstream>  
#include <map> 
#include<vector>
using namespace std;
#include <boost/program_options.hpp>  
namespace po = boost::program_options;
using namespace boost;
using std::vector;
/*
    <CMakeLists.txt>
    project (test)

    SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11")
    find_package(Boost REQUIRED COMPONENTS
    # regex
    program_options   # 我的工程中只使用了 program_options 功能,因此这里只有一个组件
    )
    if(NOT Boost_FOUND)
        message("Not found Boost")
    endif()

    include_directories(${Boost_INCLUDE_DIRS})
    message("${Boost_INCLUDE_DIRS}")
    message("${Boost_LIBRARIES}")

    add_executable (test main.cpp net.cpp myqueue.cpp node.cpp  
    mythread.cpp mutexlock.cpp objectPool.cpp program.cpp)

    TARGET_LINK_LIBRARIES(test pthread miniupnpc ${Boost_LIBRARIES})
*/
/*
    https://www.boost.org/doc/libs/1_59_0/doc/html/program_options/tutorial.html
    https://github.com/yejiangnan123/c-/tree/master/boost
      ./test --cpu=5 --listenPort=1234

    yejn@ubuntu:/mnt/hgfs/work/p2p_tcp$ cat config.cfg 
    cpu=5
    listenPort=5555
    listenNumber=20
    connectIp=192.168.1.45
    connectPort=6666
    thread1Num=10
    thread2Num=11
    thread3Num=12
*/
class program
{
private:
public:
    int main(int ac, char*av[])
    {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "produce help message\n"
                    "1\n"
                    "2"
            )
            ("cpu", po::value<int>(&cpu)->default_value(4), "CPU number")
         ;
        
        po::options_description network("network options");
        network.add_options()
            ("listenPort",po::value<unsigned short>(&listenPort)->default_value(4),"listen Port")
            ("listenNumber",po::value<unsigned short>(&listenNumber)->default_value(4),"listen number")
            ("connectIp",po::value<string>(&connectIp)->default_value("127.0.0.1"),"connectIp")
            ("connectPort",po::value<unsigned short>(&connectPort)->default_value(4),"connectPort")
            ;

        po::options_description threadOption("thread options");
        threadOption.add_options()
            ("thread1Num",po::value<int>(&thread1Num)->default_value(4),"thread1Num")
            ("thread2Num",po::value<int>(&thread2Num)->default_value(4),"thread2Num")
            ("thread3Num",po::value<int>(&thread3Num)->default_value(4),"thread3Num")
            ;
        
        
        po::options_description cmdline_options;
        cmdline_options.add(desc).add(network).add(threadOption);

        po::options_description config_file_options;
        config_file_options.add(desc).add(network).add(threadOption);

        po::variables_map vm;
        po::store(po::parse_command_line(ac, av, cmdline_options), vm);
        std::ifstream settings_file("config.cfg");
        po::store(po::parse_config_file(settings_file, config_file_options), vm);
        po::notify(vm);
        if (vm.count("help")) 
        {
            cout << cmdline_options << "\n";
        }
        #if 0
        if (vm.count("cpu"))
        {
            cpu = vm["cpu"].as<int>();
        }
        if (vm.count("listenPort"))
        {
            listenPort = vm["listenPort"].as<unsigned short>();
        }
        if (vm.count("listenNumber"))
        {
            listenNumber = vm["listenNumber"].as<unsigned short>();
        }
        if (vm.count("connectIp"))
        {
            connectIp = vm["connectIp"].as<string>();
        }
        if (vm.count("connectPort"))
        {
            connectPort = vm["connectPort"].as<unsigned short>();
        }
        if (vm.count("thread1Num"))
        {
            thread1Num = vm["thread1Num"].as<int>();
        }
        if (vm.count("thread2Num"))
        {
            thread2Num = vm["thread2Num"].as<int>();
        }
        if (vm.count("thread3Num"))
        {
            thread3Num = vm["thread3Num"].as<int>();
        }
        #endif
        print();
    }
    void print()
    {
        std::cout<<"cpu="<<cpu<<endl;
        std::cout<<"listenPort="<<listenPort<<endl;
        std::cout<<"listenNumber="<<listenNumber<<endl;
        std::cout<<"connectIp="<<connectIp<<endl;
        std::cout<<"connectPort="<<connectPort<<endl;
        std::cout<<"thread1Num="<<thread1Num<<endl;
        std::cout<<"thread2Num="<<thread2Num<<endl;
        std::cout<<"thread3Num="<<thread3Num<<endl;
    }
    program()
    {
        cpu = 4;
        listenPort = 0;
        listenNumber = 10;
        connectIp = "";
        connectPort = 0;
        thread1Num = 4;
        thread2Num = 4;
        thread3Num = 4;
    }
    /*
        --cpu=5 --listenPort=5555 --listenNumber=20 --connectIp=192.168.1.45 --connectPort=6666 --thread1Num=10 --thread2Num=11 --thread3Num=12
    */
    int cpu;
    unsigned short listenPort;
    unsigned short listenNumber;
    string connectIp;
    unsigned short connectPort;
    int thread1Num;
    int thread2Num;
    int thread3Num;    
};
#endif
