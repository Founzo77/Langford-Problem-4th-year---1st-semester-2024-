#pragma once

#include <cstddef>
#include <memory>
#include <iostream>

#include <list>
#include <string>
#include <array>
#include <scoped_allocator>

namespace LRDF
{
	namespace Aruka
	{
		template<typename T, typename Allocator, typename IMemoryTracker>
		class WrapperAllocator
		{
		public:
			using value_type = typename std::allocator_traits<Allocator>::value_type;
			using difference_type = typename std::allocator_traits<Allocator>::difference_type;
			using size_type = typename std::allocator_traits<Allocator>::size_type;
			using pointer = typename std::allocator_traits<Allocator>::pointer;
			using const_pointer = typename std::allocator_traits<Allocator>::const_pointer;
			using propagate_on_container_move_assignment = typename std::allocator_traits<Allocator>::propagate_on_container_move_assignment;

			using m_MemoryTracker = IMemoryTracker;
			using m_Allocator = Allocator;

			template <class Other>
			struct rebind {
				using other = WrapperAllocator<
					Other,
					typename std::allocator_traits<Allocator>::template rebind_alloc<Other>,
					IMemoryTracker
				>;
			};

		private:
			m_Allocator allocator;
			m_MemoryTracker tracker;

		public:
			WrapperAllocator() : allocator(), tracker()
			{

			}

			WrapperAllocator(const WrapperAllocator& other) noexcept :
				allocator(), tracker()
			{

			}

			template<typename U>
			WrapperAllocator(const WrapperAllocator<U,
				typename std::allocator_traits<Allocator>::template rebind_alloc<U>, IMemoryTracker>& other) noexcept :
				allocator(), tracker()
			{

			}

			pointer allocate(std::size_t n)
			{
				pointer p = std::allocator_traits<Allocator>::allocate(allocator, n);
				tracker.allocated(n * sizeof(value_type), (const void*)p);
				return p;
			}

			void deallocate(pointer p, std::size_t n)
			{
				tracker.freed((const void*)p);
				std::allocator_traits<Allocator>::deallocate(allocator, p, n);
			}

			template<typename U, typename... Args>
			void construct(U* p, Args&&... args)
			{
				std::allocator_traits<Allocator>::construct(
					allocator, p, std::forward<Args>(args)...);
			}

			template<typename U>
			void destroy(U* p)
			{
				std::allocator_traits<Allocator>::destroy(allocator, p);
			}

			bool operator==(const WrapperAllocator& other) const
			{
				return allocator == other.allocator;
			}

			bool operator!=(const WrapperAllocator& other) const
			{
				return !(*this == other);
			}

			inline const m_MemoryTracker& getMemoryTracker() const
			{
				return tracker;
			}
		};
	}
}