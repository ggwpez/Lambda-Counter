#pragma once

#define tem template
#define typ typename

namespace std
{
tem<bool Cond1, bool Cond2, typ t, typ ft, typ ff> struct conditional23;

tem<bool Cond2, typ t, typ ft, typ ff> struct conditional23<true, Cond2, t, ft, ff>
{
	typedef t type;
};

tem<typ t, typ ft, typ ff> struct conditional23<false, true, t, ft, ff>
{
	typedef ft type;
};

tem<typ t, typ ft, typ ff> struct conditional23<false, false, t, ft, ff>
{
	typedef ff type;
};

tem<bool Cond1, bool Cond2, typ t, typ ft, typ ff> using conditional23_t =
	typename conditional23<Cond1, Cond2, t, ft, ff>::type;
} // namespace std
