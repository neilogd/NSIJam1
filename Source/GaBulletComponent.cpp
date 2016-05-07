#include "GaBulletComponent.h"

#include "Base/BcRandom.h"
#include "System/Scene/ScnEntity.h"
#include "System/Scene/Rendering/ScnModel.h"

#include "System/Os/OsCore.h"
#include "System/Os/OsEvents.h"

#include "GaGameComponent.h"
//////////////////////////////////////////////////////////////////////////
// Ctor
GaBulletProcessor::GaBulletProcessor():
	ScnComponentProcessor( {
	ScnComponentProcessFuncEntry(
		"Update Update",
		ScnComponentPriority::DEFAULT_UPDATE,
		std::bind(&GaBulletProcessor::updateBullets, this, std::placeholders::_1)),
	} )
{

}

//////////////////////////////////////////////////////////////////////////
// Dtor
GaBulletProcessor::~GaBulletProcessor()
{
}


//////////////////////////////////////////////////////////////////////////
// updateBullets
void GaBulletProcessor::updateBullets(const ScnComponentList& Components)
{
	static float Time = 0.0f;
	static float dt = 1 / 60.0f;
	Time += dt;
	// Iterate over all the ships.
	for (BcU32 Idx = 0; Idx < Components.size(); ++Idx)
	{
		auto Component = Components[Idx];
		BcAssert(Component->isTypeOf< GaBulletComponent >());
		auto* ShipComponent = static_cast< GaBulletComponent* >(Component.get());
		MaVec3d movement(ShipComponent->Direction_);
		movement.normalise();
		movement.x(movement.x() * ShipComponent->Speed_);
		movement.z(movement.z() * ShipComponent->Speed_);
		MaVec3d oldPos = ShipComponent->getParentEntity()->getWorldPosition();
		MaVec3d newPos = oldPos + movement * dt;
		ShipComponent->getParentEntity()->setWorldPosition(newPos);
		if ( ( newPos.x() < MinConstraint_.x()) || (newPos.x() > MaxConstraint_.x() ) || 
			(newPos.z() < MinConstraint_.z()) || (newPos.z() > MaxConstraint_.z()) ) 
		{
			//ShipComponent->ParentEntity_->destroy();
			ScnCore::pImpl()->removeEntity(ShipComponent->getParentEntity());
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// addPlayer
void GaBulletProcessor::registerGame(GaGameComponent* Game)
{
	GameComponent_ = Game;
	MinConstraint_ = Game->getConstraintMin();
	MaxConstraint_ = Game->getConstraintMax();
}

//////////////////////////////////////////////////////////////////////////
// addPlayer
void GaBulletProcessor::deregisterGame(GaGameComponent* Game)
{
	if (Game == GameComponent_)
		GameComponent_ = nullptr;
}
//////////////////////////////////////////////////////////////////////////
// initialise
void GaBulletProcessor::initialise()
{
}


//////////////////////////////////////////////////////////////////////////
// shutdown
void GaBulletProcessor::shutdown()
{

}

//////////////////////////////////////////////////////////////////////////
// StaticRegisterClass
REFLECTION_DEFINE_DERIVED( GaBulletComponent );

void GaBulletComponent::StaticRegisterClass()
{
	ReField* Fields[] = 
	{
		new ReField( "Speed_", &GaBulletComponent::Speed_, bcRFF_IMPORTER ),
		new ReField( "Direction_", &GaBulletComponent::Direction_, bcRFF_IMPORTER ),
	};
	
	using namespace std::placeholders;
	ReRegisterClass< GaBulletComponent, Super >( Fields )
		.addAttribute( new GaBulletProcessor() );
}

//////////////////////////////////////////////////////////////////////////
// Ctor
GaBulletComponent::GaBulletComponent()
	: Speed_(15)
{
}

//////////////////////////////////////////////////////////////////////////
// Dtor
GaBulletComponent::~GaBulletComponent()
{
}
	
//////////////////////////////////////////////////////////////////////////
// onAttach
void GaBulletComponent::onAttach( ScnEntityWeakRef Parent )
{
	Super::onAttach( Parent );
	Model_ = Parent->getComponentByType< ScnModelComponent >();
}

//////////////////////////////////////////////////////////////////////////
// onDetach
void GaBulletComponent::onDetach( ScnEntityWeakRef Parent )
{
	Super::onDetach( Parent );
}

void GaBulletComponent::setDirection(MaVec3d Direction)
{
	Direction_ = Direction;
}

