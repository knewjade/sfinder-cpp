#include "perfect_clear.hpp"

namespace finder {
    int extractLastHoldPriority(uint8_t priority, core::PieceType hold) {
        int slide = hold != core::PieceType::Empty ? hold : 7;
        uint8_t bit = priority >> static_cast<unsigned>(slide);
        return bit & 1U;
    }

    int compareToLastHoldPriority(uint8_t priority, int bestBit, core::PieceType newHold) {
        // Priority is given when the least significant bit is 1

        int newBit = extractLastHoldPriority(priority, newHold);

        // When the least significant bits are different
        if (0 < (newBit ^ bestBit)) {
            // Return true when giving priority to new
            return 0 < newBit ? 1 : -1;
        }

        return 0;
    }

    // For fast search
    void Recorder<FastCandidate, FastRecord>::clear() {
        best_ = FastRecord{
                std::vector<Operation>{},
                core::PieceType::Empty,
                INT_MAX,
                INT_MAX,
                INT_MAX,
        };
    }

    void Recorder<FastCandidate, FastRecord>::update(
            const Configure &configure, const Solution &solution, const FastCandidate &current
    ) {
        auto hold = 0 <= current.holdIndex ? configure.pieces[current.holdIndex] : core::PieceType::Empty;
        best_ = FastRecord{
                solution, hold,
                current.softdropCount, current.holdCount, current.lineClearCount, current.maxCombo,
                extractLastHoldPriority(configure.lastHoldPriority, hold)
        };
    }

    bool Recorder<FastCandidate, FastRecord>::isWorseThanBest(
            bool leastLineClears, const FastCandidate &current
    ) const {
        if (best_.holdPriority == 0) {
            return false;
        }

        return best_.softdropCount < current.softdropCount;
    }

    bool shouldUpdateLeastLineClear(
            const FastRecord &oldRecord, const FastCandidate &newRecord
    ) {
        if (newRecord.softdropCount != oldRecord.softdropCount) {
            return newRecord.softdropCount < oldRecord.softdropCount;
        }

        if (newRecord.lineClearCount != oldRecord.lineClearCount) {
            return newRecord.lineClearCount < oldRecord.lineClearCount;
        }

        return newRecord.holdCount < oldRecord.holdCount;
    }

    bool shouldUpdateMostLineClear(
            const FastRecord &oldRecord, const FastCandidate &newRecord
    ) {
        if (newRecord.softdropCount != oldRecord.softdropCount) {
            return newRecord.softdropCount < oldRecord.softdropCount;
        }

        if (newRecord.maxCombo != oldRecord.maxCombo) {
            return oldRecord.maxCombo < newRecord.maxCombo;
        }

        if (newRecord.lineClearCount != oldRecord.lineClearCount) {
            return oldRecord.lineClearCount < newRecord.lineClearCount;
        }

        return newRecord.holdCount < oldRecord.holdCount;
    }

    bool Recorder<FastCandidate, FastRecord>::shouldUpdate(
            const Configure &configure, const FastCandidate &newRecord
    ) const {
        if (best_.solution.empty()) {
            return true;
        }

        core::PieceType newHold = 0 <= newRecord.holdIndex
                                  ? configure.pieces[newRecord.holdIndex]
                                  : core::PieceType::Empty;
        auto compare = compareToLastHoldPriority(configure.lastHoldPriority, best_.holdPriority, newHold);
        if (compare != 0) {
            return 0 < compare;
        }

        if (configure.leastLineClears) {
            return shouldUpdateLeastLineClear(best_, newRecord);
        } else {
            return shouldUpdateMostLineClear(best_, newRecord);
        }
    }


    // For T-Spin search
    void Recorder<TSpinCandidate, TSpinRecord>::clear() {
        best_ = TSpinRecord{
                std::vector<Operation>{},
                core::PieceType::Empty,
                INT_MAX,
                INT_MAX,
                INT_MAX,
                0,
        };
    }

    void Recorder<TSpinCandidate, TSpinRecord>::update(
            const Configure &configure, const Solution &solution, const TSpinCandidate &current
    ) {
        auto hold = 0 <= current.holdIndex ? configure.pieces[current.holdIndex] : core::PieceType::Empty;
        best_ = TSpinRecord{
                solution, 0 <= current.holdIndex ? configure.pieces[current.holdIndex] : core::PieceType::Empty,
                current.softdropCount, current.holdCount, current.lineClearCount,
                current.maxCombo, current.tSpinAttack, extractLastHoldPriority(configure.lastHoldPriority, hold)
        };
    }

    bool Recorder<TSpinCandidate, TSpinRecord>::isWorseThanBest(
            bool leastLineClears, const TSpinCandidate &current
    ) const {
        if (best_.holdPriority == 0) {
            return false;
        }

        if (current.leftNumOfT == 0) {
            if (current.tSpinAttack != best_.tSpinAttack) {
                return current.tSpinAttack < best_.tSpinAttack;
            }

            return best_.softdropCount < current.softdropCount;
        }

        return false;
    }

