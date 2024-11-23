#include "fps_counter.h"

FpsCounter::FpsCounter(Debug* debug, std::string prefix)
{
	debug->buildUI.connect(boost::bind(&FpsCounter::debugUI, this));
	this->prefix = prefix;
	fps = 0;
}

void FpsCounter::debugUI() const
{
	ImGui::SetNextWindowPos(ImVec2(10, ImGui::GetIO().DisplaySize.y - 10), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
	ImGui::SetNextWindowBgAlpha(0.3f);
	ImGui::SetNextWindowSize(ImVec2(150, 60));
	ImGui::Begin("Counters", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove);
	auto print = prefix + "%4.0f FPS";
	ImGui::Text(print.data(), fps);
	ImGui::End();
}

void FpsCounter::update(float deltaTime)
{
	fps = 1 / deltaTime;
}