#ifndef LOCKABLEVAR_H
#define LOCKABLEVAR_H

#include <Locker.h>

class LockableBool
{
public:
								LockableBool(const bool &value = false);
		virtual					~LockableBool(void);
						
		LockableBool &	operator=(const bool &from);
		bool 			operator==(const bool &from);
		bool 			operator!=(const bool &from);
				
		void			SetValue(const bool &value);
		bool			GetValue(void);

private:
		bool	fValue;
		BLocker	fLock;
};

#endif
