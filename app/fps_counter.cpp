#include "fps_counter.h"

FpsCounter::FpsCounter(Debug* debug, std::string prefix)
{
	debug->buildUI.connect(boost::bind(&FpsCounter::debugUI, this));
	this->prefix = prefix;
	fps = 0;
}

void FpsCounter::debugUI()
{
	ImGui::SetNextWindowPos(ImVec2(10, ImGui::GetIO().DisplaySize.y - shift), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
	ImGui::SetNextWindowBgAlpha(0.3f);
	ImGui::SetNextWindowSize(ImVec2(100, 60));
	ImGui::Begin(prefix.data(), nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
	ImGui::Text(prefix.data());
	ImGui::Text("%4.0f FPS", fps);
	ImGui::End();
}

void FpsCounter::update(float deltaTime)
{
	fps = 1 / deltaTime;
}