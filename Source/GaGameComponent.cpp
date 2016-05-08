#include "GaGameComponent.h"

#include "System/Scene/Rendering/ScnShaderFileData.h"
#include "System/Scene/Rendering/ScnModel.h"
#include "System/Scene/Rendering/ScnViewComponent.h"

#include "System/Scene/Rendering/ScnDebugRenderComponent.h"

#include "System/Scene/Rendering/ScnCanvasComponent.h"
#include "System/Scene/Rendering/ScnFont.h"

#include "System/Content/CsPackage.h"
#include "System/Content/CsCore.h"

#include "System/Os/OsCore.h"
#include "System/Os/OsClient.h"

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
#if !PSY_PRODUCTION
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

	MaAABB AABB( getConstraintMin(), getConstraintMax() );
	ScnDebugRenderComponent::pImpl()->drawAABB( AABB, RsColour::RED, 0 );
#endif

	ScnFontComponent* Font = getComponentAnyParentByType< ScnFontComponent >();
	ScnCanvasComponent* Canvas = getComponentAnyParentByType< ScnCanvasComponent >();

	OsClient* Client = OsCore::pImpl()->getClient( 0 );
	BcF32 HalfWidth = static_cast< BcF32 >( Client->getWidth() / 2 );
	BcF32 HalfHeight = static_cast< BcF32 >( Client->getHeight() / 2 );

	MaMat4d Projection;
	Projection.orthoProjection( -HalfWidth, HalfWidth, HalfHeight, -HalfHeight, -1.0f, 1.0f );

	Canvas->clear();
	Canvas->pushMatrix( Projection );

	ScnFontDrawParams DrawParams = ScnFontDrawParams()
		.setSize( 48.0f )
		.setMargin( 8.0f )
		.setAlignment( ScnFontAlignment::HCENTRE | ScnFontAlignment::TOP )
		.setWrappingEnabled( BcTrue )
		.setTextColour( RsColour::WHITE )
		.setBorderColour( RsColour::BLACK )
		.setTextSettings( MaVec4d( 0.4f, 0.5f, 0.0f, 0.0f ) )
		.setBorderSettings( MaVec4d( 1.0f, 0.0f, 0.0f, 0.0f ) );
			
	Font->drawText( 
		Canvas,
		DrawParams,
		MaVec2d( -HalfWidth, -HalfHeight ),
		MaVec2d( HalfWidth, HalfHeight ) * 2.0f,
		L"The quick brown fox jumps over the lazy dog.\n" );
	
	Canvas->popMatrix();
}

//////////////////////////////////////////////////////////////////////////
// onAttach
//virtual
void GaGameComponent::onAttach( ScnEntityWeakRef Parent )
{
	Super::onAttach( Parent );
	GaShipProcessor::pImpl()->registerGame(this);
	GaBulletProcessor::pImpl()->registerGame(this);
	MaVec3d max = getConstraintMax();
	MaVec3d min = getConstraintMin();
	MaMat4d mat;
	float d = 0.9f;
	float pos = (1.0f - d) * max.x() + d * min.y();
	mat.translation(MaVec3d(0.0f, 0.0f, pos));
	ScnCore::pImpl()->spawnEntity(
		ScnEntitySpawnParams(
			"PlayerShip_0", PlayerShipTemplates_[0],
			mat, getParentEntity()));

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
