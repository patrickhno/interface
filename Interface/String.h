
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#ifndef STRING_H
#define STRING_H

class String{
public:				String(){ hook=-1; }
					String(const String &str);
					String(const char *str);
					~String();
		String		&operator=(const String &);
		bool		operator==(const String &) const;
		bool		operator!=(const String &) const;
		String		&operator+=(String &);
const	char		*GetString(void) const; //const int);
//const	char		*DuplicateString(void) const;
private:
		int			HookString(const char *);
		void		UnhookString(const int);
		int			hook;
		char		*string;
};

inline String::String(const char *str){
	hook = HookString(str);
}

inline String::~String(){
	UnhookString(hook);
}

inline bool String::operator==(const String &str) const{
	if(hook<0||str.hook<0) return false;
//printf("%s==%s\n",GetString(),((String&)str).GetString());
	return(hook==str.hook);
}

inline bool String::operator!=(const String &str) const{
	if(hook<0||str.hook<0) return false;
//printf("%s==%s\n",GetString(),((String&)str).GetString());
	return(hook!=str.hook);
}

#endif
