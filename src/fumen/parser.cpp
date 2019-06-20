#include <sstream>
#include <iostream>
#include <cmath>

#include "parser.hpp"

namespace fumen {
    namespace {
        const auto ASCII_CHARACTERS = std::string(
                "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz 0123456789 @*_+-./"
        );

        const auto ENCODE_TABLE = std::string(
                "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
        );

        const auto COMMENT_TABLE = std::string(
                " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~"
        );

        const int ENCODE_TABLE_SIZE = ENCODE_TABLE.size();
        const int COMMENT_TABLE_SIZE = COMMENT_TABLE.size() + 1;  // Fumen specifications

        const int kTETFU_FIELD_TOP = 23;
        const int kTETFU_MAX_HEIGHT = kTETFU_FIELD_TOP + 1;

        const int kTETFU_FIELD_WIDTH = 10;
        const int kTETFU_FIELD_BLOCKS = kTETFU_MAX_HEIGHT * kTETFU_FIELD_WIDTH;

        // Unsupported multi-byte character set
        std::string escape(const std::string &str) {
            std::stringstream out;
            for (const auto &c : str) {
                if (std::find(ASCII_CHARACTERS.begin(), ASCII_CHARACTERS.end(), c) != ASCII_CHARACTERS.end()) {
                    out << c;
                } else {
                    out << std::hex << c;
                }
            }
            return out.str();
        }

        int encodeCommentChar(unsigned char c) {
            for (int index = 0; index < COMMENT_TABLE.size(); ++index) {
                if (COMMENT_TABLE[index] == c) {
                    return index;
                }
            }
            assert(false);
        }

        unsigned char encodeData(int value) {
            return ENCODE_TABLE.at(value);
        }

        void pushValues(std::vector<int> &encodedValues, int value, int splitCount) {
            int current = value;
            for (int count = 0; count < splitCount; count++) {
                encodedValues.push_back(current % ENCODE_TABLE_SIZE);
                current /= ENCODE_TABLE_SIZE;
            }
        }
    }

    namespace {
        int parseBool(bool flag) {
            return flag ? 1 : 0;
        }

        int coordinate_to_int(Element element) {
            int x = element.x;
            int y = element.y;
            ColorType type = element.color;
            core::RotateType rotate = element.rotate;

            if (type == ColorType::Empty || type == ColorType::Gray) {
                x = 0;
                y = 22;
            } else if (type == ColorType::O && rotate == core::RotateType::Left) {
                x -= 1;
                y += 1;
            } else if (type == ColorType::O && rotate == core::RotateType::Reverse)
                x -= 1;
            else if (type == ColorType::O && rotate == core::RotateType::Spawn)
                y += 1;
            else if (type == ColorType::I && rotate == core::RotateType::Reverse)
                x -= 1;
            else if (type == ColorType::I && rotate == core::RotateType::Left)
                y += 1;
            else if (type == ColorType::S && rotate == core::RotateType::Spawn)
                y += 1;
            else if (type == ColorType::S && rotate == core::RotateType::Right)
                x += 1;
            else if (type == ColorType::Z && rotate == core::RotateType::Spawn)
                y += 1;
            else if (type == ColorType::Z && rotate == core::RotateType::Left)
                x -= 1;

            return (kTETFU_FIELD_TOP - y - 1) * kTETFU_FIELD_WIDTH + x;
        }

        int parseRotate(Element element) {
            ColorType type = element.color;
            core::RotateType rotate = element.rotate;

            if (type == ColorType::Empty || type == ColorType::Gray)
                return 0;

            switch (rotate) {
                case core::RotateType::Reverse:
                    return 0;
                case core::RotateType::Right:
                    return 1;
                case core::RotateType::Spawn:
                    return 2;
                case core::RotateType::Left:
                    return 3;
            }

            assert(false);
        }
    }

    bool FieldEncoder::encode() {
        // Convert field value to number of contiguous blocks
        bool isChanged = false;
        int prev_diff = getDiff(0, 0);
        int counter = -1;
        for (int yIndex = 0; yIndex < kTETFU_MAX_HEIGHT; yIndex++) {
            for (int xIndex = 0; xIndex < kTETFU_FIELD_WIDTH; xIndex++) {
                int diff = getDiff(xIndex, yIndex);
                if (diff != prev_diff) {
                    recordBlockCounts(prev_diff, counter);
                    counter = 0;
                    prev_diff = diff;
                    isChanged = true;
                } else {
                    counter += 1;
                }
            }
        }

        // Process last consecutive block
        recordBlockCounts(prev_diff, counter);

        return isChanged;
    }

    int FieldEncoder::getDiff(int xIndex, int yIndex) {
        int y = kTETFU_FIELD_TOP - yIndex - 1;
        if (y < 0) {
            return 8;
        }
        return current.at(xIndex, y) - prev.at(xIndex, y) + 8;
    }

