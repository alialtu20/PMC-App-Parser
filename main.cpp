// Example program
#include "AppParser.h"

#include <iostream>

using namespace std;

bool TestGeneral(const char testName[], const uint8_t* message, uint8_t messageLength, const AppParser::RequestTypeId& expectedTypeId, const AppParser::RequestState& expectedState, uint8_t expectedParameter1, uint8_t expectedParameter2, uint8_t expectedParameter3)
{
	bool result = true;

	cout << "----------------------\nStarted test: " << testName << endl;

	auto requestValue = AppParser::ParseUartMessage(message, messageLength); 

	if(expectedState != requestValue->requestState)
		result = false;
	
	if(expectedTypeId != requestValue->requestTypeId)
		result = false;
	
	if(requestValue->requestState == AppParser::RequestState::SUCCESS)
		
		switch (requestValue->requestTypeId)
		{
			case AppParser::RequestTypeId::SET_WATCHDOG:
				if(static_cast<AppParser::WatchdogType>(expectedParameter1) != static_cast<AppParser::SetWatchdogRequest*>(requestValue)->watchdogType ||
		  		  (expectedParameter2 | (expectedParameter3 << 8)) != static_cast<AppParser::SetWatchdogRequest*>(requestValue)->duration)
					result = false;
				break;

			case AppParser::RequestTypeId::SET_DEVICE_CONTROL:
				if(static_cast<AppParser::Device>(expectedParameter1) != static_cast<AppParser::SetDeviceControlRequest*>(requestValue)->device ||
		  		  static_cast<AppParser::DeviceCommand>(expectedParameter2) != static_cast<AppParser::SetDeviceControlRequest*>(requestValue)->deviceCommand)
					result = false;
				break;

			case AppParser::RequestTypeId::SET_WAKEUP_SOURCE:
				if(static_cast<AppParser::WakeUpSourceType>(expectedParameter1) != static_cast<AppParser::SetWakeUpSourceRequest*>(requestValue)->wakeUpSourceType)
					result = false;
				break;

			case AppParser::RequestTypeId::GET_WATCHDOG_TYPE:
				if(static_cast<AppParser::WatchdogType>(expectedParameter1) != static_cast<AppParser::GetWatchdogTypeRequest*>(requestValue)->watchdogType)
					result = false;
				break;

			case AppParser::RequestTypeId::GET_DEVICE_CONTROL:
				if(static_cast<AppParser::Device>(expectedParameter1) != static_cast<AppParser::GetDeviceControlRequest*>(requestValue)->device)
					result = false;				
				break;

			case AppParser::RequestTypeId::SET_PMC_STATE:
				if(static_cast<AppParser::PMCState>(expectedParameter1) != static_cast<AppParser::SetPMCStateRequest*>(requestValue)->pmcState)
					result = false;
				break;

			default:
				return true;		 	
		}

	cout << "Test result: " << result << endl;

	cout << "End test: " << testName << endl;

	return result;
}

