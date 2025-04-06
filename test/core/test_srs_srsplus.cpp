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
    }
}
