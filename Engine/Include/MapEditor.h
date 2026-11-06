#pragma once
#include <string>
#include <vector>
#include <cstddef>
#include <cstdio>
#include <limits>
#include <stdexcept>
typedef unsigned short Index;
typedef unsigned short Dim;
typedef std::vector<std::vector<Index>> TileMap;

extern std::vector<TileMap> MapLayers;

void SetTile (TileMap* m, Dim col, Dim row, Index index);
Index GetTile (const TileMap* m, Dim col, Dim row);

void WriteBinMap (const TileMap* m, FILE* fp);
bool ReadBinMap(TileMap* m, FILE* fp);

void WriteTextMap(const TileMap*, FILE* fp);
bool ReadTextMap(TileMap* m, FILE* fp);

TileMap* GetMapData(size_t layer);