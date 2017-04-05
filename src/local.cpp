#include "utils/Logger.h"
#include "server/sockv5/Setting.h"
#include "server/sockv5/Sockv5Proxy.h"
#include "utils/RandomUtils.h"
#include <boost/program_options.hpp>
#include "codec/CodecFacade.h"
#include "frame/FrameFacade.h"

#ifdef _WIN32
#pragma comment(lib, "libprotobufd.lib")
#pragma comment(lib, "libprotocd.lib")
#endif

Setting g_setting;
using namespace std;

void initLogger(Setting &setting)
{
    initLog((LOG_LV) setting.logLv);
}

void initCodec(Setting &setting)
{
    auto &codecFacade = CodecFacade::get_mutable_instance();
    codecFacade.init(setting.key);
    SF_LOG(LOG_INFO, "All codec:%s", codecFacade.toString().c_str());
    bool ret = codecFacade.addCodec(setting.encryptMethod);
    if (!ret)
    {
        SF_LOG(LOG_ERROR, "Codec init failed, not found codec:%s, pass 'none' if you don't need codec!", setting.encryptMethod.c_str());
        exit(1);
    }
    else
    {
        SF_LOG(LOG_INFO, "Codec init success, current codec:%s!", setting.encryptMethod.c_str());
    }
}

void initFrame(Setting &setting)
{
    auto &frameFacade = FrameFacade::get_mutable_instance();
    frameFacade.init();
    SF_LOG(LOG_INFO, "All frames:%s", frameFacade.toString().c_str());
    bool ret = frameFacade.addFrame(setting.frame);
    if(!ret)
    {
        SF_LOG(LOG_ERROR, "Frame init failed, not found frame:%s", setting.frame.c_str());
        exit(2);
    }
    else
    {
        SF_LOG(LOG_INFO, "Header init success, current header:%s", setting.frame.c_str());
    }
}

void initSetting(int argc, const char *argv[], Setting &setting)
{
    boost::program_options::options_description opts("Server options");
    opts.add_options()
            ("setting,s", boost::program_options::value<std::string>(), "setting file path")
            ("method,m", boost::program_options::value<std::string>(), "encrypt method")
            ("server_addr,s", boost::program_options::value<std::string>(), "server bind addr and port, example: 127.0.0.1:7000")
            ("local_addr,l", boost::program_options::value<std::string>(), "local bind addr and port, example: 127.0.0.1:8000")
            ("key,k", boost::program_options::value<std::string>(), "encrypt key")
            ("user,u", boost::program_options::value<std::string>(), "user")
            ("password,p", boost::program_options::value<std::string>(), "password")
            ("log_level,v", boost::program_options::value<int>(), "log level")
            ("frame,f", boost::program_options::value<std::string>(), "msg frame")
            ("thread,t", boost::program_options::value<int>(), "thread count"); //多个参数

    boost::program_options::variables_map vm;
    try
    {
        boost::program_options::store(parse_command_line(argc, argv, opts), vm); // 分析参数
    }
    catch(boost::program_options::error_with_no_option_name &ex)
    {
        std::cout << "Parse cmd failed, err:" << ex.what() << std::endl;
        exit(3);
    }
    boost::program_options::notify(vm);
    if(vm.count("setting"))
    {
        setting.loadConfig(vm["setting"].as<std::string>());
        SF_LOG(LOG_INFO, "Read setting file, skip other cmd args..");
        return;
    }
    //
    if(vm.count("method"))
        setting.encryptMethod = vm["method"].as<std::string>();
    else
    {
        SF_LOG(LOG_ERROR, "Mot found encrypt method, use 'none' as default");
        setting.encryptMethod = "none";
    }
    if(vm.count("server_addr"))
    {
        std::string server = vm["server_addr"].as<std::string>();
        uint32_t pos = server.find(":");
        if(pos == std::string::npos)
        {
            SF_LOG(LOG_ERROR, "Read invalid server address, string:%s", server.c_str());
            exit(3);
        }
        setting.serverBindAddr = server.substr(0, pos);
        setting.serverListenPort = boost::lexical_cast<uint32_t>(server.substr(pos + 1));
    }
    else
    {
        SF_LOG(LOG_ERROR, "Not found server address, use '0.0.0.0:8000' as default!");
        setting.serverBindAddr = "0.0.0.0";
        setting.serverListenPort = 8000;
    }
    if(vm.count("local_addr"))
    {
        std::string local = vm["local_addr"].as<std::string>();
        uint32_t pos = local.find(":");
        if(pos == std::string::npos)
        {
            SF_LOG(LOG_ERROR, "Read invalid server address, string:%s", local.c_str());
            exit(3);
        }
        setting.serverBindAddr = local.substr(0, pos);
        setting.serverListenPort = boost::lexical_cast<uint32_t>(local.substr(pos + 1));
    }
    else
    {
        SF_LOG(LOG_ERROR, "Not found local address, use '127.0.0.1:8000' as default!");
        setting.localBindAddr = "127.0.0.1";
        setting.localListenPort = 8000;
    }
    if(vm.count("key"))
        setting.key = vm["key"].as<std::string>();
    else
    {
        setting.key = RandomUtils::randStringVisible(8);
        SF_LOG(LOG_ERROR, "Not found key, use '%s' as default!", setting.key.c_str());
    }
    if(vm.count("user"))
        setting.user = vm["user"].as<std::string>();
    else
    {
        SF_LOG(LOG_ERROR, "Not found user name, use 'test' as default!");
        setting.user = "test";
    }
    if(vm.count("password"))
        setting.password = vm["password"].as<std::string>();
    else
    {
        SF_LOG(LOG_ERROR, "Not found password, use 'test' as default!");
        setting.password = "test";
    }
    if(vm.count("log_level"))
    {
        setting.logLv = vm["log_level"].as<int>();
    }
    else
    {
        SF_LOG(LOG_ERROR, "Not found log level, use LOG_TRACE as default!");
        setting.logLv = LOG_TRACE;
    }
    if(vm.count("frame"))
        setting.frame = vm["frame"].as<std::string>();
    else
    {
        SF_LOG(LOG_ERROR, "Not found frame, use 'fix' as default");
        setting.frame = "fix";
    }
    if(vm.count("thread"))
        setting.threadCount = (uint32_t) vm["thread"].as<int>();
    else
    {
        SF_LOG(LOG_ERROR, "Not found thread count, use '4' as default");
        setting.threadCount = 4;
    }
}

void init(int argc, const char *argv[], Setting &setting)
{
    RandomUtils::setSeed((unsigned int) time(NULL));
    initSetting(argc, argv, setting);
    SF_LOG(LOG_INFO, "Setting init success, setting:%s", setting.toString().c_str());
    initLogger(setting);
    initCodec(setting);
    initFrame(setting);
}

int main(int argc, const char *argv[])
{
    init(argc, argv, g_setting);
	Sockv5Proxy proxy(g_setting.localBindAddr, (uint16_t) g_setting.localListenPort);
	proxy.start();

    return 0;
}