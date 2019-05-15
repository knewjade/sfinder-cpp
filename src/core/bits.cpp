#include "bits.hpp"

namespace core {
    Bitboard deleteLine_(Bitboard x, LineKey key) {
        switch (key) {
            case 3072:
                return (x & 1023ULL) | ((x >> 10) & 1073740800ULL) | ((x >> 20) & 1098437885952ULL);
            case 2048:
                return (x & 1099511627775ULL) | ((x >> 10) & 1124800395214848ULL);
            case 1024:
                return (x & 1023ULL) | ((x >> 10) & 1125899906841600ULL);
            case 0:
                return (x);
            case 3073:
                return ((x >> 20) & 1048575ULL) | ((x >> 30) & 1072693248ULL);
            case 2049:
                return ((x >> 10) & 1073741823ULL) | ((x >> 20) & 1098437885952ULL);
            case 1025:
                return ((x >> 20) & 1099511627775ULL);
            case 1:
                return ((x >> 10) & 1125899906842623ULL);
            case 3074:
                return (x & 1023ULL) | ((x >> 10) & 1047552ULL) | ((x >> 30) & 1072693248ULL);
            case 2050:
                return (x & 1073741823ULL) | ((x >> 20) & 1098437885952ULL);
            case 1026:
                return (x & 1023ULL) | ((x >> 10) & 1047552ULL) | ((x >> 20) & 1099510579200ULL);
            case 2:
                return (x & 1073741823ULL) | ((x >> 10) & 1125898833100800ULL);
            case 3075:
                return ((x >> 20) & 1023ULL) | ((x >> 40) & 1047552ULL);
            case 2051:
                return ((x >> 10) & 1048575ULL) | ((x >> 30) & 1072693248ULL);
            case 1027:
                return ((x >> 20) & 1023ULL) | ((x >> 30) & 1073740800ULL);
            case 3:
                return ((x >> 10) & 1048575ULL) | ((x >> 20) & 1099510579200ULL);
            case 1051648:
                return (x & 1023ULL) | ((x >> 20) & 1047552ULL) | ((x >> 30) & 1072693248ULL);
            case 1050624:
                return (x & 1048575ULL) | ((x >> 10) & 1072693248ULL) | ((x >> 20) & 1098437885952ULL);
            case 1049600:
                return (x & 1023ULL) | ((x >> 20) & 1099511626752ULL);
            case 1048576:
                return (x & 1048575ULL) | ((x >> 10) & 1125899905794048ULL);
            case 1051649:
                return ((x >> 30) & 1023ULL) | ((x >> 40) & 1047552ULL);
            case 1050625:
                return ((x >> 10) & 1023ULL) | ((x >> 20) & 1047552ULL) | ((x >> 30) & 1072693248ULL);
            case 1049601:
                return ((x >> 30) & 1073741823ULL);
            case 1048577:
                return ((x >> 10) & 1023ULL) | ((x >> 20) & 1099511626752ULL);
            case 1051650:
                return (x & 1023ULL) | ((x >> 40) & 1047552ULL);
            case 1050626:
                return (x & 1048575ULL) | ((x >> 30) & 1072693248ULL);
            case 1049602:
                return (x & 1023ULL) | ((x >> 30) & 1073740800ULL);
            case 1048578:
                return (x & 1048575ULL) | ((x >> 20) & 1099510579200ULL);
            case 1051651:
                return ((x >> 50) & 1023ULL);
            case 1050627:
                return ((x >> 10) & 1023ULL) | ((x >> 40) & 1047552ULL);
            case 1049603:
                return ((x >> 40) & 1048575ULL);
            case 1048579:
                return ((x >> 10) & 1023ULL) | ((x >> 30) & 1073740800ULL);
            case 2100224:
                return (x & 1023ULL) | ((x >> 10) & 1073740800ULL);
            case 2099200:
                return (x & 1099511627775ULL);
            case 2098176:
                return (x & 1023ULL) | ((x >> 10) & 1099511626752ULL);
            case 2097152:
                return (x & 1125899906842623ULL);
            case 2100225:
                return ((x >> 20) & 1048575ULL);
            case 2099201:
                return ((x >> 10) & 1073741823ULL);
            case 2098177:
                return ((x >> 20) & 1073741823ULL);
            case 2097153:
                return ((x >> 10) & 1099511627775ULL);
            case 2100226:
                return (x & 1023ULL) | ((x >> 10) & 1047552ULL);
            case 2099202:
                return (x & 1073741823ULL);
            case 2098178:
                return (x & 1023ULL) | ((x >> 10) & 1047552ULL) | ((x >> 20) & 1072693248ULL);
            case 2097154:
                return (x & 1073741823ULL) | ((x >> 10) & 1098437885952ULL);
            case 2100227:
                return ((x >> 20) & 1023ULL);
            case 2099203:
                return ((x >> 10) & 1048575ULL);
            case 2098179:
                return ((x >> 20) & 1023ULL) | ((x >> 30) & 1047552ULL);
            case 2097155:
                return ((x >> 10) & 1048575ULL) | ((x >> 20) & 1072693248ULL);
            case 3148800:
                return (x & 1023ULL) | ((x >> 20) & 1047552ULL);
            case 3147776:
                return (x & 1048575ULL) | ((x >> 10) & 1072693248ULL);
            case 3146752:
                return (x & 1023ULL) | ((x >> 20) & 1073740800ULL);
            case 3145728:
                return (x & 1048575ULL) | ((x >> 10) & 1099510579200ULL);
            case 3148801:
                return ((x >> 30) & 1023ULL);
            case 3147777:
                return ((x >> 10) & 1023ULL) | ((x >> 20) & 1047552ULL);
            case 3146753:
                return ((x >> 30) & 1048575ULL);
            case 3145729:
                return ((x >> 10) & 1023ULL) | ((x >> 20) & 1073740800ULL);
            case 3148802:
                return (x & 1023ULL);
            case 3147778:
                return (x & 1048575ULL);
            case 3146754:
                return (x & 1023ULL) | ((x >> 30) & 1047552ULL);
            case 3145730:
                return (x & 1048575ULL) | ((x >> 20) & 1072693248ULL);
            case 3148803:
                return (0ULL);
            case 3147779:
                return ((x >> 10) & 1023ULL);
            case 3146755:
                return ((x >> 40) & 1023ULL);
            case 3145731:
                return ((x >> 10) & 1023ULL) | ((x >> 30) & 1047552ULL);
            default:
                assert(false);
                return x;
        }
    }

