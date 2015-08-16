#pragma once
#include "APIMain.h"



namespace GWAPI {

	class ChatMgr{
		friend class GWAPIMgr;
		GWAPIMgr* parent;
		ChatMgr(GWAPIMgr* obj) : parent(obj) {}

		struct P5E_SendChat{
			const DWORD header = 0x5E;
			wchar_t channel;
			wchar_t msg[137];
		};

	public:
		
		// Sendchat, should be self explanatory. SendChat(L"I love gwtoolbox",L'!');
		void SendChat(wchar_t* msg, wchar_t channel);

		// Write to chat as a PM with printf style arguments.
		void WriteToChat(wchar_t* format, ...);

	};

}