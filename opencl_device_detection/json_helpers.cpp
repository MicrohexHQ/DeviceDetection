#include "json_helpers.h"

//#define NLOHMANN_JSON

// JSON HPP disabled due to msvc2013 incompatibility
#ifdef NLOHMANN_JSON
#include "json.hpp"
// for convenience
using json = nlohmann::json;

namespace amd_json {
	json getJsonDevicesArray(const std::vector<OpenCLDevice>& devs) {
		json retArr = json::array();
		for (const auto &dev : devs) {
			retArr.push_back({
				{ "DeviceID", dev.DeviceID },
				{ "BUS_ID", dev.BUS_ID },
				{ "_CL_DEVICE_NAME", dev._CL_DEVICE_NAME },
				{ "_CL_DEVICE_TYPE", dev._CL_DEVICE_TYPE },
				{ "_CL_DEVICE_GLOBAL_MEM_SIZE", dev._CL_DEVICE_GLOBAL_MEM_SIZE },
				{ "_CL_DEVICE_VENDOR", dev._CL_DEVICE_VENDOR },
				{ "_CL_DEVICE_VERSION", dev._CL_DEVICE_VERSION },
				{ "_CL_DRIVER_VERSION", dev._CL_DRIVER_VERSION },
				{ "_CL_DEVICE_BOARD_NAME_AMD", dev._CL_DEVICE_BOARD_NAME_AMD},
			});
		}
		return retArr;
	}

    json getJsonPlatform(const OpenCLPlatform& p) {
        return json {
			{ "PlatformVendor", p.PlatformVendor },
			{ "PlatformName", p.PlatformName },
			{ "PlatformNum", p.PlatformNum },
			{ "Devices", getJsonDevicesArray(p.Devices) }
        };
    }

    json conver_to_json(const std::vector<OpenCLPlatform>& ps, std::string statusStr, std::string errorStr) {
    	json platformsJson = json::array();
		for (const auto &p : ps) {
			platformsJson.push_back(getJsonPlatform(p));
		}
		return json {
        	{ "Status", statusStr },
			{ "Platforms", platformsJson },
			{ "ErrorString", errorStr }
        };
    }
}

std::string json_helpers::GetPlatformDevicesJsonString(std::vector<OpenCLPlatform> &platforms, std::string statusStr, std::string errorStr, bool prettyPrint) {
	const auto j = amd_json::conver_to_json(platforms, statusStr, errorStr);
	if (prettyPrint) {
		return j.dump(4);
	}
	return j.dump();
}

#else // use trivial_json_printer

#include "trivial_json_printer.hpp"

template <>
void WriteValue<OpenCLDevice>(std::stringstream &ss, OpenCLDevice dev) {
	StartObject(ss);
	AddJSONPropertyAndValue(ss, "DeviceID", dev.DeviceID);
	AddJSONPropertyAndValue(ss, "BUS_ID", dev.BUS_ID);
	AddJSONPropertyAndValue(ss, "_CL_DEVICE_NAME", dev._CL_DEVICE_NAME);
	AddJSONPropertyAndValue(ss, "_CL_DEVICE_TYPE", dev._CL_DEVICE_TYPE);
	AddJSONPropertyAndValue(ss, "_CL_DEVICE_GLOBAL_MEM_SIZE", dev._CL_DEVICE_GLOBAL_MEM_SIZE);
	AddJSONPropertyAndValue(ss, "_CL_DEVICE_VENDOR", dev._CL_DEVICE_VENDOR);
	AddJSONPropertyAndValue(ss, "_CL_DEVICE_VERSION", dev._CL_DEVICE_VERSION);
	AddJSONPropertyAndValue(ss, "_CL_DRIVER_VERSION", dev._CL_DRIVER_VERSION);
	AddJSONPropertyAndValue(ss, "_CL_DEVICE_BOARD_NAME_AMD", dev._CL_DEVICE_BOARD_NAME_AMD, false); // FALSE DO NOT TERMINATE WITH COMMA
	EndObject(ss);
}

template <>
void WriteValue<OpenCLPlatform>(std::stringstream &ss, OpenCLPlatform p) {
	StartObject(ss);
	AddJSONPropertyAndValue(ss, "PlatformNum", p.PlatformNum);
	AddJSONPropertyAndValue(ss, "PlatformVendor", p.PlatformVendor);
	AddJSONPropertyAndValue(ss, "PlatformName", p.PlatformName);
	AddJSONPropertyAndValue(ss, "Devices", p.Devices, false); // FALSE DO NOT TERMINATE WITH COMMA
	EndObject(ss);
}

std::string json_helpers::GetPlatformDevicesJsonString(std::vector<OpenCLPlatform> &platforms, std::string statusStr, std::string errorStr, bool prettyPrint) {
	std::stringstream ss;
	StartObject(ss);
	AddJSONPropertyAndValue(ss, "Platforms", platforms);
	AddJSONPropertyAndValue(ss, "Status", statusStr);
	AddJSONPropertyAndValue(ss, "ErrorString", errorStr, false); // FALSE DO NOT TERMINATE WITH COMMA
	EndObject(ss);

	if (prettyPrint) {
		return getPrettyString(ss.str(), 4);
	}

	return ss.str();
}

#endif 