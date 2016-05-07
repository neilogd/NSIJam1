#include "GaWaveComponent.h"
#include "System/Scene/ScnEntity.h"
#include "System/Scene/Rendering/ScnModel.h"

#include "System/Os/OsCore.h"
#include "System/Os/OsEvents.h"

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

}