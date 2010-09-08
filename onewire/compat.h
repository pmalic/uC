#ifndef ONEWIRE_COMPAT_H
#define ONEWIRE_COMPAT_H

static inline void cli ()
{
#if defined(_LPC2100)
#endif
}

static inline void sei ()
{
#if defined(_LPC2100)
#endif
}

inline void delayMicroseconds (unsigned int us)
{
#if defined(_LPC2100)
			T0PR = 58982 / VPBDIV / 1000 - 1; // prescale divider
			T0TCR = 3; // reset counter
			T0IR = 0xff; // clear interrupts
			T0TCR = 1; // start counting

			while (T0TC < us)
				__asm volatile ( "nop;" );

			T0TCR = 2; // stop counting
#endif
}

inline void delay (unsigned int ms)
{
#if defined(_LPC2100)
			T0PR = 58982 / VPBDIV - 1; // prescale divider
			T0TCR = 3; // reset counter
			T0IR = 0xff; // clear interrupts
			T0TCR = 1; // start counting

			while (T0TC < ms)
				__asm volatile ( "nop;" );

			T0TCR = 2; // stop counting
#endif
}

#endif
