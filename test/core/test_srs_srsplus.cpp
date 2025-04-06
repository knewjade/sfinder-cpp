#include "gtest/gtest.h"

#include "core/field.hpp"
#include "core/srs.hpp"
#include "core/types.hpp"

namespace core {
    using namespace std::literals::string_literals;

    namespace srs {
        class SRSPlusRotationTest : public ::testing::Test {
        };

        TEST_F(SRSPlusRotationTest, case_i_cw_from_spawn) {
            auto factory = Factory::createForSSRPlus();
            auto &piece = factory.get(I);

            constexpr auto bias = 0;

            {
                const auto field = createField(""s +
                        "__________"s +
                        "___    ___"s +
                        "__________"s +
                        "__________"s +
                        "__________"s +
                        ""
                );
                const auto index = right(field, piece, Spawn, Right, 4, 3);
                EXPECT_EQ(index - bias, 0);
            }
            {
                const auto field = createField(""s +
                        "__________"s +
                        "___    ___"s +
                        "__________"s +
                        "_____X____"s +
                        "__________"s +
                        ""
                );
                const auto index = right(field, piece, Spawn, Right, 4, 3);
                EXPECT_EQ(index - bias, 1);
            }
            {
                const auto field = createField(""s +
                        "__________"s +
                        "___    ___"s +
                        "__________"s +
                        "_____XX___"s +
                        "__________"s +
                        ""
                );
                const auto index = right(field, piece, Spawn, Right, 4, 3);
                EXPECT_EQ(index - bias, 2);
            }
            {
                const auto field = createField(""s +
                        "___X______"s +
                        "___    ___"s +
                        "__________"s +
                        "_____XX___"s +
                        "__________"s +
                        ""
                );
                const auto index = right(field, piece, Spawn, Right, 4, 3);
                EXPECT_EQ(index - bias, 3);
            }
            {
                const auto field = createField(""s +
                        "___X______"s +
                        "___    ___"s +
                        "__________"s +
                        "_____XX___"s +
                        "___X______"s +
                        ""
                );
                const auto index = right(field, piece, Spawn, Right, 4, 3);
                EXPECT_EQ(index - bias, 4);
            }
            {
                const auto field = createField(""s +
                        "______X___"s +
                        "__________"s +
                        "___X______"s +
                        "___    ___"s +
                        "__________"s +
                        "_____XX___"s +
                        "___X______"s +
                        ""
                );
                const auto index = right(field, piece, Spawn, Right, 4, 3);
                EXPECT_EQ(index, -1);
            }
        }

        TEST_F(SRSPlusRotationTest, case_i_cw_from_right) {
            auto factory = Factory::createForSSRPlus();
            auto &piece = factory.get(I);

            constexpr auto bias = 5;

            {
                const auto field = createField(""s +
                        "_____ ____"s +
                        "_____ ____"s +
                        "_____ ____"s +
                        "_____ ____"s +
                        "__________"s +
                        ""
                );
                const auto index = right(field, piece, Right, Reverse, 5, 3);
                EXPECT_EQ(index - bias, 0);
            }
            {
                const auto field = createField(""s +
                        "_____ ____"s +
                        "_____ ____"s +
                        "_____ X___"s +
                        "_____ ____"s +
                        "__________"s +
                        ""
                );
                const auto index = right(field, piece, Right, Reverse, 5, 3);
                EXPECT_EQ(index - bias, 1);
            }
            {
                const auto field = createField(""s +
                        "_____ ____"s +
                        "_____ ____"s +
                        "__XX_ ____"s +
                        "_____ ____"s +
                        "__________"s +
                        ""
                );
                const auto index = right(field, piece, Right, Reverse, 5, 3);
                EXPECT_EQ(index - bias, 2);
            }
            {
                const auto field = createField(""s +
                        "_____ ____"s +
                        "_____ ____"s +
                        "__XX_ __X_"s +
                        "_____ ____"s +
                        "__________"s +
                        ""
                );
                const auto index = right(field, piece, Right, Reverse, 5, 3);
                EXPECT_EQ(index - bias, 3);
            }
            {
                const auto field = createField(""s +
                        "__X__ ____"s +
                        "_____ ____"s +
                        "__XX_ __X_"s +
                        "_____ ____"s +
                        "__________"s +
                        ""
                );
                const auto index = right(field, piece, Right, Reverse, 5, 3);
                EXPECT_EQ(index - bias, 4);
            }
            {
                const auto field = createField(""s +
                        "__X__ ____"s +
                        "_____ ____"s +
                        "__XX_ __X_"s +
                        "_____ __X_"s +
                        "__________"s +
                        ""
                );
                const auto index = right(field, piece, Right, Reverse, 5, 3);
                EXPECT_EQ(index, -1);
            }
        }

