#ifndef FUMEN_CPP_PARSER_HPP
#define FUMEN_CPP_PARSER_HPP

#include <string>
#include <vector>

#include "../core/piece.hpp"
#include "color_field.hpp"

namespace fumen {
    struct ActionFlags {
        std::string escapedComment;
        std::string prevEscapedComment;
        bool isLock;
        bool isCommented;
        bool isColor;
        bool isMirror;
        bool isBlockUp;
    };

    const ColorField kNullField(24);
    const std::string kEmptyComment;

    class Element {
    public:
        Element(
                const ColorType color, const core::RotateType rotate, const int x, const int y,
                const bool isLock, ColorField field, std::string comment
        ) : color(color), rotate(rotate), x(x), y(y), isLock(isLock),
            validField(true), field(std::move(field)), comment(std::move(comment)) {
        }

        Element(
                const ColorType color, const core::RotateType rotate, const int x, const int y, std::string comment
        ) : color(color), rotate(rotate), x(x), y(y), isLock(true),
            validField(false), field(kNullField), comment(std::move(comment)) {
        }

        Element(
                const ColorType color, const core::RotateType rotate, const int x, const int y
        ) : color(color), rotate(rotate), x(x), y(y), isLock(true),
            validField(false), field(kNullField), comment(kEmptyComment) {
        }

        const ColorType color;
        const core::RotateType rotate;
        const int x;
        const int y;
        const bool isLock;

        ColorField getFieldOrElse(const ColorField &defaultField) const;

        std::string getEscapedComment() const;

    private:
        const bool validField;
        const ColorField field;
        const std::string comment;
    };

    class FieldEncoder {
    public:
        FieldEncoder(const ColorField &prev, const ColorField &current) : prev(prev), current(current) {}

        std::vector<int> encodedValues{};

        // Specify an empty field if there is no previous field
        // height of input field is 23, width is 10
        bool encode();

    private:
        const ColorField &prev;
        const ColorField &current;

        int getDiff(int xIndex, int yIndex);

        void recordBlockCounts(int diff, int counter);
    };

    class ActionEncoder {
    public:
        ActionEncoder(const Element &element, const ActionFlags &flags) : element(element), flags(flags) {}

        std::vector<int> encodedValues{};

        bool encode();

    private:
        const Element &element;
        const ActionFlags &flags;
    };

    class CommentEncoder {
    public:
        explicit CommentEncoder(const std::string &escapedComment) : escapedComment(escapedComment) {}

        std::vector<int> encodedValues{};

        void encode();

    private:
        const std::string &escapedComment;
    };

    class Parser {
    public:
        Parser(
                const core::Factory &factory, const ColorConverter &converter
        ) : factory(factory), converter(converter) {}

        std::string encode(std::vector<Element> elements);

    private:
        const core::Factory &factory;
        const ColorConverter &converter;

        std::vector<int> encodedValues{};
        int lastRepeatIndex = -1;

        void encodeField(const ColorField &prevField, const ColorField &currentField);
    };
}

#endif //FUMEN_CPP_PARSER_HPP
