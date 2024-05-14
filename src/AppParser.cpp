#include "AppParser.h"

AppParser::Request* AppParser::ParseUartMessage(const uint8_t* msg, uint8_t msgLength)
{
	auto requestValue = new Request();

    for(uint8_t i = 0; i < msgLength; i++)
    {	
        const bool goOn = [&requestValue, msg, i]()
        {	
			if(static_cast<MessageStruct>(msg[i]) == MessageStruct::MESSAGE_START && static_cast<MessageStruct>(msg[i+5]) == MessageStruct::MESSAGE_END)
			{		
                requestValue->requestTypeId = static_cast<RequestTypeId>(msg[i+1]);
                AppParser::DetectRequest(requestValue, msg, i);
                requestValue->requestState = RequestState::SUCCESS;
				
                return false;		
			}
			else if(static_cast<MessageStruct>(msg[i]) == MessageStruct::MESSAGE_START || requestValue->requestState == RequestState::INCOMPLETE)
                requestValue->requestState = RequestState::INCOMPLETE;
			else
				requestValue->requestState = RequestState::GARBAGE;

			return true;
        }();	

		if(!goOn)
			break;
    }
	return requestValue;
}

void AppParser::DetectRequest(Request* &requestValue, const uint8_t* msg, uint8_t i)
{   
    if(AppParser::IsPossibleRequest(requestValue))
        requestValue->requestTypeId = static_cast<RequestTypeId>(msg[i+1]);

        for (uint8_t dataIndex = 0; dataIndex < 3; dataIndex++)			
            if (!AppParser::ExactRequest(requestValue, msg, i, dataIndex))
                break;
}

bool AppParser::IsPossibleRequest(Request* &requestValue)
{  
    Request* tempRequestValue = requestValue;

	switch (requestValue->requestTypeId)
	{
		case RequestTypeId::SET_WATCHDOG:
			requestValue = new SetWatchdogRequest();
            break;
		case RequestTypeId::SET_DEVICE_CONTROL:
			requestValue = new SetDeviceControlRequest();
            break;
		case RequestTypeId::SET_WAKEUP_SOURCE:
			requestValue = new SetWakeUpSourceRequest();
            break;
		case RequestTypeId::GO_TO_BOOTLOADER:
			requestValue = new GoToBootloaderRequest();
            break;
		case RequestTypeId::GET_PMC_STATE:
			requestValue = new GetPMCStateRequest();
            break;
		case RequestTypeId::GET_WATCHDOG_TYPE:
			requestValue = new GetWatchdogTypeRequest();
            break;
		case RequestTypeId::GET_DEVICE_CONTROL:
			requestValue = new GetDeviceControlRequest();
            break;
		case RequestTypeId::GET_WAKEUP_SOURCE:
			requestValue = new GetWakeUpSourceRequest();
            break;
		case RequestTypeId::GET_PMC_VERSION:
			requestValue = new GetPMCVersionRequest();
            break;
		case RequestTypeId::SET_PMC_STATE:
			requestValue = new SetPMCStateRequest();
            break;
		case RequestTypeId::GET_RUNNING_EXECUTABLE:
			requestValue = new GetRunningExecutableRequest();
            break;
		default:
			return false;		 	
	}

    delete tempRequestValue;
    return true;	
}
	
bool AppParser::ExactRequest(Request* &requestValue, const uint8_t* msg, uint8_t i, uint8_t dataIndex)
{	
	uint8_t dataIndexOffset = 2;
		
	i = dataIndex + i + dataIndexOffset;

	switch (requestValue->requestTypeId)
	{	
		case RequestTypeId::SET_WATCHDOG:
			switch (dataIndex)
			{	
				case 0:
					static_cast<SetWatchdogRequest*>(requestValue)->watchdogType = static_cast<WatchdogType>(msg[i]);
					break;
				case 1:
					static_cast<SetWatchdogRequest*>(requestValue)->duration = msg[i];
					break;
				case 2:
					static_cast<SetWatchdogRequest*>(requestValue)->duration |= msg[i] << 8;
                    break;
                default:
                    return false;
			}
			break;
			
		case RequestTypeId::SET_DEVICE_CONTROL:
			switch(dataIndex)
			{
				case 0:
					static_cast<SetDeviceControlRequest*>(requestValue)->device = static_cast<Device>(msg[i]);
					break;
				case 1:
					static_cast<SetDeviceControlRequest*>(requestValue)->deviceCommand = static_cast<DeviceCommand>(msg[i]);
					break;
                default:
                    return false;
			}
			break;	

		case RequestTypeId::SET_WAKEUP_SOURCE:
			static_cast<SetWakeUpSourceRequest*>(requestValue)->wakeUpSourceType = static_cast<WakeUpSourceType>(msg[i]);
			return false;
		
		case RequestTypeId::GET_WATCHDOG_TYPE:
			static_cast<GetWatchdogTypeRequest*>(requestValue)->watchdogType = static_cast<WatchdogType>(msg[i]);
			return false;

		case RequestTypeId::GET_DEVICE_CONTROL:
			static_cast<GetDeviceControlRequest*>(requestValue)->device = static_cast<Device>(msg[i]);
			return false;

		case RequestTypeId::SET_PMC_STATE:
			static_cast<SetPMCStateRequest*>(requestValue)->pmcState = static_cast<PMCState>(msg[i]);
			return false;

		case RequestTypeId::GO_TO_BOOTLOADER:
			return false;

		case RequestTypeId::GET_PMC_STATE:
			return false;

		case RequestTypeId::GET_WAKEUP_SOURCE:
			return false;

		case RequestTypeId::GET_PMC_VERSION:
			return false;

		case RequestTypeId::GET_RUNNING_EXECUTABLE:
			return false;				
	}
	return true;		
} 

