#include "GaBulletComponent.h"

#include "Base/BcRandom.h"
#include "System/Scene/ScnEntity.h"
#include "System/Scene/Rendering/ScnModel.h"

#include "System/Os/OsCore.h"
#include "System/Os/OsEvents.h"
#include "System/SysKernel.h"
#include "GaGameComponent.h"
#include "GaShipComponent.h"

#include "System/Scene/Sound/ScnSoundEmitter.h"
#include "GaTitleComponent.h"
//////////////////////////////////////////////////////////////////////////
// Ctor
GaBulletProcessor::GaBulletProcessor():
	ScnComponentProcessor( {
	ScnComponentProcessFuncEntry(
		"Update Update",
		ScnComponentPriority::DEFAULT_UPDATE,
		std::bind(&GaBulletProcessor::updateBullets, this, std::placeholders::_1)),
	ScnComponentProcessFuncEntry(
		"Do Collisions",
		ScnComponentPriority::DEFAULT_UPDATE + 10,
		std::bind(&GaBulletProcessor::bulletCollisions, this, std::placeholders::_1)),
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
	float dt = SysKernel::pImpl()->getFrameTime();
	static float Time = 0.0f;
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
		if ((newPos.x() < MinConstraint_.x()) || (newPos.x() > MaxConstraint_.x()) ||
			(newPos.z() < MinConstraint_.z()) || (newPos.z() > MaxConstraint_.z()))
		{
			//ShipComponent->ParentEntity_->destroy();
			ScnCore::pImpl()->removeEntity(ShipComponent->getParentEntity());
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// updateBullets
void GaBulletProcessor::bulletCollisions(const ScnComponentList& Components)
{
	float dt = SysKernel::pImpl()->getFrameTime();
	static float Time = 0.0f;
	Time += dt;
	// Iterate over all the ships.
	for (BcU32 Idx = 0; Idx < Components.size(); ++Idx)
	{
		auto Component = Components[Idx];
		BcAssert(Component->isTypeOf< GaBulletComponent >());
		auto* BulletComponent = static_cast< GaBulletComponent* >(Component.get());
		MaVec3d position = BulletComponent->getParentEntity()->getWorldPosition();
		for (BcU32 Idx2 = 0; Idx2 < Ships_.size(); ++Idx2)
		{
			auto* Ship = Ships_[Idx2];
			if (Ship == BulletComponent->Ship_)
				continue;
			MaVec3d shipPos = Ship->getParentEntity()->getWorldPosition();
			MaVec3d dif = position - shipPos;
			float distance = dif.magnitude();
			if (distance < 5.0f) 
			{ 
				if( BulletComponent->ExplodeSounds_.size() > 0 )
				{
					auto Emitter = BulletComponent->getComponentByType< ScnSoundEmitterComponent >();
					BcAssert( Emitter );
					Emitter->playOneShot( BulletComponent->ExplodeSounds_[ BcRandom::Global.randRange( 0, BulletComponent->ExplodeSounds_.size() - 1 ) ] );
				}

				if (!Ship->IsPlayer()){
					ScnCore::pImpl()->removeEntity(Ship->getParentEntity());
					BulletComponent->Ship_->addScore(100);
					ScnCore::pImpl()->removeEntity(BulletComponent->getParentEntity());
				}
				else 
				{
					GaTitleProcessor::pImpl()->showTitle();
					ScnCore::pImpl()->removeEntity(BulletComponent->getParentEntity()->getParentEntity());
				}
			}
		}/**/
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
void GaBulletProcessor::addShip(GaShipComponent * Ship)
{
	Ships_.push_back(Ship);
}
void GaBulletProcessor::removeShip(GaShipComponent * Ship)
{
	for (int i = 0; i < Ships_.size(); ++i)
	{
		if (Ships_[i] == Ship)
		{
			Ships_.erase(Ships_.begin() + i);
			return;
		}
	}
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
		new ReField( "EngineSound_", &GaBulletComponent::EngineSound_, bcRFF_IMPORTER | bcRFF_SHALLOW_COPY ),
		new ReField( "ExplodeSounds_", &GaBulletComponent::ExplodeSounds_, bcRFF_IMPORTER | bcRFF_SHALLOW_COPY ),

		new ReField( "Ship_", &GaBulletComponent::Ship_, bcRFF_TRANSIENT ),
		
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

	if( EngineSound_ )
	{
		auto Emitter = getComponentByType< ScnSoundEmitterComponent >();
		BcAssert( Emitter );
		Emitter->play( EngineSound_, true );
	}
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

void GaBulletComponent::setShip(GaShipComponent * Ship)
{
	Ship_ = Ship;
}

