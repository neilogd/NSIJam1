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
#if 0
	ReField* Fields[] = 
	{
	};
#endif

	ReRegisterClass< GaGameComponent, Super >()
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
