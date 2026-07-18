#pragma once

#define LIS_REMASTERED_EXE_NAME "LiS-Win64-Shipping.exe"
#define LIS_BTS_REMASTERED_EXE_NAME "Life is Strange - Before the Storm.exe"

namespace Cosmoem::Utils {
    class GameUtils {
    public:
        enum GameType {
            LIS_REMASTERED,
            LIS_BTS_REMASTERED
        };

        static const char* gameTypeToString(GameType type);
    };
}