        TEST_F(SRSPlusRotationTest, case_i_cw_from_reverse) {
            auto factory = Factory::createForSSRPlus();
            auto &piece = factory.get(I);

            constexpr auto bias = 10;

            {
                const auto field = createField(""s +
                        "__________"s +
                        "__________"s +
                        "___    ___"s +
                        "__________"s +
                        "__________"s +
                        "__________"s +
                        ""
                );
                const auto index = right(field, piece, Reverse, Left, 5, 3);
                EXPECT_EQ(index - bias, 0);
            }
            {
                const auto field = createField(""s +
                        "__________"s +
                        "__________"s +
                        "___    ___"s +
                        "____X_____"s +
                        "__________"s +
                        "__________"s +
                        ""
                );
                const auto index = right(field, piece, Reverse, Left, 5, 3);
                EXPECT_EQ(index - bias, 1);
            }
            {
                const auto field = createField(""s +
                        "__________"s +
                        "__________"s +
                        "___    ___"s +
                        "____X_X___"s +
                        "__________"s +
                        "__________"s +
                        ""
                );
                const auto index = right(field, piece, Reverse, Left, 5, 3);
                EXPECT_EQ(index - bias, 2);
            }
            {
                const auto field = createField(""s +
                        "__________"s +
                        "___X______"s +
                        "___    ___"s +
                        "____X_X___"s +
                        "__________"s +
                        "__________"s +
                        ""
                );
                const auto index = right(field, piece, Reverse, Left, 5, 3);
                EXPECT_EQ(index - bias, 3);
            }
            {
                const auto field = createField(""s +
                        "______X___"s +
                        "__________"s +
                        "___X______"s +
                        "___    ___"s +
                        "____X_X___"s +
                        "__________"s +
                        "__________"s +
                        ""
                );
                const auto index = right(field, piece, Reverse, Left, 5, 3);
                EXPECT_EQ(index - bias, 4);
            }
            {
                const auto field = createField(""s +
                        "______X___"s +
                        "__________"s +
                        "___X______"s +
                        "___    ___"s +
                        "____X_X___"s +
                        "__________"s +
                        "___X______"s +
                        ""
                );
                const auto index = right(field, piece, Reverse, Left, 5, 3);
                EXPECT_EQ(index, -1);
            }
        }

        TEST_F(SRSPlusRotationTest, case_i_cw_from_left) {
            auto factory = Factory::createForSSRPlus();
            auto &piece = factory.get(I);

            constexpr auto bias = 15;

            {
                const auto field = createField(""s +
                        "__________"s +
                        "____ _____"s +
                        "____ _____"s +
                        "____ _____"s +
                        "____ _____"s +
                        "__________"s +
                        ""
                );
                const auto index = right(field, piece, Left, Spawn, 4, 2);
                EXPECT_EQ(index - bias, 0);
            }
            {
                const auto field = createField(""s +
                        "__________"s +
                        "____ _____"s +
                        "___X _____"s +
                        "____ _____"s +
                        "____ _____"s +
                        "__________"s +
                        ""
                );
                const auto index = right(field, piece, Left, Spawn, 4, 2);
                EXPECT_EQ(index - bias, 1);
            }
            {
                const auto field = createField(""s +
                        "__________"s +
                        "____ _____"s +
                        "____ _XX__"s +
                        "____ _____"s +
                        "____ _____"s +
                        "__________"s +
                        ""
                );
                const auto index = right(field, piece, Left, Spawn, 4, 2);
                EXPECT_EQ(index - bias, 2);
            }
            {
                const auto field = createField(""s +
                        "__________"s +
                        "____ _____"s +
                        "_X__ _XX__"s +
                        "____ _____"s +
                        "____ _____"s +
                        "__________"s +
                        ""
                );
                const auto index = right(field, piece, Left, Spawn, 4, 2);
                EXPECT_EQ(index - bias, 3);
            }
            {
                const auto field = createField(""s +
                        "__________"s +
                        "____ _____"s +
                        "_X__ _XX__"s +
                        "____ _____"s +
                        "____ __X__"s +
                        "__________"s +
                        ""
                );
                const auto index = right(field, piece, Left, Spawn, 4, 2);
                EXPECT_EQ(index - bias, 4);
            }
            {
                const auto field = createField(""s +
                        "__________"s +
                        "_X__ _____"s +
                        "_X__ _XX__"s +
                        "____ _____"s +
                        "____ __X__"s +
                        "__________"s +
                        ""
                );
                const auto index = right(field, piece, Left, Spawn, 4, 2);
                EXPECT_EQ(index, -1);
            }
        }

