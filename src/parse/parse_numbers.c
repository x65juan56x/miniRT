#include <stddef.h>
#include "../../include/parser_internal.h"

static void	skip_sign(const char **p, int *neg)
{
	if (**p == '+')
		(*p)++;
	else if (**p == '-')
	{
		*neg = 1;
		(*p)++;
	}
}
/*
* Purpose: Advance past an optional leading '+' or '-' and flag negativity.
* Inputs: pointer to the current string cursor and an int tracking the sign.
* Notes: Updates the cursor in place; any non-sign character is left untouched.
*/

static int	parse_digits(const char **p, double *val)
{
	int	has;

	has = 0;
	while (**p >= '0' && **p <= '9')
	{
		*val = *val * 10.0 + (double)(**p - '0');
		(*p)++;
		has = 1;
	}
	return (has);
}
/*
* Purpose: Consume consecutive decimal digits and accumulate them into *val.
* Inputs: cursor pointer and accumulating double value.
* Returns: 1 if at least one digit was read; 0 otherwise.
*/

static int	parse_fraction(const char **p, double *val)
{
	double	scale;
	int		has;

	if (**p != '.')
		return (0);
	(*p)++;
	scale = 0.1;
	has = 0;
	while (**p >= '0' && **p <= '9')
	{
		*val = *val + ((double)(**p - '0')) * scale;
		scale = scale * 0.1;
		(*p)++;
		has = 1;
	}
	return (has);
}
/*
* Purpose: Parse the fractional part of a decimal number after a dot.
* Inputs: cursor pointer and accumulating double value.
* Behavior: Multiplies successive digits by decreasing powers of ten.
*/

bool	parse_float(const char *s, float *out)
{
	const char	*p;
	int			neg;
	double		val;
	int			ok;

	p = s;
	neg = 0;
	val = 0.0;
	ok = 0;
	skip_sign(&p, &neg);
	if (parse_digits(&p, &val))
		ok = 1;
	if (parse_fraction(&p, &val))
		ok = 1;
	if (!ok || *p != '\0')
		return (false);
	if (neg)
		val = -val;
	*out = (float)val;
	return (true);
}
/*
* Purpose: Convert a string representing a decimal float into a float output.
* Success: Returns true and writes the parsed value when the string is valid.
* Failure: Returns false if format is incorrect or extra characters remain.
*/

bool	parse_int_in_range(const char *s, int minv, int maxv, int *out)
{
	const char	*p;
	int			neg;
	long		value;

	p = s;
	neg = 0;
	value = 0;
	skip_sign(&p, &neg);
	if (*p < '0' || *p > '9')
		return (false);
	while (*p >= '0' && *p <= '9')
	{
		value = value * 10 + (long)(*p - '0');
		p++;
	}
	if (*p != '\0')
		return (false);
	if (neg)
		value = -value;
	if (value < (long)minv || value > (long)maxv)
		return (false);
	*out = (int)value;
	return (true);
}
/*
* Purpose: Parse an integer string and ensure it falls inside [minv, maxv].
* Success: Stores the bounded value in *out and returns true.
* Failure: Rejects malformed numbers or values outside the allowed interval.
*/
