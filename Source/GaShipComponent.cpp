#include "GaShipComponent.h"
#include "System/Scene/ScnEntity.h"
#include "System/Scene/Rendering/ScnModel.h"

#include "System/Os/OsCore.h"
#include "System/Os/OsEvents.h"


//////////////////////////////////////////////////////////////////////////
// Ctor
GaShipProcessor::GaShipProcessor():
	ScnComponentProcessor( {
		ScnComponentProcessFuncEntry(
			"Update Ships",
			ScnComponentPriority::DEFAULT_UPDATE,
			std::bind( &GaShipProcessor::updateShips, this, std::placeholders::_1 ) ) } )
{
	
}

//////////////////////////////////////////////////////////////////////////
// Dtor
GaShipProcessor::~GaShipProcessor()
{
}

//////////////////////////////////////////////////////////////////////////
// updateShips
void GaShipProcessor::updateShips( const ScnComponentList& Components )
{
	// Iterate over all the ships.
	for( BcU32 Idx = 0; Idx < Components.size(); ++Idx )
	{
		auto Component = Components[ Idx ];
		BcAssert( Component->isTypeOf< GaShipComponent >() );
		auto* ShipComponent = static_cast< GaShipComponent* >( Component.get() );

		// Individual ship updates here?
		BcUnusedVar( ShipComponent );
	}
}

//////////////////////////////////////////////////////////////////////////
// initialise
void GaShipProcessor::initialise()
{
	OsCore::pImpl()->subscribe(osEVT_INPUT_KEYDOWN, this,
		[this] (EvtID ID, const EvtBaseEvent& InEvent) {
			const auto &Event = InEvent.get<OsEventInputKeyboard>();
			return evtRET_PASS;
		});
}

//////////////////////////////////////////////////////////////////////////
// shutdown
void GaShipProcessor::shutdown()
{

}

//////////////////////////////////////////////////////////////////////////
// StaticRegisterClass
REFLECTION_DEFINE_DERIVED( GaShipComponent );

void GaShipComponent::StaticRegisterClass()
{
	ReField* Fields[] = 
	{
		new ReField( "IsPlayer_", &GaShipComponent::IsPlayer_, bcRFF_IMPORTER ),

		new ReField( "InstructionSet_", &GaShipComponent::InstructionSet_, bcRFF_TRANSIENT ),
		new ReField( "Model_", &GaShipComponent::Model_, bcRFF_TRANSIENT ),
		new ReField( "GunNodeIndices_", &GaShipComponent::GunNodeIndices_, bcRFF_TRANSIENT ),
		new ReField( "EngineNodeIndices_", &GaShipComponent::EngineNodeIndices_, bcRFF_TRANSIENT ),
	};
	
	using namespace std::placeholders;
	ReRegisterClass< GaShipComponent, Super >( Fields )
		.addAttribute( new GaShipProcessor() );
}

//////////////////////////////////////////////////////////////////////////
// Ctor
GaShipComponent::GaShipComponent()
	: InstructionSet_(0)
{
}

//////////////////////////////////////////////////////////////////////////
// Dtor
GaShipComponent::~GaShipComponent()
{
}
	
//////////////////////////////////////////////////////////////////////////
// onAttach
void GaShipComponent::onAttach( ScnEntityWeakRef Parent )
{
	Super::onAttach( Parent );

	Model_ = Parent->getComponentByType< ScnModelComponent >();
	if( Model_ )
	{
		// Find all gun nodes.
		BcU32 Idx = 0;
		BcU32 GunNodeIdx = BcErrorCode;
		while( ( GunNodeIdx = Model_->findNodeIndexByName( BcName( "Gun", Idx ) ) ) != BcErrorCode )
		{
			GunNodeIndices_.push_back( GunNodeIdx );
			++Idx;
		}

		// Find all engine nodes.
		Idx = 0;
		BcU32 EngineNodeIdx = BcErrorCode;
		while( ( EngineNodeIdx = Model_->findNodeIndexByName( BcName( "Engine", Idx ) ) ) != BcErrorCode )
		{
			EngineNodeIndices_.push_back( EngineNodeIdx );
			++Idx;
		}
	}
	else
	{
		PSY_LOG( "GaShipComponent: Unable to find a model component on entity \"%s\"", Parent->getFullName().c_str() );
	}
}

//////////////////////////////////////////////////////////////////////////
// onDetach
void GaShipComponent::onDetach( ScnEntityWeakRef Parent )
{
	Super::onDetach( Parent );
}