int main()
{	
	//TEST
	// General Types expect SetWatchdog
	// for watchdog request->0x03, 0x03, 0x05 exact request data parameteres
	// and another request types
	
	int failedTests = 0;

	if(uint8_t msgGarbage[] = {0x03, 0x05, 0x01, 0xB0, 0x02, 0x03, 0x05}; !TestGeneral("Garbage_Parser_Test", msgGarbage, sizeof(msgGarbage), static_cast<AppParser::RequestTypeId>(0x00), AppParser::RequestState::GARBAGE, 0x00, 0x00, 0x00))
		failedTests++;

	if(uint8_t msgIncompleted[] = {0xAA, 0xB0, 0x02, 0x03, 0x05, 0xB1}; !TestGeneral("Incompleted_Parser_Test", msgIncompleted, sizeof(msgIncompleted), static_cast<AppParser::RequestTypeId>(0x00), AppParser::RequestState::INCOMPLETE, 0x00, 0x00, 0x00))
		failedTests++;

	if(uint8_t msgSuccess[] =  {0xAA, 0x01, 0x03, 0x03, 0x05, 0xFF}; !TestGeneral("Success_Parser_Test", msgSuccess, sizeof(msgSuccess), AppParser::RequestTypeId::SET_WATCHDOG ,AppParser::RequestState::SUCCESS, 0x03, 0x03, 0x05))
		failedTests++;

	if(uint8_t msgIncompletedPlusGarbage[] = {0x03, 0x05, 0xAA, 0xB0, 0x02, 0x03, 0x05, 0x06, 0x09}; !TestGeneral("IncompletedPlusGarbage_Parser_Test", msgIncompletedPlusGarbage, sizeof(msgIncompletedPlusGarbage), static_cast<AppParser::RequestTypeId>(0x00) ,AppParser::RequestState::INCOMPLETE, 0x00, 0x00, 0x00))
		failedTests++;

	if(uint8_t msgSuccessPlusGarbage[] = {0x03, 0x05, 0xAA, 0xAA, 0x01, 0x03, 0x03, 0x05, 0xFF, 0xB0, 0x02, 0x03, 0x05}; !TestGeneral("SuccessPlusGarbage_Parser_Test", msgSuccessPlusGarbage, sizeof(msgSuccessPlusGarbage), AppParser::RequestTypeId::SET_WATCHDOG ,AppParser::RequestState::SUCCESS, 0x03, 0x03, 0x05))
		failedTests++;

	if(uint8_t msgSuccessPlusIncompleted[] = {0xAA, 0xB9, 0x02, 0x03, 0x05, 0xAA, 0x01, 0x03, 0x03, 0x05, 0xFF}; !TestGeneral("SuccessPlusIncompleted_Parser_Test", msgSuccessPlusIncompleted, sizeof(msgSuccessPlusIncompleted), AppParser::RequestTypeId::SET_WATCHDOG ,AppParser::RequestState::SUCCESS, 0x03, 0x03, 0x05))
		failedTests++;

	if(uint8_t msgSuccessPlusIncompletedPlusGarbage[] = {0x03, 0x05, 0xAA, 0xB9, 0x02, 0x03, 0x05, 0xAA, 0x01, 0x03, 0x03, 0x05, 0xFF, 0x07, 0xC5}; !TestGeneral("SuccessPlusIncompletedPlusGarbage_Parser_Test", msgSuccessPlusIncompletedPlusGarbage, sizeof(msgSuccessPlusIncompletedPlusGarbage), AppParser::RequestTypeId::SET_WATCHDOG ,AppParser::RequestState::SUCCESS, 0x03, 0x03, 0x05))
		failedTests++;
	
	if(uint8_t msgSetWatchdog[] = {0xAA, 0x01, 0x02, 0x01, 0x02, 0xFF}; !TestGeneral("SetWatchdog_Parser_Test", msgSetWatchdog, sizeof(msgSetWatchdog), AppParser::RequestTypeId::SET_WATCHDOG ,AppParser::RequestState::SUCCESS, 0x02, 0x01, 0x02))
		failedTests++;

	if(uint8_t msgSetDeviceControl[] = {0xAA, 0x02, 0x02, 0x01, 0x05, 0xFF}; !TestGeneral("SetDeviceControl_Parser_Test", msgSetDeviceControl, sizeof(msgSetDeviceControl), AppParser::RequestTypeId::SET_DEVICE_CONTROL ,AppParser::RequestState::SUCCESS, 0x02, 0x01, 0x05))
		failedTests++;

	if(uint8_t msgSetWakeUpSource[] = {0xAA, 0x03, 0x02, 0x03, 0x05, 0xFF}; !TestGeneral("SetWakeUpSource_Parser_Test", msgSetWakeUpSource, sizeof(msgSetWakeUpSource), AppParser::RequestTypeId::SET_WAKEUP_SOURCE ,AppParser::RequestState::SUCCESS, 0x02, 0x03, 0x05))
		failedTests++;
	
	if(uint8_t msgGetWatchdogType[] = {0xAA, 0x06, 0x01, 0x03, 0x05, 0xFF}; !TestGeneral("GetWatchdogType_Parser_Test", msgGetWatchdogType, sizeof(msgGetWatchdogType), AppParser::RequestTypeId::GET_WATCHDOG_TYPE ,AppParser::RequestState::SUCCESS, 0x01, 0x03, 0x05))
		failedTests++;

	if(uint8_t msgGetDeviceControl[] = {0xAA, 0x07,0x03, 0x03, 0x05, 0xFF}; !TestGeneral("GetDeviceControl_Parser_Test", msgGetDeviceControl, sizeof(msgGetDeviceControl), AppParser::RequestTypeId::GET_DEVICE_CONTROL ,AppParser::RequestState::SUCCESS, 0x03, 0x03, 0x05))
		failedTests++; 

	if(uint8_t msgSetPMCState[] = {0xAA, 0xFD, 0x01, 0x03, 0x05, 0xFF}; !TestGeneral("SetPMCState_Parser_Test", msgSetPMCState, sizeof(msgSetPMCState), AppParser::RequestTypeId::SET_PMC_STATE ,AppParser::RequestState::SUCCESS, 0x01, 0x03, 0x05))
		failedTests++;

	cout << "\n----------------------TOTAL_FAILED_TESTS----------------------\nFailed tests: " << failedTests << endl;
}