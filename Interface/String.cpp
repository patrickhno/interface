
// Copyright (C) 1997-1998 Reality Systems as
// All rights reserved.

#include <posix/stdio.h>
#include <Interface/String.h>
#include <../include/string.h>
#include <assert.h>

struct Hook{
	int		count;
	char	*string;
};

Hook *string_hooks = 0;
int *string_masksize = 0;
long *string_mask;

String::String(const String &str){
	string_hooks[str.hook].count++;
	hook = str.hook;
string = string_hooks[hook].string;
}

String &String::operator=(const String &str){
	string_hooks[str.hook].count++;
	hook = str.hook;
string = string_hooks[hook].string;
	return *this;
}

String &String::operator+=(String &str){
	const char *ts = GetString();
	const char *fs = str.GetString();
	int len = strlen(ts) + strlen(fs);
	char *cstr = new char[len+1];
	sprintf(cstr,"%s%s",ts,fs);
	UnhookString(hook);
	hook = HookString(cstr);
string = string_hooks[hook].string;
	delete cstr;
	return *this;
}


int String::HookString(const char *str){
	if(!string_hooks){
		string_hooks = new Hook[0x1000];
		string_masksize = new int;
		*string_masksize = 0x1000;
		string_mask = new long[0x1000/32];
		memset(string_mask,-1,0x1000/8);
	}
	// search for hook
	for(int n=0; n<*string_masksize/32; n++){
		int maskf = string_mask[n];
		if(maskf!=0xffffffff){
			int bits = 0;
//			if(maskf&0xffff0000){
//				if(maskf&0xff000000) bits = 0;
//				else bits = 8;
//			}else{
//				if(maskf&0xff00) bits = 16;
//				else bits = 24;
//			}
			int b;
			for(b=bits, bits+=32; b<bits; b++){
				if(!(maskf&0x80000000>>b)){
					if(!strcmp(string_hooks[n*32+b].string,str)){
						string_hooks[n*32+b].count++;
string = string_hooks[n*32+b].string;
						return n*32+b;
					}
				}
			}
		}
	}
	//
	for(n=0; n<*string_masksize/32; n++){
		int maskb = string_mask[n];
		if(maskb){
			int bits = 0;
//			if(maskb&0xffff0000){
//				if(maskb&0xff000000) bits = 0;
//				else bits = 8;
//			}else{
//				if(maskb&0xff00) bits = 16;
//				else bits = 24;
//			}
			int b;
			for(b=bits,bits+=32; b<bits; b++){
				if(maskb&(0x80000000>>b)){
					string_mask[n]&=~(0x80000000>>b);
					int hook = n*32+b;
					string_hooks[hook].string = new char[strlen(str)+1];
					sprintf(string_hooks[hook].string,"%s",str);
					string_hooks[hook].count = 1;
string = string_hooks[hook].string;
					return hook;
				}
			}
		}
	}
	assert(false);
	return 0;
}

void String::UnhookString(const int hook){
	string_hooks[hook].count--;
	if(!string_hooks[hook].count){
		delete string_hooks[hook].string;
		string_mask[hook/32]|=0x80000000>>(hook&0x1f);
	}
}

const char *String::GetString(void) const{ //const int hook){
//printf("%s\n",hooks[hook].string);
	return string_hooks[hook].string;
}

//const char *String::DuplicateString(void) const{
//	return hooks[hook];
//}

