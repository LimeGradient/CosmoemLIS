#include "utils/GameUtils.hpp"

namespace Cosmoem::Utils {
    const char* GameUtils::gameTypeToString(GameType type) {
        switch (type) {
            case LIS_REMASTERED:
                return LIS_REMASTERED_EXE_NAME;
            case LIS_BTS_REMASTERED:
                return LIS_BTS_REMASTERED_EXE_NAME;
        }
    }
}