    bool shouldUpdateLeastLineClear(
            const TSpinRecord &oldRecord, const TSpinCandidate &newRecord
    ) {
        if (newRecord.tSpinAttack != oldRecord.tSpinAttack) {
            return oldRecord.tSpinAttack < newRecord.tSpinAttack;
        }

        if (newRecord.softdropCount != oldRecord.softdropCount) {
            return newRecord.softdropCount < oldRecord.softdropCount;
        }

        if (newRecord.lineClearCount != oldRecord.lineClearCount) {
            return newRecord.lineClearCount < oldRecord.lineClearCount;
        }

        return newRecord.holdCount < oldRecord.holdCount;
    }

    bool shouldUpdateMostLineClear(
            const TSpinRecord &oldRecord, const TSpinCandidate &newRecord
    ) {
        if (newRecord.tSpinAttack != oldRecord.tSpinAttack) {
            return oldRecord.tSpinAttack < newRecord.tSpinAttack;
        }

        if (newRecord.softdropCount != oldRecord.softdropCount) {
            return newRecord.softdropCount < oldRecord.softdropCount;
        }

        if (newRecord.maxCombo != oldRecord.maxCombo) {
            return oldRecord.maxCombo < newRecord.maxCombo;
        }

        if (newRecord.lineClearCount != oldRecord.lineClearCount) {
            return oldRecord.lineClearCount < newRecord.lineClearCount;
        }

        return newRecord.holdCount < oldRecord.holdCount;
    }

    bool Recorder<TSpinCandidate, TSpinRecord>::shouldUpdate(
            const Configure &configure, const TSpinCandidate &newRecord
    ) const {
        if (best_.solution.empty()) {
            return true;
        }

        core::PieceType newHold = 0 <= newRecord.holdIndex
                                  ? configure.pieces[newRecord.holdIndex]
                                  : core::PieceType::Empty;
        auto compare = compareToLastHoldPriority(configure.lastHoldPriority, best_.holdPriority, newHold);
        if (compare != 0) {
            return 0 < compare;
        }

        if (configure.leastLineClears) {
            return shouldUpdateLeastLineClear(best_, newRecord);
        } else {
            return shouldUpdateMostLineClear(best_, newRecord);
        }
    }


    // For all spins search
    void Recorder<AllSpinsCandidate, AllSpinsRecord>::clear() {
        best_ = AllSpinsRecord{
                std::vector<Operation>{},
                core::PieceType::Empty,
                INT_MAX,
                INT_MAX,
                INT_MAX,
                0,
                0,
        };
    }

    void Recorder<AllSpinsCandidate, AllSpinsRecord>::update(
            const Configure &configure, const Solution &solution, const AllSpinsCandidate &current
    ) {
        auto hold = 0 <= current.holdIndex ? configure.pieces[current.holdIndex] : core::PieceType::Empty;
        best_ = AllSpinsRecord{
                solution, 0 <= current.holdIndex ? configure.pieces[current.holdIndex] : core::PieceType::Empty,
                current.softdropCount, current.holdCount, current.lineClearCount,
                current.maxCombo, current.spinAttack, extractLastHoldPriority(configure.lastHoldPriority, hold)
        };
    }

    bool Recorder<AllSpinsCandidate, AllSpinsRecord>::isWorseThanBest(
            bool leastLineClears, const AllSpinsCandidate &current
    ) const {
        // There is a high possibility of spin attack until the last piece. so, it's difficult to prune along the way
        return false;
    }

    bool shouldUpdateLeastLineClear(
            const AllSpinsRecord &oldRecord, const AllSpinsCandidate &newRecord
    ) {
        if (newRecord.spinAttack != oldRecord.spinAttack) {
            return oldRecord.spinAttack < newRecord.spinAttack;
        }

        if (newRecord.softdropCount != oldRecord.softdropCount) {
            return newRecord.softdropCount < oldRecord.softdropCount;
        }

        if (newRecord.lineClearCount != oldRecord.lineClearCount) {
            return newRecord.lineClearCount < oldRecord.lineClearCount;
        }

        return newRecord.holdCount < oldRecord.holdCount;
    }

    bool shouldUpdateMostLineClear(
            const AllSpinsRecord &oldRecord, const AllSpinsCandidate &newRecord
    ) {
        if (newRecord.spinAttack != oldRecord.spinAttack) {
            return oldRecord.spinAttack < newRecord.spinAttack;
        }

        if (newRecord.softdropCount != oldRecord.softdropCount) {
            return newRecord.softdropCount < oldRecord.softdropCount;
        }

        if (newRecord.maxCombo != oldRecord.maxCombo) {
            return oldRecord.maxCombo < newRecord.maxCombo;
        }

        if (newRecord.lineClearCount != oldRecord.lineClearCount) {
            return oldRecord.lineClearCount < newRecord.lineClearCount;
        }

        return newRecord.holdCount < oldRecord.holdCount;
    }

    bool Recorder<AllSpinsCandidate, AllSpinsRecord>::shouldUpdate(
            const Configure &configure, const AllSpinsCandidate &newRecord
    ) const {
        if (best_.solution.empty()) {
            return true;
        }

        core::PieceType newHold = 0 <= newRecord.holdIndex
                                  ? configure.pieces[newRecord.holdIndex]
                                  : core::PieceType::Empty;
        auto compare = compareToLastHoldPriority(configure.lastHoldPriority, best_.holdPriority, newHold);
        if (compare != 0) {
            return 0 < compare;
        }

        if (configure.leastLineClears) {
            return shouldUpdateLeastLineClear(best_, newRecord);
        } else {
            return shouldUpdateMostLineClear(best_, newRecord);
        }
    }
}