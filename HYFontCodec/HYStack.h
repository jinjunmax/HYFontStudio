#pragma once
#include <vector>
#include <math.h>

namespace HYFONTCODEC
{
	template <class T> int Trunc (T const &x)
	{
		return static_cast <int> (x);

	}	// end of int Trunc ()

	template <class T> int make_int32_t (T const &x)
	{
		int		ix = Trunc (x);

		if (T (ix) != x) return -1;
		return ix;

	}	// end of template <class T> int make_int32_t ()

	template <class T>size_t make_size_t (T const &x)
	{
		int		ix = Trunc (x);

		if (T (ix) != x || ix < 0) return 0;
		return static_cast <size_t> (ix);

	}	// end of template <class T>size_t make_size_t ()

	template <class T>
	class CHYStack
	{
	public:
		CHYStack(size_t maxelems) : fMaxElems (maxelems)
		{
			stack.reserve (fMaxElems);

		}	// end of CHYStack()

		size_t size() const
		{
			return stack.size();

		}	// end of size_t size() const

		T const & operator [] (size_t ix) const
		{
			return stack[ix];

		}	// end of T const & operator [] (size_t ix)

		void clear()
		{
			stack.clear();

		}	// end of  void clear()

		T &push (T const &x)
		{
			stack.push_back (x);
			return stack.back();

		}	// end of T &push ()

		T & back() const
		{
			return stack.back();

		}	// end of T & back() const

		T	pop()
		{
			T		result (stack.back());
			stack.pop_back();
			return result;

		}	// end of T	pop()

		void popN (size_t n)
		{
			if (n)
			{			
				stack.resize (stack.size() - n);
			}
		}	// end of void popN ()

		void drop()
		{
			stack.pop_back();

		}	// end of void drop()

		void exch()
		{	
			std::swap (stack [stack.size() - 1], stack [stack.size() - 2]);

		}	// end of void exch()

		T & index()
		{	
			int		ix = make_int32_t (pop());

			if (ix < 0)
				ix = 0;

			push (stack [stack.size() - 1 - ix]);
			return stack.back();

		}	// end of T & index()

		int modulo(int x, size_t mod)
		{
			while (x < 0)
				x += static_cast <int> (mod);

			while (x >= static_cast <int> (mod))
				x -= static_cast <int> (mod);

			return x;

		}	// end of int modulo()

		void roll()
		{
			int		j = make_int32_t (pop());
			size_t		n = make_size_t (pop());

			int		stackend = static_cast <int> (stack.size() - 1);

			for (int ix = 0; ix != static_cast <int> (n); ++ix)
				std::swap (stack [stackend - ix], stack [stackend - modulo (ix + j, n)]);


		}	// end of void roll()

		T &	dup()
		{
			stack.push_back (stack.back());
			return stack.back();

		}	// end of T & dup()

	protected:
		unsigned int			fMaxElems;
		std::vector<T>			stack;
	};
};

