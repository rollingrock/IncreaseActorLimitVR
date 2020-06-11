#include <ShlObj.h>
#include "SKSE/API.h"
#include "SKSE/Interfaces.h"
#include "SKSE/Logger.h"
#include "INIReader.h"

int numActors;

REL::Offset<std::uintptr_t> firstLocaction(0x6fbfb0);
REL::Offset<std::uintptr_t> secondLocaction(0x6fbfbc);


bool loadConfig() {
	INIReader in(".\\Data\\SKSE\\plugins\\IncreaseActorLimit.ini");

	if (in.ParseError() < 0) {
		_MESSAGE("ERROR: cannot read IncreaseActorLimit.ini");
		return false;
	}

	numActors = in.GetInteger("IncreaseActorLimit", "NumberOfActors", 128);

	return true;
}

bool doFix() {

	if (numActors != 128) {
		UInt8 x = numActors & 0xFF;
		SKSE::SafeWrite8(firstLocaction.GetAddress() + 1, x);
		SKSE::SafeWrite8(secondLocaction.GetAddress() + 1, x);

		_MESSAGE("patched offsets %016I64X and %016I64X", firstLocaction.GetOffset(), secondLocaction.GetOffset());
		return true;
	}

	_MESSAGE("selected value is default.    no need to patch");

	return true;
}

extern "C" {
	bool SKSEPlugin_Query(const SKSE::QueryInterface* a_skse, SKSE::PluginInfo* a_info)
	{
		SKSE::Logger::OpenRelative(FOLDERID_Documents, R"(\\My Games\\Skyrim VR\\SKSE\\IncreaseActorLimit.log)");
		SKSE::Logger::SetPrintLevel(SKSE::Logger::Level::kDebugMessage);
		SKSE::Logger::SetFlushLevel(SKSE::Logger::Level::kDebugMessage);
		SKSE::Logger::TrackTrampolineStats(true);
		SKSE::Logger::UseLogStamp(true);

		a_info->infoVersion = SKSE::PluginInfo::kVersion;
		a_info->name = "IncreaseActorLimit";
		a_info->version = 1;

		if (a_skse->IsEditor())
		{
			_ERROR("loaded in editor, marking as incompatible");

			return false;
		}

		auto ver = a_skse->RuntimeVersion();
		if (ver <= SKSE::RUNTIME_VR_1_4_15)
		{
			_FATALERROR("Unsupported runtime version %s!\n", ver.GetString().c_str());
			return false;
		}
		return true;
	}

	bool SKSEPlugin_Load(const SKSE::LoadInterface* a_skse)
	{
		if (!SKSE::Init(a_skse))
		{
			_FATALERROR("Cannot INIT SKSE CommonLib!!!!");
			return false;
		}

		if (!loadConfig()) {
			return false;
		}
		_MESSAGE("settings ini successfully read in");

		doFix();
		_MESSAGE("[MESSAGE] IncreaseActorLimit loaded");
		return true;
	}
};