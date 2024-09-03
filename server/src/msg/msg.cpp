#include"msg.hpp"

uint32_t Msg::getFromServiceId()
{
	return this->mFromServiceId;
}

void Msg::setFromServiceId(uint32_t value)
{
	this->mFromServiceId = value;
}

uint32_t Msg::getToServiceId()
{
	return this->mServiceId;
}

void Msg::setToServiceId(uint32_t value)
{
	this->mServiceId = value;
}

int32_t Msg::getProtocol()
{
	return this->mProtocol;
}

void Msg::setProtocol(int32_t value)
{
	this->mProtocol = value;
}

std::string Msg::getCmd()
{
	return this->mCmd;
}

void Msg::setCmd(std::string value)
{
	this->mCmd = value;
}

uint32_t Msg::getSessionId()
{
	return this->mSessionId;
}

void Msg::setSessionId(uint32_t value)
{
	this->mSessionId = value;
}

std::shared_ptr<std::vector<uint8_t>> Msg::getData() const
{
	return this->mData;
}

void Msg::setData(std::shared_ptr<std::vector<uint8_t>> value)
{
	this->mData = value;
}
