// Copyright© 2011, Viatronix Inc., All Rights Reserved.
//
// This is PROPRIETARY SOURCE CODE of Viatronix Inc. The contents of this file
// may not be disclosed to third parties, copied or duplicated in any form, 
// in whole or in part, without the prior written permission of Viatronix Inc. 
// 
// Owner: Mark Kontak   mkontak@viatronix.net

#pragma once

#include <functional>

/**
 * A simple scope guard, alternative of BOOST_SCOPE_EXIT, that uses RAII practices to cleanup resources.
 *
 * Use provided macro, SCOPE_EXIT, to implement scope_exit_t in code.
 * ex. 
 *     SCOPE_EXIT 
 *     { 
 *       // cleanup resources here
 *     }; 
 */
class scope_exit_t
{
	typedef std::function<void()> func_t;

public:

  // constructor accepts on rvalues
	scope_exit_t(func_t &&f) : func(f) {}

  // destructor
	~scope_exit_t() { func(); }

private:
	// Prohibit construction from lvalues.
	scope_exit_t(func_t &);

	// Prohibit copying.
	scope_exit_t(const scope_exit_t&);
	const scope_exit_t &operator=(const scope_exit_t &);

	// Prohibit new/delete.
	void *operator new(size_t);
	void *operator new[](size_t);
	void operator delete(void *);
	void operator delete[](void *);

	const func_t func;
};

#define SCOPE_EXIT_CAT2(x, y) x##y
#define SCOPE_EXIT_CAT1(x, y) SCOPE_EXIT_CAT2(x, y)
#define SCOPE_EXIT scope_exit_t SCOPE_EXIT_CAT1(scope_exit_, __COUNTER__) = [&]