    Bitboard deleteLine(Bitboard x, LineKey mask) {
        // 1073741823 = (1 << 30) - 1
        LineKey key = (mask >> 29) | (mask & 1073741823ULL);
        return deleteLine_(x, key);
    }

    Bitboard insertBlackLine_(Bitboard x, LineKey key) {
        switch (key) {
            case 3072:
                return (x & 1023ULL) | ((x & 1073740800ULL) << 10) | ((x & 1098437885952ULL) << 20) |
                       (1124800396262400ULL);
            case 2048:
                return (x & 1099511627775ULL) | ((x & 1124800395214848ULL) << 10) | (1124800395214848ULL);
            case 1024:
                return (x & 1023ULL) | ((x & 1125899906841600ULL) << 10) | (1047552ULL);
            case 0:
                return (x);
            case 3073:
                return ((x & 1048575ULL) << 20) | ((x & 1072693248ULL) << 30) | (1124800396263423ULL);
            case 2049:
                return ((x & 1073741823ULL) << 10) | ((x & 1098437885952ULL) << 20) | (1124800395215871ULL);
            case 1025:
                return ((x & 1099511627775ULL) << 20) | (1048575ULL);
            case 1:
                return ((x & 1125899906842623ULL) << 10) | (1023ULL);
            case 3074:
                return (x & 1023ULL) | ((x & 1047552ULL) << 10) | ((x & 1072693248ULL) << 30) | (1125898834148352ULL);
            case 2050:
                return (x & 1073741823ULL) | ((x & 1098437885952ULL) << 20) | (1125898833100800ULL);
            case 1026:
                return (x & 1023ULL) | ((x & 1047552ULL) << 10) | ((x & 1099510579200ULL) << 20) | (1098438933504ULL);
            case 2:
                return (x & 1073741823ULL) | ((x & 1125898833100800ULL) << 10) | (1098437885952ULL);
            case 3075:
                return ((x & 1023ULL) << 20) | ((x & 1047552ULL) << 40) | (1125898834149375ULL);
            case 2051:
                return ((x & 1048575ULL) << 10) | ((x & 1072693248ULL) << 30) | (1125898833101823ULL);
            case 1027:
                return ((x & 1023ULL) << 20) | ((x & 1073740800ULL) << 30) | (1098438934527ULL);
            case 3:
                return ((x & 1048575ULL) << 10) | ((x & 1099510579200ULL) << 20) | (1098437886975ULL);
            case 1051648:
                return (x & 1023ULL) | ((x & 1047552ULL) << 20) | ((x & 1072693248ULL) << 30) | (1124801468955648ULL);
            case 1050624:
                return (x & 1048575ULL) | ((x & 1072693248ULL) << 10) | ((x & 1098437885952ULL) << 20) |
                       (1124801467908096ULL);
            case 1049600:
                return (x & 1023ULL) | ((x & 1099511626752ULL) << 20) | (1073740800ULL);
            case 1048576:
                return (x & 1048575ULL) | ((x & 1125899905794048ULL) << 10) | (1072693248ULL);
            case 1051649:
                return ((x & 1023ULL) << 30) | ((x & 1047552ULL) << 40) | (1124801468956671ULL);
            case 1050625:
                return ((x & 1023ULL) << 10) | ((x & 1047552ULL) << 20) | ((x & 1072693248ULL) << 30) |
                       (1124801467909119ULL);
            case 1049601:
                return ((x & 1073741823ULL) << 30) | (1073741823ULL);
            case 1048577:
                return ((x & 1023ULL) << 10) | ((x & 1099511626752ULL) << 20) | (1072694271ULL);
            case 1051650:
                return (x & 1023ULL) | ((x & 1047552ULL) << 40) | (1125899906841600ULL);
            case 1050626:
                return (x & 1048575ULL) | ((x & 1072693248ULL) << 30) | (1125899905794048ULL);
            case 1049602:
                return (x & 1023ULL) | ((x & 1073740800ULL) << 30) | (1099511626752ULL);
            case 1048578:
                return (x & 1048575ULL) | ((x & 1099510579200ULL) << 20) | (1099510579200ULL);
            case 1051651:
                return ((x & 1023ULL) << 50) | (1125899906842623ULL);
            case 1050627:
                return ((x & 1023ULL) << 10) | ((x & 1047552ULL) << 40) | (1125899905795071ULL);
            case 1049603:
                return ((x & 1048575ULL) << 40) | (1099511627775ULL);
            case 1048579:
                return ((x & 1023ULL) << 10) | ((x & 1073740800ULL) << 30) | (1099510580223ULL);
            case 2100224:
                return (x & 1023ULL) | ((x & 1073740800ULL) << 10) | (1152920405096266752ULL);
            case 2099200:
                return (x & 1099511627775ULL) | (1152920405095219200ULL);
            case 2098176:
                return (x & 1023ULL) | ((x & 1099511626752ULL) << 10) | (1151795604701051904ULL);
            case 2097152:
                return (x & 1125899906842623ULL) | (1151795604700004352ULL);
            case 2100225:
                return ((x & 1048575ULL) << 20) | (1152920405096267775ULL);
            case 2099201:
                return ((x & 1073741823ULL) << 10) | (1152920405095220223ULL);
            case 2098177:
                return ((x & 1073741823ULL) << 20) | (1151795604701052927ULL);
            case 2097153:
                return ((x & 1099511627775ULL) << 10) | (1151795604700005375ULL);
            case 2100226:
                return (x & 1023ULL) | ((x & 1047552ULL) << 10) | (1152921503534152704ULL);
            case 2099202:
                return (x & 1073741823ULL) | (1152921503533105152ULL);
            case 2098178:
                return (x & 1023ULL) | ((x & 1047552ULL) << 10) | ((x & 1072693248ULL) << 20) |
                       (1151796703138937856ULL);
            case 2097154:
                return (x & 1073741823ULL) | ((x & 1098437885952ULL) << 10) | (1151796703137890304ULL);
            case 2100227:
                return ((x & 1023ULL) << 20) | (1152921503534153727ULL);
            case 2099203:
                return ((x & 1048575ULL) << 10) | (1152921503533106175ULL);
            case 2098179:
                return ((x & 1023ULL) << 20) | ((x & 1047552ULL) << 30) | (1151796703138938879ULL);
            case 2097155:
                return ((x & 1048575ULL) << 10) | ((x & 1072693248ULL) << 20) | (1151796703137891327ULL);
            case 3148800:
                return (x & 1023ULL) | ((x & 1047552ULL) << 20) | (1152920406168960000ULL);
            case 3147776:
                return (x & 1048575ULL) | ((x & 1072693248ULL) << 10) | (1152920406167912448ULL);
            case 3146752:
                return (x & 1023ULL) | ((x & 1073740800ULL) << 20) | (1151795605773745152ULL);
            case 3145728:
                return (x & 1048575ULL) | ((x & 1099510579200ULL) << 10) | (1151795605772697600ULL);
            case 3148801:
                return ((x & 1023ULL) << 30) | (1152920406168961023ULL);
            case 3147777:
                return ((x & 1023ULL) << 10) | ((x & 1047552ULL) << 20) | (1152920406167913471ULL);
            case 3146753:
                return ((x & 1048575ULL) << 30) | (1151795605773746175ULL);
            case 3145729:
                return ((x & 1023ULL) << 10) | ((x & 1073740800ULL) << 20) | (1151795605772698623ULL);
            case 3148802:
                return (x & 1023ULL) | (1152921504606845952ULL);
            case 3147778:
                return (x & 1048575ULL) | (1152921504605798400ULL);
            case 3146754:
                return (x & 1023ULL) | ((x & 1047552ULL) << 30) | (1151796704211631104ULL);
            case 3145730:
                return (x & 1048575ULL) | ((x & 1072693248ULL) << 20) | (1151796704210583552ULL);
            case 3148803:
                return (1152921504606846975ULL);
            case 3147779:
                return ((x & 1023ULL) << 10) | (1152921504605799423ULL);
            case 3146755:
                return ((x & 1023ULL) << 40) | (1151796704211632127ULL);
            case 3145731:
                return ((x & 1023ULL) << 10) | ((x & 1047552ULL) << 30) | (1151796704210584575ULL);
            default:
                assert(false);
                return x;
        }
    }

