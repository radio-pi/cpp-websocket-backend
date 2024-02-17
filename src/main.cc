#include <thread>
#include <future>
#include <iostream>

#include <vlcpp/vlc.hpp>

#include <crow.h>

void play_stream(std::stop_token stoken, std::string stream_url)
{
    CROW_LOG_INFO << "mediaplayer: start for " << stream_url;
    auto instance = VLC::Instance(0, nullptr);

    // Note: get all logs from VLC for debugging
    // instance.logSet([](int lvl, const libvlc_log_t*, std::string message ) {
    //    std::cout << "Hooked VLC log: " << lvl << ' ' << message << std::endl;
    //    });

    auto media = VLC::Media(instance, stream_url, VLC::Media::FromLocation);
    auto mediaplayer = VLC::MediaPlayer(media);

    std::promise<void> stop;
    std::stop_callback callBack(stoken, [&mediaplayer, &stop] {
	CROW_LOG_INFO << "mediaplayer: stop";
        mediaplayer.stop();
	stop.set_value();
    });

    mediaplayer.play();

    // wait until cancled
    stop.get_future().wait();
}

int main(int /*ac*/, char** /*av*/)
{
    CROW_LOG_INFO << "Playing some music";
    std::jthread current;

    crow::SimpleApp app;

    CROW_ROUTE(app, "/play")
      .methods(crow::HTTPMethod::Post)([&current](const crow::request& req) {
        auto data = crow::json::load(req.body);
        if (!data || !data.has("url")) 
	{ 
	   CROW_LOG_WARNING << "/play could not parse the json request, which should include a 'url' key";
	   return crow::response(400); 
	}
        
	std::string stream_url = data["url"].s();
	CROW_LOG_INFO << "/play with for: " << stream_url;
        std::jthread play{play_stream, stream_url};
        current.swap(play);

        return crow::response{"{}"};
    });

    CROW_ROUTE(app, "/stop")([&current](){
        current.request_stop();
        return "{}";
    });

    app.loglevel(crow::LogLevel::Info);
    app.port(8000).multithreaded().run();
}