        TEST_F(SRSPlusRotationTest, case_i_ccw_from_spawn) {
            auto factory = Factory::createForSSRPlus();
            auto &piece = factory.get(I);

            constexpr auto bias = 0;

            {
                const auto field = createField(""s +
                        "__________"s +
                        "___    ___"s +
                        "__________"s +
                        "__________"s +
                        "__________"s +
                        ""
                );
                const auto index = left(field, piece, Spawn, Left, 4, 3);
                EXPECT_EQ(index - bias, 0);
            }
            {
                const auto field = createField(""s +
                        "____X_____"s +
                        "___    ___"s +
                        "__________"s +
                        "__________"s +
                        "__________"s +
                        ""
                );
                const auto index = left(field, piece, Spawn, Left, 4, 3);
                EXPECT_EQ(index - bias, 1);
            }
            {
                const auto field = createField(""s +
                        "____X_____"s +
                        "___    ___"s +
                        "__________"s +
                        "___X______"s +
                        "__________"s +
                        ""
                );
                const auto index = left(field, piece, Spawn, Left, 4, 3);
                EXPECT_EQ(index - bias, 2);
            }
            {
                const auto field = createField(""s +
                        "____X_X___"s +
                        "___    ___"s +
                        "__________"s +
                        "___X______"s +
                        "__________"s +
                        ""
                );
                const auto index = left(field, piece, Spawn, Left, 4, 3);
                EXPECT_EQ(index - bias, 3);
            }
            {
                const auto field = createField(""s +
                        "____X_X___"s +
                        "___    ___"s +
                        "__________"s +
                        "___X______"s +
                        "______X___"s +
                        ""
                );
                const auto index = left(field, piece, Spawn, Left, 4, 3);
                EXPECT_EQ(index - bias, 4);
            }
            {
                const auto field = createField(""s +
                        "___X______"s +
                        "__________"s +
                        "____X_X___"s +
                        "___    ___"s +
                        "__________"s +
                        "___X______"s +
                        "______X___"s +
                        ""
                );
                const auto index = left(field, piece, Spawn, Left, 4, 3);
                EXPECT_EQ(index, -1);
            }
        }

