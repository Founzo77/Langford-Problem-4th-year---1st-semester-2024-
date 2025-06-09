#pragma once

#include <Langford.hpp>

namespace LRDF
{
	namespace Aruka
	{
		template <uint8_t C, uint8_t F>
		class Task
		{
		public:
			using m_Langford = Langford<C, F>;

		private:
			m_Langford state;
			uint8_t currentFrequency;
			bool isSymmetryVerification;

		public:
			Task() : state(), currentFrequency(F), isSymmetryVerification(0)
			{

			}
			Task(m_Langford state, uint8_t currentFrequency) : state(state),
				currentFrequency(currentFrequency), isSymmetryVerification(0)
			{

			}
			Task(m_Langford state, uint8_t currentFrequency, bool isSymmetryVerification) : state(state),
				currentFrequency(currentFrequency), isSymmetryVerification(isSymmetryVerification)
			{

			}
			Task(const Task& other) : state(other.state), currentFrequency(other.currentFrequency), isSymmetryVerification(other.isSymmetryVerification)
			{

			}

			LRDF::Aruka::Task<C, F> operator=(
				const LRDF::Aruka::Task<C, F>& other)
			{
				if (this == &other)
					return *this;

				state = other.state;
				currentFrequency = other.currentFrequency;
				isSymmetryVerification = other.isSymmetryVerification;

				return *this;
			}

			inline const m_Langford& getState() const noexcept
			{
				return state;
			}

			inline uint8_t getCurrentFrequency() const noexcept
			{
				return currentFrequency;
			}

			inline bool getIsSymmetryVerification() const noexcept
			{
				return isSymmetryVerification;
			}
		};
	}
}