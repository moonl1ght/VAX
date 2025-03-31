//
// Created by Alexander Lakhonin on 28.03.2025.
//

#include "Texture.hpp"

#include <utility>

using namespace vax;

Texture::~Texture() {
  _texture->release();
}

Texture::Texture(const Texture& rhs): _texture(rhs._texture) {
}

Texture::Texture(Texture&& rhs) {
  std::swap(rhs._texture, _texture);
}

Texture& Texture::operator=(Texture& rhs) {
  if (&rhs != this) {
    Texture temp(rhs);
    std::swap(*this, temp);
  }
  return *this;
}

Texture& Texture::operator=(Texture&& rhs) {
  Texture temp(std::move(rhs));
  std::swap(*this, temp);
  return *this;
}

MTL::Texture& Texture::texture() const noexcept {
  return *_texture;
}