    Bitboard insertBlackLine(Bitboard x, LineKey mask) {
        // 1073741823 = (1 << 30) - 1
        LineKey key = (mask >> 29) | (mask & 1073741823ULL);
        return insertBlackLine_(x, key);
    }

    Bitboard insertWhiteLine_(Bitboard x, LineKey key) {
        switch (key) {
            case 3072:
                return (x & 1023ULL) | ((x & 1073740800ULL) << 10) | ((x & 1098437885952ULL) << 20);
            case 2048:
                return (x & 1099511627775ULL) | ((x & 1124800395214848ULL) << 10);
            case 1024:
                return (x & 1023ULL) | ((x & 1125899906841600ULL) << 10);
            case 0:
                return (x);
            case 3073:
                return ((x & 1048575ULL) << 20) | ((x & 1072693248ULL) << 30);
            case 2049:
                return ((x & 1073741823ULL) << 10) | ((x & 1098437885952ULL) << 20);
            case 1025:
                return ((x & 1099511627775ULL) << 20);
            case 1:
                return ((x & 1125899906842623ULL) << 10);
            case 3074:
                return (x & 1023ULL) | ((x & 1047552ULL) << 10) | ((x & 1072693248ULL) << 30);
            case 2050:
                return (x & 1073741823ULL) | ((x & 1098437885952ULL) << 20);
            case 1026:
                return (x & 1023ULL) | ((x & 1047552ULL) << 10) | ((x & 1099510579200ULL) << 20);
            case 2:
                return (x & 1073741823ULL) | ((x & 1125898833100800ULL) << 10);
            case 3075:
                return ((x & 1023ULL) << 20) | ((x & 1047552ULL) << 40);
            case 2051:
                return ((x & 1048575ULL) << 10) | ((x & 1072693248ULL) << 30);
            case 1027:
                return ((x & 1023ULL) << 20) | ((x & 1073740800ULL) << 30);
            case 3:
                return ((x & 1048575ULL) << 10) | ((x & 1099510579200ULL) << 20);
            case 1051648:
                return (x & 1023ULL) | ((x & 1047552ULL) << 20) | ((x & 1072693248ULL) << 30);
            case 1050624:
                return (x & 1048575ULL) | ((x & 1072693248ULL) << 10) | ((x & 1098437885952ULL) << 20);
            case 1049600:
                return (x & 1023ULL) | ((x & 1099511626752ULL) << 20);
            case 1048576:
                return (x & 1048575ULL) | ((x & 1125899905794048ULL) << 10);
            case 1051649:
                return ((x & 1023ULL) << 30) | ((x & 1047552ULL) << 40);
            case 1050625:
                return ((x & 1023ULL) << 10) | ((x & 1047552ULL) << 20) | ((x & 1072693248ULL) << 30);
            case 1049601:
                return ((x & 1073741823ULL) << 30);
            case 1048577:
                return ((x & 1023ULL) << 10) | ((x & 1099511626752ULL) << 20);
            case 1051650:
                return (x & 1023ULL) | ((x & 1047552ULL) << 40);
            case 1050626:
                return (x & 1048575ULL) | ((x & 1072693248ULL) << 30);
            case 1049602:
                return (x & 1023ULL) | ((x & 1073740800ULL) << 30);
            case 1048578:
                return (x & 1048575ULL) | ((x & 1099510579200ULL) << 20);
            case 1051651:
                return ((x & 1023ULL) << 50);
            case 1050627:
                return ((x & 1023ULL) << 10) | ((x & 1047552ULL) << 40);
            case 1049603:
                return ((x & 1048575ULL) << 40);
            case 1048579:
                return ((x & 1023ULL) << 10) | ((x & 1073740800ULL) << 30);
            case 2100224:
                return (x & 1023ULL) | ((x & 1073740800ULL) << 10);
            case 2099200:
                return (x & 1099511627775ULL);
            case 2098176:
                return (x & 1023ULL) | ((x & 1099511626752ULL) << 10);
            case 2097152:
                return (x & 1125899906842623ULL);
            case 2100225:
                return ((x & 1048575ULL) << 20);
            case 2099201:
                return ((x & 1073741823ULL) << 10);
            case 2098177:
                return ((x & 1073741823ULL) << 20);
            case 2097153:
                return ((x & 1099511627775ULL) << 10);
            case 2100226:
                return (x & 1023ULL) | ((x & 1047552ULL) << 10);
            case 2099202:
                return (x & 1073741823ULL);
            case 2098178:
                return (x & 1023ULL) | ((x & 1047552ULL) << 10) | ((x & 1072693248ULL) << 20);
            case 2097154:
                return (x & 1073741823ULL) | ((x & 1098437885952ULL) << 10);
            case 2100227:
                return ((x & 1023ULL) << 20);
            case 2099203:
                return ((x & 1048575ULL) << 10);
            case 2098179:
                return ((x & 1023ULL) << 20) | ((x & 1047552ULL) << 30);
            case 2097155:
                return ((x & 1048575ULL) << 10) | ((x & 1072693248ULL) << 20);
            case 3148800:
                return (x & 1023ULL) | ((x & 1047552ULL) << 20);
            case 3147776:
                return (x & 1048575ULL) | ((x & 1072693248ULL) << 10);
            case 3146752:
                return (x & 1023ULL) | ((x & 1073740800ULL) << 20);
            case 3145728:
                return (x & 1048575ULL) | ((x & 1099510579200ULL) << 10);
            case 3148801:
                return ((x & 1023ULL) << 30);
            case 3147777:
                return ((x & 1023ULL) << 10) | ((x & 1047552ULL) << 20);
            case 3146753:
                return ((x & 1048575ULL) << 30);
            case 3145729:
                return ((x & 1023ULL) << 10) | ((x & 1073740800ULL) << 20);
            case 3148802:
                return (x & 1023ULL);
            case 3147778:
                return (x & 1048575ULL);
            case 3146754:
                return (x & 1023ULL) | ((x & 1047552ULL) << 30);
            case 3145730:
                return (x & 1048575ULL) | ((x & 1072693248ULL) << 20);
            case 3148803:
                return (0ULL);
            case 3147779:
                return ((x & 1023ULL) << 10);
            case 3146755:
                return ((x & 1023ULL) << 40);
            case 3145731:
                return ((x & 1023ULL) << 10) | ((x & 1047552ULL) << 30);
            default:
                assert(false);
                return x;
        }
    }