        TEST_F(SRSPlusRotationTest, case_i_ccw_from_right) {
            auto factory = Factory::createForSSRPlus();
            auto &piece = factory.get(I);

            constexpr auto bias = 5;

            {
                const auto field = createField(""s +
                        "_____ ____"s +
                        "_____ ____"s +
                        "_____ ____"s +
                        "_____ ____"s +
                        "__________"s +
                        ""
                );
                const auto index = left(field, piece, Right, Spawn, 5, 3);
                EXPECT_EQ(index - bias, 0);
            }
            {
                const auto field = createField(""s +
                        "_____ ____"s +
                        "_____ X___"s +
                        "_____ ____"s +
                        "_____ ____"s +
                        "__________"s +
                        ""
                );
                const auto index = left(field, piece, Right, Spawn, 5, 3);
                EXPECT_EQ(index - bias, 1);
            }
            {
                const auto field = createField(""s +
                        "_____ ____"s +
                        "__XX_ ____"s +
                        "_____ ____"s +
                        "_____ ____"s +
                        "__________"s +
                        ""
                );
                const auto index = left(field, piece, Right, Spawn, 5, 3);
                EXPECT_EQ(index - bias, 2);
            }
            {
                const auto field = createField(""s +
                        "_____ ____"s +
                        "__XX_ __X_"s +
                        "_____ ____"s +
                        "_____ ____"s +
                        "__________"s +
                        ""
                );
                const auto index = left(field, piece, Right, Spawn, 5, 3);
                EXPECT_EQ(index - bias, 3);
            }
            {
                const auto field = createField(""s +
                        "_____ ____"s +
                        "__XX_ __X_"s +
                        "_____ ____"s +
                        "__X__ ____"s +
                        "__________"s +
                        ""
                );
                const auto index = left(field, piece, Right, Spawn, 5, 3);
                EXPECT_EQ(index - bias, 4);
            }
            {
                const auto field = createField(""s +
                        "_____ __X_"s +
                        "__XX_ __X_"s +
                        "_____ ____"s +
                        "__X__ ____"s +
                        "__________"s +
                        ""
                );
                const auto index = left(field, piece, Right, Spawn, 5, 3);
                EXPECT_EQ(index, -1);
            }
        }

        TEST_F(SRSPlusRotationTest, case_i_ccw_from_reverse) {
            auto factory = Factory::createForSSRPlus();
            auto &piece = factory.get(I);

            constexpr auto bias = 10;

            {
                const auto field = createField(""s +
                          "__________"s +
                          "__________"s +
                          "__________"s +
                          "___    ___"s +
                          "__________"s +
                          "__________"s +
                          "__________"s +
                          ""
                  );
                const auto index = left(field, piece, Reverse, Right, 5, 3);
                EXPECT_EQ(index - bias, 0);
            }
            {
                const auto field = createField(""s +
                          "__________"s +
                          "__________"s +
                          "__________"s +
                          "___    ___"s +
                          "_____X____"s +
                          "__________"s +
                          "__________"s +
                          ""
                  );
                const auto index = left(field, piece, Reverse, Right, 5, 3);
                EXPECT_EQ(index - bias, 1);
            }
            {
                const auto field = createField(""s +
                          "__________"s +
                          "__________"s +
                          "__________"s +
                          "___    ___"s +
                          "___X_X____"s +
                          "__________"s +
                          "__________"s +
                          ""
                  );
                const auto index = left(field, piece, Reverse, Right, 5, 3);
                EXPECT_EQ(index - bias, 2);
            }
            {
                const auto field = createField(""s +
                          "__________"s +
                          "______X___"s +
                          "__________"s +
                          "___    ___"s +
                          "___X_X____"s +
                          "__________"s +
                          "__________"s +
                          ""
                  );
                const auto index = left(field, piece, Reverse, Right, 5, 3);
                EXPECT_EQ(index - bias, 3);
            }
            {
                const auto field = createField(""s +
                          "___X______"s +
                          "______X___"s +
                          "__________"s +
                          "___    ___"s +
                          "___X_X____"s +
                          "__________"s +
                          "__________"s +
                          ""
                  );
                const auto index = left(field, piece, Reverse, Right, 5, 3);
                EXPECT_EQ(index - bias, 4);
            }
            {
                const auto field = createField(""s +
                          "___X______"s +
                          "______X___"s +
                          "__________"s +
                          "___    ___"s +
                          "___X_X____"s +
                          "__________"s +
                          "______X___"s +
                          ""
                  );
                const auto index = left(field, piece, Reverse, Right, 5, 3);
                EXPECT_EQ(index, -1);
            }
        }

