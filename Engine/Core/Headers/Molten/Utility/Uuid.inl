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

namespace Molten
{

	template<typename TRandomEngine>
	Uuid Uuid::GenerateVersion4(TRandomEngine& randomEngine)
	{
		std::uniform_int_distribution<uint64_t> randomDistribution(
			std::numeric_limits<uint64_t>::min(),
			std::numeric_limits<uint64_t>::max()); 

		const uint64_t low = static_cast<uint64_t>(randomDistribution(randomEngine) & 0xFF0FFFFFFFFFFFFFUL | 0x0040000000000000UL);
		const uint64_t high = static_cast<uint64_t>(randomDistribution(randomEngine) & 0xFFFFFFFFFFFFFF3FUL | 0x0000000000000080UL);

		return { low, high };
	}

	template<>
	inline std::string ToString(const Uuid& value)
	{
		auto hex = StringUtility::ToLowercaseHex<16>(value.Data);

		std::string result;
		result.reserve(36);

		result += std::string_view{ hex.data(), 8 };
		result += '-';
		result += std::string_view{ hex.data() + 8, 4 };
		result += '-';
		result += std::string_view{ hex.data() + 12, 4 };
		result += '-';
		result += std::string_view{ hex.data() + 16, 4 };
		result += '-';
		result += std::string_view{ hex.data() + 20, 12 };

		return result;
	}

	template<>
	inline Result<Uuid, size_t> FromString(const std::string_view input)
	{
		static const auto indices = std::array<size_t, 16>{
			0, 2, 4, 6,
			9, 11,
			14, 16,
			19, 21,
			24, 26, 28, 30, 32, 34
		};

		if (input.size() != 36 || input[8] != '-' || input[13] != '-' || input[18] != '-' || input[23] != '-')
		{
			return Result<Uuid, size_t>::CreateError(0);
		}

		auto getHexAsDecimal = [](const char c) -> uint8_t
		{
			return
				(c >= '0' && c <= '9') ?
					(c - '0') :
					(c >= 'a' && c <= 'f') ?
						(c - 'a' + 10) :
						(c >= 'A' && c <= 'F') ?
							(c - 'A' + 10) :
							255;
		};

		auto data = std::array<uint8_t, 16>{ };

		for (size_t i = 0; i < indices.size(); i++)
		{
			const auto high = getHexAsDecimal(input[indices[i]]);
			if (high == 255)
			{
				return Result<Uuid, size_t>::CreateError(size_t{ 0 });
			}

			const auto low = getHexAsDecimal(input[indices[i] + 1]);
			if (low == 255)
			{
				return Result<Uuid, size_t>::CreateError(size_t{ 0 + 1 });
			}

			data[i] = (high << 4) | low;
		}

		return Result<Uuid, size_t>::CreateSuccess(Uuid{ data });
	}

}