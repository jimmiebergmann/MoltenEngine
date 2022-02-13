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

	// Buffer capacity scalar policy implementations.
	inline BufferCapacityScalarPolicy::BufferCapacityScalarPolicy(const size_t value) :
		m_value(std::max(value, size_t{ 1 }))
	{}

	inline size_t BufferCapacityScalarPolicy::operator()(size_t requestedSize, size_t oldCapacity) const
	{
		// Grow
		if(requestedSize > oldCapacity)
		{
			return ((requestedSize / m_value) + 1) * m_value;
		}

		// Shrink
		if(oldCapacity >= m_value && requestedSize < oldCapacity - m_value)
		{
			return std::max(m_value, ((requestedSize / m_value) + 1) * m_value);
		}

		return oldCapacity;
	}


	// Buffer capacity policy implementations.
	inline BufferCapacityPolicy::BufferCapacityPolicy(CapacityFunctor&& capacityFunctor) :
		m_capacity(0),
		m_size(0),
		m_capacityFunctor(std::move(capacityFunctor))
	{}

	inline bool BufferCapacityPolicy::SetSize(const size_t size)
	{
		const auto newCapacity = m_capacityFunctor(size, m_capacity);
		const auto result = newCapacity != m_capacity;

		m_size = size;
		m_capacity = newCapacity;

		return result;
	}

	inline size_t BufferCapacityPolicy::GetSize() const
	{
		return m_size;
	}

	inline void BufferCapacityPolicy::SetCapacity(const size_t capacity)
	{
		m_capacity = capacity;
	}

	inline size_t BufferCapacityPolicy::GetCapacity() const
	{
		return m_capacity;
	}

}