        TEST_F(SRSPlusRotationTest, case_i_ccw_from_left) {
            auto factory = Factory::createForSSRPlus();
            auto &piece = factory.get(I);

            constexpr auto bias = 15;

            {
                const auto field = createField(""s +
                        "__________"s +
                        "____ _____"s +
                        "____ _____"s +
                        "____ _____"s +
                        "____ _____"s +
                        "__________"s +
                        ""
                );
                const auto index = left(field, piece, Left, Reverse, 4, 2);
                EXPECT_EQ(index - bias, 0);
            }
            {
                const auto field = createField(""s +
                        "__________"s +
                        "____ _____"s +
                        "____ _____"s +
                        "___X _____"s +
                        "____ _____"s +
                        "__________"s +
                        ""
                );
                const auto index = left(field, piece, Left, Reverse, 4, 2);
                EXPECT_EQ(index - bias, 1);
            }
            {
                const auto field = createField(""s +
                        "__________"s +
                        "____ _____"s +
                        "____ _____"s +
                        "____ _XX__"s +
                        "____ _____"s +
                        "__________"s +
                        ""
                );
                const auto index = left(field, piece, Left, Reverse, 4, 2);
                EXPECT_EQ(index - bias, 2);
            }
            {
                const auto field = createField(""s +
                        "__________"s +
                        "____ _____"s +
                        "____ _____"s +
                        "_X__ _XX__"s +
                        "____ _____"s +
                        "__________"s +
                        ""
                );
                const auto index = left(field, piece, Left, Reverse, 4, 2);
                EXPECT_EQ(index - bias, 3);
            }
            {
                const auto field = createField(""s +
                        "__________"s +
                        "____ __X__"s +
                        "____ _____"s +
                        "_X__ _XX__"s +
                        "____ _____"s +
                        "__________"s +
                        ""
                );
                const auto index = left(field, piece, Left, Reverse, 4, 2);
                EXPECT_EQ(index - bias, 4);
            }
            {
                const auto field = createField(""s +
                        "__________"s +
                        "____ __X__"s +
                        "____ _____"s +
                        "_X__ _XX__"s +
                        "_X__ _____"s +
                        "__________"s +
                        ""
                );
                const auto index = left(field, piece, Left, Reverse, 4, 2);
                EXPECT_EQ(index, -1);
            }
        }

        TEST_F(SRSPlusRotationTest, case_t_180_from_spawn) {
            auto factory = Factory::createForSSRPlus();
            auto &piece = factory.get(T);

            constexpr auto bias = 0;

            {
                const auto field = createField(""s +
                        "__________"s +
                        "____ _____"s +
                        "___   ____"s +
                        "XXXX_XXXX_"s +
                        ""
                );
                const auto index = rotate180(field, piece, Spawn, Reverse, 4, 1);
                EXPECT_EQ(index - bias, 0);
            }
            {
                const auto field = createField(""s +
                        "__________"s +
                        "____ _____"s +
                        "___   ____"s +
                        "XXXXXXXXX_"s +
                        ""
                );
                const auto index = rotate180(field, piece, Spawn, Reverse, 4, 1);
                EXPECT_EQ(index - bias, 1);
            }
            {
                const auto field = createField(""s +
                        "__________"s +
                        "__XX _____"s +
                        "__X   ____"s +
                        "XXXXXXXXX_"s +
                        ""
                );
                const auto index = rotate180(field, piece, Spawn, Reverse, 4, 1);
                EXPECT_EQ(index - bias, 2);
            }
            {
                const auto field = createField(""s +
                        "__________"s +
                        "____ XX___"s +
                        "___   X___"s +
                        "XXXXXXXXX_"s +
                        ""
                );
                const auto index = rotate180(field, piece, Spawn, Reverse, 4, 1);
                EXPECT_EQ(index - bias, 3);
            }
            {
                const auto field = createField(""s +
                        "__________"s +
                        "___X XXX__"s +
                        "___   _X__"s +
                        "XXXXX_XXX_"s +
                        ""
                );
                const auto index = rotate180(field, piece, Spawn, Reverse, 4, 1);
                EXPECT_EQ(index - bias, 4);
            }
            {
                const auto field = createField(""s +
                        "__________"s +
                        "_XXX X____"s +
                        "_X_   ____"s +
                        "XXX_XXXXX_"s +
                        ""
                );
                const auto index = rotate180(field, piece, Spawn, Reverse, 4, 1);
                EXPECT_EQ(index - bias, 5);
            }
            {
                const auto field = createField(""s +
                        "__________"s +
                        "_XXX X____"s +
                        "_X_   ____"s +
                        "XXXXXXXXX_"s +
                        ""
                );
                const auto index = rotate180(field, piece, Spawn, Reverse, 4, 1);
                EXPECT_EQ(index, -1);
            }
        }

