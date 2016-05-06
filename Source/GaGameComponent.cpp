#include "GaGameComponent.h"

#include "System/Scene/Rendering/ScnShaderFileData.h"
#include "System/Scene/Rendering/ScnModel.h"
#include "System/Scene/Rendering/ScnViewComponent.h"

#include "System/Content/CsPackage.h"
#include "System/Content/CsCore.h"

#include "System/Debug/DsImGui.h"

#include "Base/BcProfiler.h"
#include "Base/BcRandom.h"

//////////////////////////////////////////////////////////////////////////
// Define resource internals.
REFLECTION_DEFINE_DERIVED( GaGameComponent );

void GaGameComponent::StaticRegisterClass()
{
	ReField* Fields[] = 
	{
		new ReField( "PlayerShipTemplates_", &GaGameComponent::PlayerShipTemplates_, bcRFF_IMPORTER | bcRFF_SHALLOW_COPY ),
		new ReField( "EnemyShipTemplates_", &GaGameComponent::EnemyShipTemplates_, bcRFF_IMPORTER | bcRFF_SHALLOW_COPY ),
	};

	ReRegisterClass< GaGameComponent, Super >( Fields )
		.addAttribute( new ScnComponentProcessor( 
			{
				ScnComponentProcessFuncEntry::Update< GaGameComponent >()
			} ) );
}

//////////////////////////////////////////////////////////////////////////
// Ctor
GaGameComponent::GaGameComponent()
{

}

//////////////////////////////////////////////////////////////////////////
// Dtor
//virtual
GaGameComponent::~GaGameComponent()
{

}

//////////////////////////////////////////////////////////////////////////
// update
void GaGameComponent::update( BcF32 Tick )
{
	if( ImGui::Begin( "Game Debug Menu" ) )
	{
		ImGui::Separator();
		ImGui::BeginGroup();

		if( ImGui::Button( "Spawn player" ) )
		{
			BcAssert( PlayerShipTemplates_.size() > 0 );
			ScnCore::pImpl()->spawnEntity( 
				ScnEntitySpawnParams(
				"PlayerShip_0", PlayerShipTemplates_[ 0 ],
				MaMat4d(), getParentEntity() ) );
		}

		ImGui::EndGroup();
		ImGui::Separator();
	}
	ImGui::End();
}

//////////////////////////////////////////////////////////////////////////
// onAttach
//virtual
void GaGameComponent::onAttach( ScnEntityWeakRef Parent )
{
	Super::onAttach( Parent );
}

//////////////////////////////////////////////////////////////////////////
// onDetach
//virtual
void GaGameComponent::onDetach( ScnEntityWeakRef Parent )
{
	Super::onDetach( Parent );
}
