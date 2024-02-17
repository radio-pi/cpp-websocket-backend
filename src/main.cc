#include <future>
#include <iostream>
#include <thread>

#include <vlcpp/vlc.hpp>

#include <crow.h>

struct Player {
  Player() : _vlc(VLC::Instance(0, nullptr)){};
  void play(std::string stream_url) {
    auto media = VLC::Media(_vlc, stream_url, VLC::Media::FromLocation);
    _mediaplayer = VLC::MediaPlayer(media);
    _mediaplayer.play();
  }
  void stop() { _mediaplayer.stop(); }
  int volume() {
    if (_mediaplayer) {
      return _mediaplayer.volume();
    }
    return 0;
  }
  void volume(int v) {
    if (_mediaplayer) {
      _mediaplayer.setVolume(v);
    }
  }
  VLC::Instance _vlc;
  VLC::MediaPlayer _mediaplayer;
};

void play_stream(std::stop_token stoken, Player &player,
                 std::string stream_url) {
  CROW_LOG_INFO << "mediaplayer: start for " << stream_url;

  std::promise<void> stop;
  std::stop_callback callBack(stoken, [&player, &stop] {
    CROW_LOG_INFO << "mediaplayer: stop";
    player.stop();
    stop.set_value();
  });

  player.play(stream_url);

  // wait until cancled
  stop.get_future().wait();
}

int main(int /*ac*/, char ** /*av*/) {
  CROW_LOG_INFO << "Playing some music";
  Player audio_player{};
  std::jthread current;

  crow::SimpleApp app;

  CROW_ROUTE(app, "/play")
      .methods(crow::HTTPMethod::Post)(
          [&current, &audio_player](const crow::request &req) {
            auto data = crow::json::load(req.body);
            if (!data || !data.has("url")) {
              CROW_LOG_WARNING << "/play could not parse the json request, "
                                  "which should include a 'url' key";
              return crow::response(400);
            }

            std::string stream_url = data["url"].s();
            CROW_LOG_INFO << "/play with for: " << stream_url;
            std::jthread play{play_stream, std::ref(audio_player), stream_url};
            current.swap(play);

            return crow::response{"{}"};
          });

  CROW_ROUTE(app, "/stop")
  ([&current]() {
    current.request_stop();
    return "{}";
  });

  CROW_ROUTE(app, "/volume")
  ([&audio_player]() {
    return std::format("volume: {}", audio_player.volume());
  });

  CROW_ROUTE(app, "/volume")
      .methods(crow::HTTPMethod::Post)(
          [&audio_player](const crow::request &req) {
            auto data = crow::json::load(req.body);
            if (!data || !data.has("volume")) {
              CROW_LOG_WARNING << "/volume could not parse the json request, "
                                  "which should include a 'volume' key";
              return crow::response(400);
            }

            int volume = data["volume"].i();
            audio_player.volume(volume);
            return crow::response{"{}"};
          });

  app.loglevel(crow::LogLevel::Info);
  app.port(8000).multithreaded().run();
}
