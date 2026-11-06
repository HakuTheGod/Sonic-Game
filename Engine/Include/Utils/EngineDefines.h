#pragma once

#include "SDL.h"

#include <cstdint>
#include <map>


using BitmapSurface = SDL_Surface;
using BitmapTexture = SDL_Texture;

using Index = std::uint16_t;
using Dim = std::uint16_t;

using byte = std::uint8_t;
using GridIndex = byte;

using KEY_MAP_t = std::map<int, bool>;


struct Engine_Consts_t {

    Dim Tile_width{};
    Dim Tile_height{};
    Dim power{};

    Dim Map_height{};
    Dim Map_width{};

    Dim Grid_el_sz{};
    Dim grid_power{};

    Dim map_rows{};
    Dim map_cols{};

    Engine_Consts_t() = default;

    Engine_Consts_t(
        Dim tw,
        Dim th,
        Dim pow,
        Dim mh,
        Dim mw,
        Dim el_sz,
        Dim map_r,
        Dim map_c,
        Dim grid_pow
    )
        : Tile_width(tw),
          Tile_height(th),
          power(pow),
          Map_height(mh),
          Map_width(mw),
          Grid_el_sz(el_sz),
          grid_power(grid_pow),
          map_rows(map_r),
          map_cols(map_c)
    {}
};

extern Engine_Consts_t Engine_Consts;


inline constexpr Dim MAPHEIGHT = 16;
inline constexpr Dim MAPWIDTH = 300;

inline constexpr Dim TILE_WIDTH = 16;
inline constexpr Dim TILE_HEIGHT = 16;


inline constexpr std::uint16_t TILE_X_MASK = 0xFF00;
inline constexpr unsigned int TILE_X_SHIFT = 8;
inline constexpr std::uint16_t TILE_Y_MASK = 0x00FF;



constexpr unsigned int MulTileWidth(unsigned int i, unsigned int power) noexcept
{
    return i << power;
}

constexpr unsigned int MulTileHeight(unsigned int i, unsigned int power) noexcept
{
    return i << power;
}

constexpr unsigned int DivTileWidth(unsigned int i, unsigned int power) noexcept
{
    return i >> power;
}

constexpr unsigned int DivTileHeight(unsigned int i, unsigned int power) noexcept
{
    return i >> power;
}

constexpr unsigned int ModTileWidth(unsigned int i, unsigned int tw)
{
    return i % tw;
}

constexpr unsigned int ModTileHeight(unsigned int i, unsigned int th)
{
    return i % th;
}