#pragma once

#include <Langford.hpp>

namespace LRDF
{
	template <uint8_t C, uint8_t F>
	class Solution
	{
	public:
		using m_Langford = LRDF::Langford<C, F>;

	private:
		m_Langford solution;

	public:
		Solution() = default;

		inline Solution(const m_Langford& solutionState) noexcept :
			solution(solutionState)
		{

		}

		inline Solution(const Solution<C, F>& other) noexcept :
			solution(other.solution)
		{

		}

		inline const m_Langford& getSolution() const noexcept
		{
			return solution;
		}
	};
}