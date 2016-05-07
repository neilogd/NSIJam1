#include "GaWaveComponent.h"
#include "System/Scene/ScnEntity.h"
#include "System/Scene/Rendering/ScnModel.h"

#include "System/Os/OsCore.h"
#include "System/Os/OsEvents.h"

#include "System/Debug/DsImGui.h"

#include "GaEvents.h"

//////////////////////////////////////////////////////////////////////////
// StaticRegisterClass
REFLECTION_DEFINE_DERIVED( GaWaveComponent );

void GaWaveComponent::StaticRegisterClass()
{
	ReField* Fields[] = 
	{
		new ReField( "CurrentWave_", &GaWaveComponent::CurrentWave_, bcRFF_IMPORTER ),
	};
	
	using namespace std::placeholders;
	ReRegisterClass< GaWaveComponent, Super >( Fields )
		.addAttribute(new ScnComponentProcessor(
		{
			ScnComponentProcessFuncEntry::Update< GaWaveComponent >()
		}));
}

//////////////////////////////////////////////////////////////////////////
// Ctor
GaWaveComponent::GaWaveComponent()
	: CurrentWave_(0)
{
}

//////////////////////////////////////////////////////////////////////////
// Dtor
GaWaveComponent::~GaWaveComponent()
{
}
	
//////////////////////////////////////////////////////////////////////////
// onAttach
void GaWaveComponent::onAttach( ScnEntityWeakRef Parent )
{
	Super::onAttach( Parent );
}

//////////////////////////////////////////////////////////////////////////
// onDetach
void GaWaveComponent::onDetach( ScnEntityWeakRef Parent )
{
	Super::onDetach( Parent );
}

int GaWaveComponent::getEnemySpawnCount()
{
	return 1;
}

float GaWaveComponent::getShipOffset(int Ship)
{
	float enemies = getEnemySpawnCount();
	return ((float)Ship + 1.0f) / (enemies + 1.0f);
}

void GaWaveComponent::update(float Tick)
{
	if (ImGui::Begin("Wave Debug Menu"))
	{
		ImGui::Separator();
		ImGui::BeginGroup();

		if (ImGui::Button("Start New Wave"))
		{
			StartWave();
		}
		if (ImGui::Button("End Wave"))
		{
			EndWave();
		}
		ImGui::EndGroup();
		ImGui::Separator();
	}
	ImGui::End();
}

void GaWaveComponent::StartWave()
{
	GaEventWave Event;
	Event.StartWave_ = true;
	OsCore::pImpl()->publish(gaEVT_START_WAVE, Event);
}

void GaWaveComponent::EndWave()
{
	GaEventWave Event;
	Event.StartWave_ = false;
	OsCore::pImpl()->publish(gaEVT_START_WAVE, Event);

}