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

#ifndef MOLTEN_CORE_UTILITY_UUID_HPP
#define MOLTEN_CORE_UTILITY_UUID_HPP

#include "Molten/System/Result.hpp"
#include "Molten/Utility/StringConvert.hpp"
#include "Molten/Utility/StringUtility.hpp"
#include <array>
#include <string>

namespace Molten
{

	struct MOLTEN_API Uuid
	{
		Uuid() = default;

		explicit Uuid(const std::array<uint8_t, 16>& data);
		
		Uuid(
			const uint64_t low,
			const uint64_t high);

		Uuid(
			const uint32_t data1,
			const uint16_t data2,
			const uint16_t data3,
			const uint64_t data4);

		template<typename TRandomEngine>
		[[nodiscard]] static Uuid GenerateVersion4(TRandomEngine& randomEngine);
		
		[[nodiscard]] uint8_t GetVersion() const;
		[[nodiscard]] uint8_t GetVariant() const;

		[[nodiscard]] bool operator == (const Uuid& rhs) const;
		[[nodiscard]] bool operator != (const Uuid& rhs) const;
		/*[[nodiscard]] bool operator < (const Uuid& rhs) const;
		[[nodiscard]] bool operator <= (const Uuid& rhs) const;
		[[nodiscard]] bool operator > (const Uuid& rhs) const;
		[[nodiscard]] bool operator >= (const Uuid& rhs) const;*/

		MOLTEN_ANONYMOUS_STRUCTURE_BEGIN
		union
		{
			struct
			{
				union
				{
					struct
					{
						uint32_t Low1;
						uint16_t Low2;
						uint16_t Low3;
					};
					uint64_t Low;

				};
				uint64_t High;

			};
			std::array<uint8_t, 16> Data;
		};
		MOLTEN_ANONYMOUS_STRUCTURE_END
	};


	/** Convert UUID to string. */
	template<>
	[[nodiscard]] std::string ToString(const Uuid& value);

	/** Convert string to UUID. */
	template<>
	[[nodiscard]] Result<Uuid, size_t> FromString(const std::string_view value);

}

#include "Molten/Utility/Uuid.inl"

#endif