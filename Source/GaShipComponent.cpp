#include "GaShipComponent.h"

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
	};
	
	using namespace std::placeholders;
	ReRegisterClass< GaShipComponent, Super >( Fields )
		.addAttribute( new GaShipProcessor() );
}

//////////////////////////////////////////////////////////////////////////
// Ctor
GaShipComponent::GaShipComponent()
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
}

//////////////////////////////////////////////////////////////////////////
// onDetach
void GaShipComponent::onDetach( ScnEntityWeakRef Parent )
{
}

