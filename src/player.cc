#include "player.hpp"

void Player::play(std::string stream_url) {
  auto media = VLC::Media(_vlc, stream_url, VLC::Media::FromLocation);
  _mediaplayer = VLC::MediaPlayer(media);
  _mediaplayer.play();
}

void Player::stop() { _mediaplayer.stop(); }

int Player::volume() {
  if (_mediaplayer) {
    return _mediaplayer.volume();
  }
  return 0;
}

void Player::volume(int v) {
  if (_mediaplayer) {
    _mediaplayer.setVolume(v);
  }
}
