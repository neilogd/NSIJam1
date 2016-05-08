#include "GaGameComponent.h"

#include "System/Scene/Rendering/ScnShaderFileData.h"
#include "System/Scene/Rendering/ScnModel.h"
#include "System/Scene/Rendering/ScnViewComponent.h"

#include "System/Content/CsPackage.h"
#include "System/Content/CsCore.h"

#include "System/Debug/DsImGui.h"

#include "Base/BcProfiler.h"
#include "Base/BcRandom.h"

#include "GaBulletComponent.h"
#include "GaShipComponent.h"

//////////////////////////////////////////////////////////////////////////
// Define resource internals.
REFLECTION_DEFINE_DERIVED( GaGameComponent );

void GaGameComponent::StaticRegisterClass()
{
	ReField* Fields[] = 
	{
		new ReField( "PlayerShipTemplates_", &GaGameComponent::PlayerShipTemplates_, bcRFF_IMPORTER | bcRFF_SHALLOW_COPY ),
		new ReField( "EnemyShipTemplates_", &GaGameComponent::EnemyShipTemplates_, bcRFF_IMPORTER | bcRFF_SHALLOW_COPY ),
		new ReField( "MaxX_", &GaGameComponent::MaxX_, bcRFF_IMPORTER ),
		new ReField( "MinZ_", &GaGameComponent::MinZ_, bcRFF_IMPORTER ),
		new ReField( "MaxZ_", &GaGameComponent::MaxZ_, bcRFF_IMPORTER ),
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
// GetConstraintMin
MaVec3d GaGameComponent::getConstraintMin()
{
	return MaVec3d(-MaxX_, 0.0f, MinZ_);
}

//////////////////////////////////////////////////////////////////////////
// getConstraintMax
MaVec3d GaGameComponent::getConstraintMax()
{
	return MaVec3d(MaxX_, 0.0f, MaxZ_);
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
		if (ImGui::Button("Spawn Enemy"))
		{
			int ship = BcRandom::Global.rand() % EnemyShipTemplates_.size();
			BcAssert(EnemyShipTemplates_.size() > 0);
			ScnCore::pImpl()->spawnEntity(
				ScnEntitySpawnParams(
					"EnemyShip_0", EnemyShipTemplates_[ship],
					MaMat4d(), getParentEntity()));
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
	GaShipProcessor::pImpl()->registerGame(this);
	GaBulletProcessor::pImpl()->registerGame(this);
	ScnCore::pImpl()->spawnEntity(
		ScnEntitySpawnParams(
			"PlayerShip_0", PlayerShipTemplates_[0],
			MaMat4d(), getParentEntity()));

}

//////////////////////////////////////////////////////////////////////////
// onDetach
//virtual
void GaGameComponent::onDetach( ScnEntityWeakRef Parent )
{
	Super::onDetach( Parent );
	GaShipProcessor::pImpl()->deregisterGame(this);
	GaBulletProcessor::pImpl()->deregisterGame(this);
}