        TEST_F(SRSPlusRotationTest, case_t_180_from_right) {
            auto factory = Factory::createForSSRPlus();
            auto &piece = factory.get(T);

            constexpr auto bias = 6;

            {
                const auto field = createField(""s +
                        "____ _____"s +
                        "____  ____"s +
                        "____ _____"s +
                        "XXXXXXXXX_"s +
                        ""
                );
                const auto index = rotate180(field, piece, Right, Left, 4, 2);
                EXPECT_EQ(index - bias, 0);
            }
            {
                const auto field = createField(""s +
                        "___X _____"s +
                        "___X  ____"s +
                        "___X _____"s +
                        "XXXXXXXXX_"s +
                        ""
                );
                const auto index = rotate180(field, piece, Right, Left, 4, 2);
                EXPECT_EQ(index - bias, 1);
            }
            {
                const auto field = createField(""s +
                        "___X______"s +
                        "___X______"s +
                        "___X _____"s +
                        "___X  X___"s +
                        "___X XX___"s +
                        "XXXXXXXXX_"s +
                        ""
                );
                const auto index = rotate180(field, piece, Right, Left, 4, 2);
                EXPECT_EQ(index - bias, 2);
            }
            {
                const auto field = createField(""s +
                        "___XXXX___"s +
                        "___X______"s +
                        "___X _____"s +
                        "___X  X___"s +
                        "___X XX___"s +
                        "XXXXXXXXX_"s +
                        ""
                );
                const auto index = rotate180(field, piece, Right, Left, 4, 2);
                EXPECT_EQ(index - bias, 3);
            }
            {
                const auto field = createField(""s +
                        "_____X____"s +
                        "_____X____"s +
                        "____ X____"s +
                        "___X  ____"s +
                        "___X X____"s +
                        "XXXXXXXXX_"s +
                        ""
                );
                const auto index = rotate180(field, piece, Right, Left, 4, 2);
                EXPECT_EQ(index - bias, 4);
            }
            {
                const auto field = createField(""s +
                        "___XXX____"s +
                        "_____X____"s +
                        "____ X____"s +
                        "___X  ____"s +
                        "___X X____"s +
                        "XXXXXXXXX_"s +
                        ""
                );
                const auto index = rotate180(field, piece, Right, Left, 4, 2);
                EXPECT_EQ(index - bias, 5);
            }
            {
                const auto field = createField(""s +
                        "___XXX____"s +
                        "_____X____"s +
                        "___X X____"s +
                        "___X  ____"s +
                        "___X X____"s +
                        "XXXXXXXXX_"s +
                        ""
                );
                const auto index = rotate180(field, piece, Right, Left, 4, 2);
                EXPECT_EQ(index, -1);
            }
        }

        TEST_F(SRSPlusRotationTest, case_t_180_from_reverse) {
            auto factory = Factory::createForSSRPlus();
            auto &piece = factory.get(T);

            constexpr auto bias = 12;

            {
                const auto field = createField(""s +
                        "__________"s +
                        "__________"s +
                        "___   ____"s +
                        "XXXX XXXX_"s +
                        ""
                );
                const auto index = rotate180(field, piece, Reverse, Spawn, 4, 1);
                EXPECT_EQ(index - bias, 0);
            }
            {
                const auto field = createField(""s +
                        "__________"s +
                        "XXXXXX____"s +
                        "___   ____"s +
                        "____ _____"s +
                        ""
                );
                const auto index = rotate180(field, piece, Reverse, Spawn, 4, 1);
                EXPECT_EQ(index - bias, 1);
            }
            {
                const auto field = createField(""s +
                        "__________"s +
                        "XXXXXXX___"s +
                        "___   X___"s +
                        "____ XX___"s +
                        ""
                );
                const auto index = rotate180(field, piece, Reverse, Spawn, 4, 1);
                EXPECT_EQ(index - bias, 2);
            }
            {
                const auto field = createField(""s +
                        "__________"s +
                        "XXXXXX____"s +
                        "__X   ____"s +
                        "__XX _____"s +
                        ""
                );
                const auto index = rotate180(field, piece, Reverse, Spawn, 4, 1);
                EXPECT_EQ(index - bias, 3);
            }
            {
                const auto field = createField(""s +
                        "__________"s +
                        "__X_XXX___"s +
                        "___   X___"s +
                        "__XX XX___"s +
                        ""
                );
                const auto index = rotate180(field, piece, Reverse, Spawn, 4, 1);
                EXPECT_EQ(index - bias, 4);
            }
            {
                const auto field = createField(""s +
                        "__________"s +
                        "__XXX_X___"s +
                        "__X   ____"s +
                        "__XX XX___"s +
                        ""
                );
                const auto index = rotate180(field, piece, Reverse, Spawn, 4, 1);
                EXPECT_EQ(index - bias, 5);
            }
            {
                const auto field = createField(""s +
                        "__________"s +
                        "__XXX_X___"s +
                        "__X   X___"s +
                        "__XX XX___"s +
                        ""
                );
                const auto index = rotate180(field, piece, Reverse, Spawn, 4, 1);
                EXPECT_EQ(index, -1);
            }
        }

