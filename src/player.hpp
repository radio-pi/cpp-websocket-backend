#pragma once

#include <string>

#include <vlcpp/vlc.hpp>

struct Player {
  Player() : _vlc(VLC::Instance(0, nullptr)){};

  void play(std::string stream_url);
  void stop();

  int volume();
  void volume(int v);

private:
  VLC::Instance _vlc;
  VLC::MediaPlayer _mediaplayer;
};
