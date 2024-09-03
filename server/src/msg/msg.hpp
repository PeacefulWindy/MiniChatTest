#pragma once
#include<memory>
#include<vector>
#include<string>

enum class MsgType
{
	Request=1,
	Response=2,
	Socket=3,
};

class Msg
{
public:
	uint32_t getFromServiceId();
	void setFromServiceId(uint32_t value);

public:
	uint32_t getToServiceId();
	void setToServiceId(uint32_t value);

public:
	int32_t getProtocol();
	void setProtocol(int32_t value);

public:
	std::string getCmd();
	void setCmd(std::string value);

public:
	uint32_t getSessionId();
	void setSessionId(uint32_t value);

public:
	std::shared_ptr<std::vector<uint8_t>> getData()const;
	void setData(std::shared_ptr<std::vector<uint8_t>> value);

private:
	uint32_t mFromServiceId = 0;
	uint32_t mFromNodeId = 0;
	uint32_t mServiceId = 0;
	std::string mCmd = "";
	uint32_t mSessionId = 0;
	int32_t mProtocol = 0;
	std::shared_ptr<std::vector<uint8_t>> mData;
};