        TEST_F(SRSPlusRotationTest, case_t_180_from_left) {
            auto factory = Factory::createForSSRPlus();
            auto &piece = factory.get(T);

            constexpr auto bias = 18;

            {
                const auto field = createField(""s +
                        "_____ ____"s +
                        "____  ____"s +
                        "_____ ____"s +
                        "XXXXXXXXX_"s +
                        ""
                );
                const auto index = rotate180(field, piece, Left, Right, 5, 2);
                EXPECT_EQ(index - bias, 0);
            }
            {
                const auto field = createField(""s +
                        "_____ X___"s +
                        "____  X___"s +
                        "_____ X___"s +
                        "XXXXXXXXX_"s +
                        ""
                );
                const auto index = rotate180(field, piece, Left, Right, 5, 2);
                EXPECT_EQ(index - bias, 1);
            }
            {
                const auto field = createField(""s +
                        "______X___"s +
                        "______X___"s +
                        "_____ X___"s +
                        "___X  X___"s +
                        "___XX X___"s +
                        "XXXXXXXXX_"s +
                        ""
                );
                const auto index = rotate180(field, piece, Left, Right, 5, 2);
                EXPECT_EQ(index - bias, 2);
            }
            {
                const auto field = createField(""s +
                        "___XXXX___"s +
                        "______X___"s +
                        "_____ X___"s +
                        "___X  X___"s +
                        "___XX X___"s +
                        "XXXXXXXXX_"s +
                        ""
                );
                const auto index = rotate180(field, piece, Left, Right, 5, 2);
                EXPECT_EQ(index - bias, 3);
            }
            {
                const auto field = createField(""s +
                        "____X_____"s +
                        "____X_____"s +
                        "____X X___"s +
                        "____  X___"s +
                        "____X X___"s +
                        "XXXXXXXXX_"s +
                        ""
                );
                const auto index = rotate180(field, piece, Left, Right, 5, 2);
                EXPECT_EQ(index - bias, 4);
            }
            {
                const auto field = createField(""s +
                        "____XXX___"s +
                        "____X_____"s +
                        "____X ____"s +
                        "____  X___"s +
                        "____X X___"s +
                        "XXXXXXXXX_"s +
                        ""
                );
                const auto index = rotate180(field, piece, Left, Right, 5, 2);
                EXPECT_EQ(index - bias, 5);
            }
            {
                const auto field = createField(""s +
                        "____XXX___"s +
                        "____X_____"s +
                        "____X X___"s +
                        "____  X___"s +
                        "____X X___"s +
                        "XXXXXXXXX_"s +
                        ""
                );
                const auto index = rotate180(field, piece, Left, Right, 5, 2);
                EXPECT_EQ(index, -1);
            }
        }

