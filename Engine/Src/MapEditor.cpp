#include <fstream>
#include <iostream>
#include <utility>
#include <sstream>
#include "../Include/MapEditor.h"
#include "../Include/Utils/EngineDefines.h"

std::vector<TileMap> MapLayers;

void SetTile (TileMap* m, Dim col, Dim row, Index index)
    { (*m)[row][col] = index; }

Index GetTile (const TileMap* m, Dim col, Dim row)
    { return (*m)[row][col]; }

void WriteBinMap (const TileMap* m, FILE* fp)
    { fwrite(m, sizeof(TileMap), 1, fp); }

bool ReadBinMap(TileMap* m, FILE* fp) { return true; } 



bool ReadTextMap(TileMap* m, FILE* fp)
{
    if (m == nullptr || fp == nullptr)
        return false;

    TileMap newMap;
    char buffer[8192];

    while (fgets(buffer, sizeof(buffer), fp)) {

        std::vector<Index> row;
        std::stringstream str(buffer);
        std::string word;

        while (std::getline(str, word, ',')) {

            try {
                size_t pos = 0;
                int value = std::stoi(word, &pos);

                if (pos != word.size() ||
                    value < 0 ||
                    value > std::numeric_limits<Index>::max()) {
                    return false;
                }

                row.push_back(static_cast<Index>(value));
            }
            catch (const std::exception&) {
                return false;
            }
        }

        if (!newMap.empty() && row.size() != newMap[0].size())
            return false;

        newMap.push_back(std::move(row));
    }

    if (newMap.empty() || ferror(fp))
        return false;

    *m = std::move(newMap);

    return true;
}

TileMap* GetMapData(size_t layer) {

    if (layer >= MapLayers.size())
        return nullptr;

    return &MapLayers[layer];
}

//we should try to separate the og code first.