#include<hv/hloop.h>
#include<spdlog/spdlog.h>
#include<service/serviceMgr.hpp>
#include<queue/threadQueue.hpp>
#include<hv/EventLoop.h>
#include<lua/luaApi.hpp>

#define THREAD_NUM 2

hloop_t* GlobalLoop;

ThreadQueue<std::shared_ptr<Msg>> GlobalMsgQueue;

std::vector<std::thread> ServiceThreads;

void serviceEventLoop()
{
	auto serviceMgr = ServiceMgr::getInstance();
	while (hloop_status(GlobalLoop) == hloop_status_e::HLOOP_STATUS_RUNNING)
	{
		serviceMgr->poll();
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
}

void mainEventLoop(hevent_t* ev)
{
	auto serviceMgr = ServiceMgr::getInstance();

	if (GlobalMsgQueue.getSize() > 0)
	{
		std::map<uint32_t, std::list<std::shared_ptr<Msg>>> msgs;
		GlobalMsgQueue.lock();

		while (GlobalMsgQueue.getSize() > 0)
		{
			auto msg = GlobalMsgQueue.getFront();
			auto serviceId = msg->getToServiceId();

			auto iter = msgs.find(serviceId);
			if (iter == msgs.end())
			{
				msgs.insert({ serviceId,std::list<std::shared_ptr<Msg>>() });
				iter = msgs.find(serviceId);
			}

			iter->second.emplace_back(msg);
			GlobalMsgQueue.pop();
		}

		GlobalMsgQueue.unlock();

		for (auto it = msgs.begin(); it != msgs.end(); ++it)
		{
			serviceMgr->pushMsgs(it->first, it->second);
		}
	}

	hloop_post_event(GlobalLoop, ev);
}

int main(int argc, char** argv)
{
#ifdef WIN32
	system("chcp 65001");
#endif

	GlobalLoop = hloop_new(0);

	auto state = luaL_newstate();
	luaL_openlibs(state);
	luaRegisterAPI(state);

	if (luaL_dofile(state, "Resources/main.lua") != LUA_OK)
	{
		spdlog::error("{}", lua_tostring(state, -1));
		return -1;
	}

	lua_close(state);

	auto ev = new hevent_t();
	ev->event_type = (hevent_type_e)(HEVENT_TYPE_CUSTOM);
	ev->cb = mainEventLoop;
	ev->userdata = ev;
	hloop_post_event(GlobalLoop, ev);

	for (auto i = 0; i < THREAD_NUM; i++)
	{
		ServiceThreads.push_back(std::thread(serviceEventLoop));
	}

	hloop_status(GlobalLoop);
	hloop_run(GlobalLoop);
	delete ev;

	for (auto it = ServiceThreads.begin(); it != ServiceThreads.end(); ++it)
	{
		it->join();
	}

	hloop_free(&GlobalLoop);
}