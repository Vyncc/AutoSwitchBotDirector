#include "pch.h"
#include "AutoSwitchBotDirector.h"


void AutoSwitchBotDirector::RenderSettings()
{
	ImGui::Checkbox("Enable For End Of Replays", &enableForEndOfReplay);
	ImGui::Checkbox("Enable For Overtime Kick Off", &enableForOvertimeKickOff);
}