    Bitboard insertWhiteLine(Bitboard x, LineKey mask) {
        // 1073741823 = (1 << 30) - 1
        LineKey key = (mask >> 29) | (mask & 1073741823ULL);
        return insertWhiteLine_(x, key);
    }

    const Bitboard kColumnOneLineBelowY[]{
            0ULL, 0x1ULL, 0x401ULL, 0x100401ULL, 0x40100401ULL, 0x10040100401ULL, 0x4010040100401ULL
    };

    Bitboard getColumnOneLineBelowY(int maxY) {
        assert(0 <= maxY && maxY <= 6);
        return kColumnOneLineBelowY[maxY];
    }

    // Returns true if there is no wall (no gap) between the x column and its left column
    bool isWallBetweenLeft(int x, int maxY, Bitboard board) {
        assert(1 <= x && x < 10);

        Bitboard maskHigh = getColumnOneLineBelowY(maxY);
        Bitboard reverseXBoardHigh = ~board;
        Bitboard columnHigh = maskHigh << x;
        Bitboard rightHigh = reverseXBoardHigh & columnHigh;
        Bitboard leftHigh = reverseXBoardHigh & (columnHigh >> 1);
        return ((leftHigh << 1) & rightHigh) == 0L;
    }

    int bitCount(uint64_t b) {
        b -= (b >> 1) & 0x5555555555555555ULL;
        b = ((b >> 2) & 0x3333333333333333ULL) + (b & 0x3333333333333333ULL);
        b = ((b >> 4) + b) & 0x0F0F0F0F0F0F0F0FULL;
        return static_cast<int>((b * 0x0101010101010101ULL) >> 56);
    }
}