    void FieldEncoder::recordBlockCounts(int diff, int counter) {
        int value = diff * kTETFU_FIELD_BLOCKS + counter;
        pushValues(encodedValues, value, 2);
    }

    bool ActionEncoder::encode() {
        bool isComment = flags.isCommented && flags.escapedComment != flags.prevEscapedComment;

        int value = parseBool(!flags.isLock);
        value *= 2;
        value += parseBool(isComment);
        value *= 2;
        value += (parseBool(flags.isColor));
        value *= 2;
        value += parseBool(flags.isMirror);
        value *= 2;
        value += parseBool(flags.isBlockUp);
        value *= kTETFU_FIELD_BLOCKS;
        value += coordinate_to_int(element);
        value *= 4;
        value += parseRotate(element);
        value *= 8;
        value += element.color;

        pushValues(encodedValues, value, 3);

        return isComment;
    }

    void CommentEncoder::encode() {
        int commentLength = escapedComment.length();
        if (4096 <= commentLength)
            commentLength = 4095;

        pushValues(encodedValues, commentLength, 2);

        // Code comments
        for (int index = 0; index < commentLength; index += 4) {
            int value = 0;
            for (int count = 0; count < 4; count++) {
                int newIndex = index + count;
                if (commentLength <= newIndex)
                    break;
                unsigned char c = escapedComment.at(newIndex);
                value += encodeCommentChar(c) * (int) std::pow(COMMENT_TABLE_SIZE, count);
            }

            pushValues(encodedValues, value, 5);
        }
    }

    ColorField Element::getFieldOrElse(const ColorField &defaultField) const {
        if (validField) {
            return field;
        }
        return defaultField;
    }

    std::string Element::getEscapedComment() const {
        return escape(comment);
    }

    void Parser::encodeField(const ColorField &prevField, const ColorField &currentField) {
        auto encoder = FieldEncoder(prevField, currentField);
        bool isChanged = encoder.encode();

        if (isChanged) {
            // Record the field and end repeat
            auto values = encoder.encodedValues;
            std::copy(values.begin(), values.end(), std::back_inserter(encodedValues));
            lastRepeatIndex = -1;
        } else if (lastRepeatIndex < 0 || encodedValues.at(lastRepeatIndex) == ENCODE_TABLE_SIZE - 1) {
            // Record the field and start repeat
            auto values = encoder.encodedValues;
            std::copy(values.begin(), values.end(), std::back_inserter(encodedValues));
            encodedValues.push_back(0);
            lastRepeatIndex = encodedValues.size() - 1;
        } else if (encodedValues.at(lastRepeatIndex) < (ENCODE_TABLE_SIZE - 1)) {
            // Do not record the field and advance repeat
            int currentRepeatValue = encodedValues.at(lastRepeatIndex);
            encodedValues[lastRepeatIndex] = currentRepeatValue + 1;
        }
    }

    std::string Parser::encode(std::vector<Element> elements) {
        auto prevField = ColorField(kTETFU_MAX_HEIGHT);
        std::string prevComment;

        for (int index = 0; index < elements.size(); index++) {
            auto element = elements.at(index);
            auto field = ColorField(element.getFieldOrElse(prevField));

            // field settings
            // `prevField` is a field created by placing the previous Mino
            // `field` is the field to be displayed next.
            // This time, use the field that made by putting one before Mino except the first
            encodeField(prevField, field);

            auto comment = element.getEscapedComment();
            auto flags = ActionFlags{
                    comment, prevComment, element.isLock, true, index == 0, false, false,
            };

            auto actionEncoder = ActionEncoder(element, flags);
            bool isCommented = actionEncoder.encode();
            auto values = actionEncoder.encodedValues;
            std::copy(values.begin(), values.end(), std::back_inserter(encodedValues));

            if (isCommented) {
                auto commentEncoder = CommentEncoder(flags.escapedComment);
                commentEncoder.encode();
                auto values2 = commentEncoder.encodedValues;
                std::copy(values2.begin(), values2.end(), std::back_inserter(encodedValues));
            }

            ColorType colorType = element.color;
            if (flags.isLock) {
                if (colorType != ColorType::Empty && colorType != ColorType::Gray) {
                    auto piece = converter.parseToPieceType(colorType);
                    auto blocks = factory.get(piece, element.rotate);
                    field.put(blocks, element.x, element.y);
                }

                field.clearLine();

                if (flags.isBlockUp) {
                    assert(false);
                }

                if (flags.isMirror) {
                    assert(false);
                }
            }
            // next field
            prevField = field;
            prevComment = comment;
        }

        std::stringstream out;
        for (int index = 0; index < encodedValues.size(); index++) {
            int value = encodedValues.at(index);
            auto encoded = encodeData(value);
            out << encoded;
            if (index % 47 == 41)
                out << '?';
        }
        return out.str();
    }
}