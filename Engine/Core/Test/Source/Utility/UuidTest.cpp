/*
* MIT License
*
* Copyright (c) 2022 Jimmie Bergmann
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files(the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions :
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*
*/

#include "Test.hpp"
#include "Molten/Utility/Uuid.hpp"
#include <random>
#include <set>


namespace Molten
{
    TEST(Utility, Uuid)
    {
        {
            const auto uuid = Uuid{};
            EXPECT_STREQ(ToString(uuid).c_str(), "00000000-0000-0000-0000-000000000000");
            EXPECT_EQ(uuid.GetVersion(), uint8_t{ 0 });
            EXPECT_LE(uuid.GetVariant(), uint8_t{ 0 });
        }
        {
            const uint64_t low = 0xcdef90ab12345678ULL;
            const uint64_t high = 0xfedcba0987654321ULL;

            const auto uuid = Uuid{ low, high };

            EXPECT_STREQ(ToString(uuid).c_str(), "78563412-ab90-efcd-2143-658709badcfe");
            EXPECT_EQ(uuid.GetVersion(), uint8_t{ 14 });
            EXPECT_LE(uuid.GetVariant(), uint8_t{ 2 });
        }
        {
            const uint32_t low1 = 0x12345678U;
            const uint16_t low2 = 0x90abU;
            const uint16_t low3 = 0xcdefU;
            const uint64_t high = 0xfedcba0987654321ULL;

            const auto uuid = Uuid{ low1, low2, low3, high };

            EXPECT_STREQ(ToString(uuid).c_str(), "78563412-ab90-efcd-2143-658709badcfe");
            EXPECT_EQ(uuid.GetVersion(), uint8_t{ 14 });
            EXPECT_LE(uuid.GetVariant(), uint8_t{ 2 });
        }
        {
            const std::array<uint8_t, 16> data = { 
                0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef, 
                0xfe, 0xdc, 0xba, 0x09, 0x87, 0x65, 0x43, 0x21
            };

            const auto uuid = Uuid{ data };

            EXPECT_STREQ(ToString(uuid).c_str(), "12345678-90ab-cdef-fedc-ba0987654321");
            EXPECT_EQ(uuid.GetVersion(), uint8_t{ 12 });
            EXPECT_LE(uuid.GetVariant(), uint8_t{ 15 });
        }
    }

    TEST(Utility, Uuid_GenerateVersion4)
    {
        std::random_device randomDevice;
        std::mt19937 randomEngine(randomDevice());

        std::set<std::string> uuids;

        for(size_t i = 0; i < 10; i++)
        {
            Uuid uuid = Uuid::GenerateVersion4(randomEngine);
            
            EXPECT_EQ(uuid.GetVersion(), uint8_t{ 4 });
            EXPECT_GE(uuid.GetVariant(), uint8_t{ 8 });
            EXPECT_LE(uuid.GetVariant(), uint8_t{ 11 });

            const std::string uuidString = ToString(uuid);

            ASSERT_EQ(uuidString.size(), size_t{ 36 });
            EXPECT_STRNE(uuidString.c_str(), "00000000-0000-0000-0000-000000000000");       
            EXPECT_EQ(uuidString[14], '4');
            EXPECT_GE(uuidString[19], '8');
            EXPECT_LE(uuidString[19], 'b');

            Molten::Test::PrintInfo(uuidString);

            uuids.insert(uuidString);
        }

        EXPECT_EQ(uuids.size(), size_t{ 10 });
    }

    TEST(Utility, Uuid_Operator_Compare)
    {
        {
            const auto uuid1 = Uuid{};
            const auto uuid2 = Uuid{};
            EXPECT_EQ(uuid1, uuid2);
        }
        {
            std::random_device randomDevice;
            std::mt19937 randomEngine(randomDevice());

            const auto uuid1 = Uuid::GenerateVersion4(randomEngine);
            const auto uuid2 = Uuid::GenerateVersion4(randomEngine);

            EXPECT_EQ(uuid1, uuid1);
            EXPECT_EQ(uuid2, uuid2);
            EXPECT_NE(uuid1, uuid2);
        }
    }

    TEST(Utility, Uuid_FromString)
    {
        {
            const uint32_t low1 = 0x12345678U;
            const uint16_t low2 = 0x90abU;
            const uint16_t low3 = 0xcdefU;
            const uint64_t high = 0xfedcba0987654321ULL;

            const auto uuid = Uuid{ low1, low2, low3, high };

            auto fromString = FromString<Uuid>("78563412-ab90-efcd-2143-658709badcfe");
            ASSERT_TRUE(fromString.HasValue());
            EXPECT_EQ(fromString.Value(), uuid);

            EXPECT_STREQ(ToString(fromString.Value()).c_str(), "78563412-ab90-efcd-2143-658709badcfe");
        }
        {
            const std::array<uint8_t, 16> data = {
               0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef,
               0xfe, 0xdc, 0xba, 0x09, 0x87, 0x65, 0x43, 0x21
            };

            const auto uuid = Uuid{ data };

            auto fromString = FromString<Uuid>("12345678-90ab-cdef-fedc-ba0987654321");
            ASSERT_TRUE(fromString.HasValue());
            EXPECT_EQ(fromString.Value(), uuid);

            EXPECT_STREQ(ToString(fromString.Value()).c_str(), "12345678-90ab-cdef-fedc-ba0987654321");
        }
        {
            EXPECT_FALSE(FromString<Uuid>("").HasValue());
            EXPECT_FALSE(FromString<Uuid>(" ").HasValue());
            EXPECT_FALSE(FromString<Uuid>("\t").HasValue());
            EXPECT_FALSE(FromString<Uuid>("-").HasValue());
            EXPECT_FALSE(FromString<Uuid>("123x5678-90ab-cdef-fedc-ba0987654321").HasValue());
            EXPECT_FALSE(FromString<Uuid>("123x5678-90ab-cdef-fedc-ba098765432K").HasValue());
            EXPECT_FALSE(FromString<Uuid>("1234567-890ab-cdef-fedc-ba0987654321").HasValue());
            EXPECT_FALSE(FromString<Uuid>("12345678-90a-bcdef-fedc-ba0987654321").HasValue());
        }
    }

}