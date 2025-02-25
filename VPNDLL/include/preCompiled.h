#pragma once
#ifdef _WIN32
	#ifdef VPN_DLL_EXPORTS
		#define VPN_API __declspec(dllexport)  
	#else
		#define VPN_API __declspec(dllimport)  
	#endif
#else
	#define VPN_API  
#endif
