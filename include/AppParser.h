#pragma once

#include <cstdint>

class AppParser
{
	public:
		virtual ~AppParser() = 0;

		enum class RequestState : uint8_t
		{
			GARBAGE,
			SUCCESS,
			INCOMPLETE,
		};
		
		enum class RequestTypeId : uint8_t 
		{
			SET_WATCHDOG = 0x01,
			SET_DEVICE_CONTROL = 0x02,
			SET_WAKEUP_SOURCE = 0x03,
			GO_TO_BOOTLOADER = 0x04,
			GET_PMC_STATE = 0x05,
			GET_WATCHDOG_TYPE = 0x06,
			GET_DEVICE_CONTROL = 0x07,
			GET_WAKEUP_SOURCE = 0x08,
			GET_PMC_VERSION = 0x09,
			SET_PMC_STATE = 0xFD,
			GET_RUNNING_EXECUTABLE = 0xFE,
		};
		
		enum class MessageStruct : uint8_t
		{
			MESSAGE_START = 0xAA,
			MESSAGE_END = 0xFF,
			MESSAGE_BYTE_VALUE = 0x06,
		};

		enum class PMCState : uint8_t
		{
			ACTIVE = 0x01,
			SHUTDOWN = 0x02,
			SLEEP = 0x03,
			STANDBY = 0x04,
		};

		enum class WatchdogType : uint8_t
		{
			RUNTIME = 0x01,
			SHUTDOWN = 0x02,
			BOOT = 0x03,
		};
	
		enum class Device : uint8_t
		{
			ACCELEROMETER = 0x01,
			BLE = 0x02,
			CAN = 0x03,
			LTE = 0x04,
			RTC = 0x05,
			GPS = 0x06,
		};
	
		enum class DeviceCommand : uint8_t
		{
			RESET = 0x01,
			ACTIVE = 0x02,
			POWER_OFF = 0x03,
			SLEEP = 0x04,
		};
	
		enum class WakeUpSourceType : uint8_t
		{
			ACCELEROMETER = 0x01,
			BLE = 0x02,
			CAN = 0x03,
			LTE = 0x04,
			RTC = 0x05,
			CLAMP15 = 0x06,
			REMOVE_ALL = 0xFF,
		};

		struct Request
		{
			RequestState requestState;
			RequestTypeId requestTypeId;
		};

		struct SetWatchdogRequest final : public Request
		{
			WatchdogType watchdogType;
			uint16_t duration;
		};
		
		struct SetDeviceControlRequest final : public Request
		{
			Device device;
			DeviceCommand deviceCommand;	
		};
		
		struct SetWakeUpSourceRequest final : public Request
		{
			WakeUpSourceType wakeUpSourceType;
		};
		
		struct GetWatchdogTypeRequest final : public Request
		{
			WatchdogType watchdogType;
		};

		struct GetWakeUpSourceRequest final : public Request
		{
			WakeUpSourceType wakeUpSourceType;		
		};	

		struct GetDeviceControlRequest final : public Request
		{
			Device device;
		};	

		struct SetPMCStateRequest final : public Request
		{
			PMCState pmcState;
		};			

		struct GoToBootloaderRequest final : public Request {};
		struct GetPMCStateRequest final : public Request {};
		struct GetPMCVersionRequest final : public Request {};
		struct GetRunningExecutableRequest final : public Request {};
		
		static Request* ParseUartMessage(const uint8_t*, uint8_t);	

	private:
		static void DetectRequest(Request*&, const uint8_t*, uint8_t);
		static bool IsPossibleRequest(Request*&);
		static bool ExactRequest(Request*&, const uint8_t*, uint8_t, uint8_t);
};