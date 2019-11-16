#include "perfect.hpp"

namespace finder {
    void Recorder<TSpinCandidate, TSpinRecord>::clear() {
        best_ = TSpinRecord{
                std::vector<Operation>{},
                INT_MAX,
                INT_MAX,
                INT_MAX,
                0,
        };
    }

    void Recorder<TSpinCandidate, TSpinRecord>::update(const Solution &solution, const TSpinCandidate &current) {
        assert(!best_.solution.empty());
        best_ = TSpinRecord{
                solution, current.softdropCount, current.holdCount, current.lineClearCount,
                current.maxCombo, current.tSpinAttack
        };
    }

    bool Recorder<TSpinCandidate, TSpinRecord>::isWorseThanBest(
            bool leastLineClears, const TSpinCandidate &current
    ) const {
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
            bool leastLineClears, const TSpinCandidate &newRecord
    ) const {
        if (best_.solution.empty()) {
            return true;
        }

        if (leastLineClears) {
            return shouldUpdateLeastLineClear(best_, newRecord);
        } else {
            return shouldUpdateMostLineClear(best_, newRecord);
        }
    }

    void Recorder<FastCandidate, FastRecord>::clear() {
        best_ = FastRecord{
                std::vector<Operation>{},
                INT_MAX,
                INT_MAX,
                INT_MAX,
        };
    }

    void Recorder<FastCandidate, FastRecord>::update(const Solution &solution, const FastCandidate &current) {
        assert(!best_.solution.empty());
        best_ = FastRecord{
                solution, current.softdropCount, current.holdCount, current.lineClearCount, current.maxCombo
        };
    }

    bool Recorder<FastCandidate, FastRecord>::isWorseThanBest(
            bool leastLineClears, const FastCandidate &current
    ) const {
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
            bool leastLineClears, const FastCandidate &newRecord
    ) const {
        if (best_.solution.empty()) {
            return true;
        }

        if (leastLineClears) {
            return shouldUpdateLeastLineClear(best_, newRecord);
        } else {
            return shouldUpdateMostLineClear(best_, newRecord);
        }
    }
}