        TEST_F(SRSPlusRotationTest, case_o_180_from_spawn) {
            auto factory = Factory::createForSSRPlus();
            auto &piece = factory.get(O);

            constexpr auto bias = 0;

            {
                const auto field = createField(""s +
                        "___XXXX___"s +
                        "___X  X___"s +
                        "___X  X___"s +
                        "XXXXXXXXX_"s +
                        ""
                );
                const auto index = rotate180(field, piece, Spawn, Reverse, 4, 1);
                EXPECT_EQ(index - bias, 0);
            }
        }

        TEST_F(SRSPlusRotationTest, case_o_180_from_right) {
            auto factory = Factory::createForSSRPlus();
            auto &piece = factory.get(O);

            constexpr auto bias = 6;

            {
                const auto field = createField(""s +
                        "___XXXX___"s +
                        "___X  X___"s +
                        "___X  X___"s +
                        "XXXXXXXXX_"s +
                        ""
                );
                const auto index = rotate180(field, piece, Right, Left, 4, 2);
                EXPECT_EQ(index - bias, 0);
            }
        }

        TEST_F(SRSPlusRotationTest, case_o_180_from_reverse) {
            auto factory = Factory::createForSSRPlus();
            auto &piece = factory.get(O);

            constexpr auto bias = 12;

            {
                const auto field = createField(""s +
                        "___XXXX___"s +
                        "___X  X___"s +
                        "___X  X___"s +
                        "XXXXXXXXX_"s +
                        ""
                );
                const auto index = rotate180(field, piece, Reverse, Spawn, 5, 2);
                EXPECT_EQ(index - bias, 0);
            }
        }

        TEST_F(SRSPlusRotationTest, case_o_180_from_left) {
            auto factory = Factory::createForSSRPlus();
            auto &piece = factory.get(O);

            constexpr auto bias = 18;

            {
                const auto field = createField(""s +
                        "___XXXX___"s +
                        "___X  X___"s +
                        "___X  X___"s +
                        "XXXXXXXXX_"s +
                        ""
                );
                const auto index = rotate180(field, piece, Left, Right, 5, 1);
                EXPECT_EQ(index - bias, 0);
            }
        }

        TEST_F(SRSPlusRotationTest, case_i_180_from_spawn) {
            auto factory = Factory::createForSSRPlus();
            auto &piece = factory.get(I);

            constexpr auto bias = 0;

            {
                const auto field = createField(""s +
                        "__XXXXXX__"s +
                        "__X    X__"s +
                        "__XXXXXX__"s +
                        ""
                );
                const auto index = rotate180(field, piece, Spawn, Reverse, 4, 1);
                EXPECT_EQ(index - bias, 0);
            }
        }

        TEST_F(SRSPlusRotationTest, case_i_180_from_right) {
            auto factory = Factory::createForSSRPlus();
            auto &piece = factory.get(I);

            constexpr auto bias = 6;

            {
                const auto field = createField(""s +
                        "____XXX___"s +
                        "____X X___"s +
                        "____X X___"s +
                        "____X X___"s +
                        "____X X___"s +
                        "XXXXXXXXX_"s +
                        ""
                );
                const auto index = rotate180(field, piece, Right, Left, 5, 3);
                EXPECT_EQ(index - bias, 0);
            }
        }

        TEST_F(SRSPlusRotationTest, case_i_180_from_reverse) {
            auto factory = Factory::createForSSRPlus();
            auto &piece = factory.get(I);

            constexpr auto bias = 12;

            {
                const auto field = createField(""s +
                        "__XXXXXX__"s +
                        "__X    X__"s +
                        "__XXXXXX__"s +
                        ""
                );
                const auto index = rotate180(field, piece, Reverse, Spawn, 5, 1);
                EXPECT_EQ(index - bias, 0);
            }
        }

        TEST_F(SRSPlusRotationTest, case_i_180_from_left) {
            auto factory = Factory::createForSSRPlus();
            auto &piece = factory.get(I);

            constexpr auto bias = 18;

            {
                const auto field = createField(""s +
                        "____XXX___"s +
                        "____X X___"s +
                        "____X X___"s +
                        "____X X___"s +
                        "____X X___"s +
                        "XXXXXXXXX_"s +
                        ""
                );
                const auto index = rotate180(field, piece, Left, Right, 5, 2);
                EXPECT_EQ(index - bias, 0);
            }
        }
    }
}
