#include <future>
#include <iostream>
#include <mutex>
#include <thread>
#include <unordered_set>

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

  std::mutex mtx;
  std::unordered_set<crow::websocket::connection *> users;

  CROW_WEBSOCKET_ROUTE(app, "/ws")
      .onopen([&](crow::websocket::connection &conn) {
        CROW_LOG_INFO << "new websocket connection from "
                      << conn.get_remote_ip();
        std::lock_guard<std::mutex> lock(mtx);
        users.insert(&conn);
      })
      .onclose(
          [&](crow::websocket::connection &conn, const std::string &reason) {
            CROW_LOG_INFO << "websocket connection closed: " << reason;
            std::lock_guard<std::mutex> lock(mtx);
            users.erase(&conn);
          })
      .onmessage([&](crow::websocket::connection & /*conn*/,
                     const std::string &data, bool is_binary) {
        CROW_LOG_INFO << "websocket got message: Ignoring!";
      });

  CROW_ROUTE(app, "/play")
      .methods(crow::HTTPMethod::Post)([&current, &audio_player](
                                           const crow::request &req) {
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

        return crow::response{crow::json::wvalue{crow::json::wvalue::object{}}};
      });

  CROW_ROUTE(app, "/stop").methods(crow::HTTPMethod::Post)([&current]() {
    current.request_stop();
    return crow::response{crow::json::wvalue{crow::json::wvalue::object{}}};
  });

  CROW_ROUTE(app, "/volume")
  ([&audio_player]() {
    return crow::json::wvalue{{"volume", audio_player.volume()}};
  });

  CROW_ROUTE(app, "/volume")
      .methods(crow::HTTPMethod::Post)([&audio_player, &mtx,
                                        &users](const crow::request &req) {
        auto data = crow::json::load(req.body);
        if (!data || !data.has("volume")) {
          CROW_LOG_WARNING << "/volume could not parse the json request, "
                              "which should include a 'volume' key";
          return crow::response(400);
        }

        int volume = data["volume"].i();
        audio_player.volume(volume);

        crow::json::wvalue volume_json{{"volume", volume}};

        std::lock_guard<std::mutex> lock(mtx);
        for (auto u : users) {
          u->send_text(volume_json.dump());
        }

        return crow::response{crow::json::wvalue{crow::json::wvalue::object{}}};
      });

  CROW_ROUTE(app, "/streamurls")
  ([]() {
    crow::json::wvalue::list stream_list_map(
        {crow::json::wvalue::object{{
             {"name", "Hardbase"},
             {"url", "http://listen.hardbase.fm/tunein-mp3-pls"},
             {"img", "/image/Hardbasefm.jpg"},
             {"orderid", 1},
         }},
         crow::json::wvalue::object{{
             {"name", "Technobase"},
             {"url", "http://listen.technobase.fm/tunein-mp3-asx"},
             {"img", ""},
             {"orderid", 2},
         }},
         crow::json::wvalue::object{{
             {"name", "Radio 24"},
             {"url", "http://icecast.radio24.ch/radio24"},
             {"img", "https://upload.wikimedia.org/wikipedia/de/thumb/3/33/"
                     "Radio_24_Logo.svg/154px-Radio_24_Logo.svg.png"},
             {"orderid", 0},
         }},
         crow::json::wvalue::object{{
             {"name", "Radio SRF 1"},
             {"url", "http://stream.srg-ssr.ch/m/drs1/mp3_128"},
             {"img", "https://www.srf.ch/play/v3/svgs/radio-srf-1-small.svg"},
             {"orderid", 3},
         }},
         crow::json::wvalue::object{{
             {"name", "Radio SRF 2"},
             {"url", "http://stream.srg-ssr.ch/m/drs2/mp3_128"},
             {"img",
              "https://www.srf.ch/play/v3/svgs/radio-srf-2-kultur-small.svg"},
             {"orderid", 4},
         }},
         crow::json::wvalue::object{{
             {"name", "Radio SRF 3"},
             {"url", "http://stream.srg-ssr.ch/m/drs3/mp3_128"},
             {"img", "https://www.srf.ch//play/v3/svgs/radio-srf-3-small.svg"},
             {"orderid", 5},
         }},
         crow::json::wvalue::object{{
             {"name", "Radio Swiss Jazz"},
             {"url", "http://stream.srg-ssr.ch/m/rsj/mp3_128"},
             {"img", ""},
             {"orderid", 6},
         }},
         crow::json::wvalue::object{{
             {"name", "Radio Swiss Pop"},
             {"url", "http://stream.srg-ssr.ch/m/rsp/mp3_128"},
             {"img", ""},
             {"orderid", 7},
         }}});

    crow::json::wvalue stream_list{stream_list_map};
    return crow::response{stream_list};
  });

  app.loglevel(crow::LogLevel::Info);
  app.port(8000).multithreaded().run();
}
