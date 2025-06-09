#pragma once

#include <stdint.h>
#include <iostream>
#include <algorithm>
#include <cstring>

typedef uint8_t lfchar;

namespace LRDF
{
	template<uint8_t C, uint8_t F>
	class Langford
	{
	private:
		lfchar state[C * F];

	public:
		inline Langford() noexcept
		{
			std::memset(state, 0, getLength() * sizeof(lfchar));
		}
		inline Langford(const Langford<C, F>& other) noexcept
		{
			std::memcpy(state, other.state, getLength() * sizeof(lfchar));
		}

		~Langford() = default;

		Langford<C, F> duplicate() const noexcept
		{
			return Langford<C, F>(*this);
		}

		inline constexpr uint8_t getFrequency() const noexcept
		{
			return F;
		}
		inline constexpr uint8_t getCount() const noexcept
		{
			return C;
		}
		inline constexpr uint16_t getLength() const noexcept
		{
			return C * F;
		}

		inline lfchar& operator[](std::size_t id) noexcept
		{
			return state[id];
		}
		inline const lfchar& operator[](std::size_t id) const noexcept
		{
			return state[id];
		}
	};
}

template<uint8_t C, uint8_t F>
std::ostream& operator<<(std::ostream& out, const LRDF::Langford<C, F>& state)
{
	for (std::size_t i = 0; i < state.getLength(); i++)
	{
		if (i != 0)
			out << ' ';
		out << (unsigned int)state[i];
	}

	return out;
}