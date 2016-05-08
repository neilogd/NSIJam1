#include "GaShipComponent.h"

#include "Base/BcRandom.h"
#include "System/Scene/ScnEntity.h"
#include "System/Scene/Rendering/ScnModel.h"
#include "System/SysKernel.h"
#include "System/Os/OsCore.h"
#include "System/Os/OsEvents.h"
#include "System/Scene/Sound/ScnSoundEmitter.h"

#include "GaGameComponent.h"
#include "GaBulletComponent.h"
#include "GaTitleComponent.h"
//////////////////////////////////////////////////////////////////////////
// Ctor
GaShipProcessor::GaShipProcessor():
	ScnComponentProcessor( {
	ScnComponentProcessFuncEntry(
		"Update Player",
		ScnComponentPriority::DEFAULT_UPDATE,
		std::bind(&GaShipProcessor::updatePlayers, this, std::placeholders::_1)),
	ScnComponentProcessFuncEntry(
		"Update Enemies",
		ScnComponentPriority::DEFAULT_UPDATE + 10,
		std::bind(&GaShipProcessor::updateEnemies, this, std::placeholders::_1)),
	ScnComponentProcessFuncEntry(
		"Move Ships",
		ScnComponentPriority::DEFAULT_UPDATE + 20,
		std::bind(&GaShipProcessor::updateShipPositions, this, std::placeholders::_1)),
	ScnComponentProcessFuncEntry(
		"Fire Weapons!",
		ScnComponentPriority::DEFAULT_UPDATE + 30,
		std::bind(&GaShipProcessor::fireWeapons, this, std::placeholders::_1))
	} )
{
	InstructionSets_.push_back(std::vector<WaveInstruction>());
	InstructionSets_[0].push_back(WaveInstruction(0, InstructionState::SWITCH_ON, Instruction::MOVE_LEFT));
	InstructionSets_[0].push_back(WaveInstruction(3, InstructionState::SWITCH_OFF, Instruction::MOVE_LEFT));
	InstructionSets_[0].push_back(WaveInstruction(4, InstructionState::SWITCH_ON, Instruction::MOVE_RIGHT));
	InstructionSets_[0].push_back(WaveInstruction(7, InstructionState::SWITCH_OFF, Instruction::MOVE_RIGHT));
	InstructionSets_[0].push_back(WaveInstruction(8, InstructionState::SWITCH_ON, Instruction::MOVE_UP));
	InstructionSets_[0].push_back(WaveInstruction(11, InstructionState::SWITCH_OFF, Instruction::MOVE_UP));
	InstructionSets_[0].push_back(WaveInstruction(12, InstructionState::SWITCH_ON, Instruction::MOVE_DOWN));
	InstructionSets_[0].push_back(WaveInstruction(15, InstructionState::SWITCH_OFF, Instruction::MOVE_DOWN));
	InstructionSets_[0].push_back(WaveInstruction(16, InstructionState::SWITCH_ON, Instruction::MOVE_UP));
	InstructionSets_[0].push_back(WaveInstruction(16, InstructionState::SWITCH_ON, Instruction::MOVE_RIGHT));
	InstructionSets_[0].push_back(WaveInstruction(19, InstructionState::SWITCH_OFF, Instruction::MOVE_RIGHT));

}

//////////////////////////////////////////////////////////////////////////
// Dtor
GaShipProcessor::~GaShipProcessor()
{
}

//////////////////////////////////////////////////////////////////////////
// updateShips
void GaShipProcessor::updatePlayers(const ScnComponentList& Components)
{
	float dt = SysKernel::pImpl()->getFrameTime();
	float totalScore = 0.0f;
	// Iterate over all the ships.
	for (BcU32 Idx = 0; Idx < Players_.size(); ++Idx)
	{
		auto* ShipComponent = Players_[Idx];
		int Set = ShipComponent->InstructionSet_;
		int Step = ShipComponent->CurrentStep_;
		ShipComponent->Score_ += dt;
		totalScore += ShipComponent->Score_;
		// Update AI Ships setting. This will currently eventually throw an error
		ShipComponent->TimeOffset_ += dt;
		while (ShipComponent->Instructions_.size() > ShipComponent->CurrentStep_) {
			ShipComponent->Instructions_[ShipComponent->CurrentStep_].Offset_ = ShipComponent->TimeOffset_;
			if (ShipComponent->Instructions_[ShipComponent->CurrentStep_].State_ == InstructionState::SWITCH_ON)
			{
				ShipComponent->CurrentInstructions_ |= ShipComponent->Instructions_[ShipComponent->CurrentStep_].Instruction_;
			}
			else
			{
				Instruction inverse = Instruction::ALL ^ ShipComponent->Instructions_[ShipComponent->CurrentStep_].Instruction_;
				ShipComponent->CurrentInstructions_ &= inverse;
			}
			ShipComponent->CurrentStep_++;
		}
		
	}
	if (GameComponent_)
		GameComponent_->SetScore(totalScore);
	GaTitleProcessor::pImpl()->setScore(totalScore);
}
//////////////////////////////////////////////////////////////////////////
// updateShips
void GaShipProcessor::updateEnemies(const ScnComponentList& Components)
{
	float dt = SysKernel::pImpl()->getFrameTime();
	// Iterate over all the ships.
	for (BcU32 Idx = 0; Idx < Components.size(); ++Idx)
	{
		auto Component = Components[Idx];
		BcAssert(Component->isTypeOf< GaShipComponent >());
		auto* ShipComponent = static_cast< GaShipComponent* >(Component.get());
		int Set = ShipComponent->InstructionSet_;
		int Step = ShipComponent->CurrentStep_;
		// Update AI Ships setting. This will currently eventually throw an error
		ShipComponent->TimeOffset_ += dt;
		if (ShipComponent->IsPlayer_)
		{
			continue;
		}
		while (InstructionSets_[Set][Step].Offset_ < ShipComponent->TimeOffset_) {
			if (InstructionSets_[Set][Step].State_ == InstructionState::SWITCH_ON)
			{
				ShipComponent->CurrentInstructions_ |= InstructionSets_[Set][Step].Instruction_;
			}
			else
			{
				Instruction inverse = Instruction::ALL ^ InstructionSets_[Set][Step].Instruction_;
				ShipComponent->CurrentInstructions_ &= inverse;
			}
			ShipComponent->CurrentStep_++;
			if (ShipComponent->CurrentStep_ == InstructionSets_[Set].size())
			{
				ShipComponent->CurrentStep_ = 0;
				ShipComponent->TimeOffset_ = -2;
				ShipComponent->CurrentInstructions_ = Instruction::NONE;
			}
			Step = ShipComponent->CurrentStep_;
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// updateShipPositions
void GaShipProcessor::updateShipPositions(const ScnComponentList& Components)
{
	float dt = SysKernel::pImpl()->getFrameTime();
	// Iterate over all the ships.
	for (BcU32 Idx = 0; Idx < Components.size(); ++Idx)
	{
		auto Component = Components[Idx];
		BcAssert(Component->isTypeOf< GaShipComponent >());
		auto* ShipComponent = static_cast< GaShipComponent* >(Component.get());
		// Ship Movement. Applies to all ships
		float X = 0;
		float Z = 0;
		Instruction instr = ShipComponent->CurrentInstructions_;
		if (BcContainsAnyFlags(instr, Instruction::MOVE_LEFT))
			X -= 1.0f;
		if (BcContainsAnyFlags(instr, Instruction::MOVE_RIGHT))
			X += 1.0f;
		if (BcContainsAnyFlags(instr, Instruction::MOVE_UP))
			Z += 1.0f;
		if (BcContainsAnyFlags(instr, Instruction::MOVE_DOWN))
			Z -= 1.0f;
		MaVec3d movement(X, 0, Z);
		movement.normalise();
		movement.x(movement.x() * ShipComponent->Speed_);
		movement.z(movement.z() * ShipComponent->Speed_ + ShipComponent->ZSpeed_);
		MaVec3d oldPos = ShipComponent->getParentEntity()->getWorldPosition();
		MaVec3d newPos = oldPos + movement * dt;

		if (newPos.x() < MinConstraint_.x())
			newPos.x(MinConstraint_.x());
		else if (newPos.x() > MaxConstraint_.x())
			newPos.x(MaxConstraint_.x());
		if (newPos.z() > MaxConstraint_.z())
		{
			if (ShipComponent->IsPlayer_)
			{
				newPos.z(MaxConstraint_.z());
			}
			else
			{
				newPos.z(oldPos.z() + ShipComponent->ZSpeed_);
			}
		}
		if (newPos.z() < MinConstraint_.z())
		{
			if (ShipComponent->IsPlayer_)
			{
				newPos.z(MinConstraint_.z());
			}
			else if(newPos.z() < MinConstraint_.z() - 10)
			{
				ScnCore::pImpl()->removeEntity(ShipComponent->getParentEntity());
			}
		}
		ShipComponent->getParentEntity()->setWorldPosition(newPos);
	}
}

//////////////////////////////////////////////////////////////////////////
// fireWeapons
void GaShipProcessor::fireWeapons(const ScnComponentList& Components)
{
	float dt = SysKernel::pImpl()->getFrameTime();
	// Iterate over all the ships.
	for (BcU32 Idx = 0; Idx < Components.size(); ++Idx)
	{
		auto Component = Components[Idx];
		BcAssert(Component->isTypeOf< GaShipComponent >());
		auto* ShipComponent = static_cast< GaShipComponent* >(Component.get());
		ShipComponent->TimeToShoot_ += dt;
		if (ShipComponent->TimeToShoot_ >= ShipComponent->FireRate_)
		{
			if ((ShipComponent->CurrentInstructions_ & Instruction::SHOOT) != Instruction::NONE)
			{
				ShipComponent->TimeToShoot_ -= ShipComponent->FireRate_;
				float Z = -1.0f;
				if (ShipComponent->IsPlayer_)
					Z = 1.0f;
				for (int Idx = 0; Idx < ShipComponent->GunNodeIndices_.size(); ++Idx)
				{
					int index = ShipComponent->GunNodeIndices_[Idx];
					MaMat4d mat = ShipComponent->Model_->getNodeWorldTransform(index);;
					ScnEntity* ent = ScnCore::pImpl()->spawnEntity(
						ScnEntitySpawnParams(
							"Bullet_0", "ships", "WeaponBullet",
							mat, ShipComponent->getParentEntity()->getParentEntity()));
					auto* bullet = ent->getComponentByType<GaBulletComponent>();
					bullet->setDirection(MaVec3d(0, 0, Z));
					bullet->setShip(ShipComponent);
				}

				if( ShipComponent->LaunchSounds_.size() > 0 )
				{
					auto Emitter = ShipComponent->getComponentByType< ScnSoundEmitterComponent >();
					BcAssert( Emitter );
					Emitter->playOneShot( ShipComponent->LaunchSounds_[ BcRandom::Global.randRange( 0, ShipComponent->LaunchSounds_.size() - 1 ) ] );
				}
			}
			else
			{
				ShipComponent->TimeToShoot_ = ShipComponent->FireRate_;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
// initialise
void GaShipProcessor::initialise()
{
	OsCore::pImpl()->subscribe(osEVT_INPUT_KEYDOWN, this,
		[this] (EvtID ID, const EvtBaseEvent& InEvent) {
			const auto &Event = InEvent.get<OsEventInputKeyboard>();
			this->processInput(Event.AsciiCode_, InstructionState::SWITCH_ON);
			return evtRET_PASS;
		});
	OsCore::pImpl()->subscribe(osEVT_INPUT_KEYUP, this,
		[this](EvtID ID, const EvtBaseEvent& InEvent) {
		const auto &Event = InEvent.get<OsEventInputKeyboard>();
		this->processInput(Event.AsciiCode_, InstructionState::SWITCH_OFF);
		return evtRET_PASS;
	});
	OsCore::pImpl()->subscribe(gaEVT_START_WAVE, this,
		[this](EvtID ID, const EvtBaseEvent& InEvent) {
			this->startWave();
			return evtRET_PASS;
		}
	);
	OsCore::pImpl()->subscribe(gaEVT_END_WAVE, this,
		[this](EvtID ID, const EvtBaseEvent& InEvent) {
		this->endWave();
		return evtRET_PASS;
	}
	);

}

void GaShipProcessor::startWave()
{
	for (int i = 0; i < Players_.size(); ++i)
	{
		Players_[i]->Instructions_.clear();
		Players_[i]->TimeOffset_ = 0.0f;
		Players_[i]->CurrentStep_ = 0;
		if (( Players_[i]->CurrentInstructions_ & Instruction::SHOOT) != Instruction::NONE)
		{
			Players_[i]->Instructions_.push_back(WaveInstruction(0.0f, InstructionState::SWITCH_ON, Instruction::SHOOT));
		}
	}
}

void GaShipProcessor::endWave()
{
	for (int i = 0; i < Players_.size(); ++i)
	{
		if (Players_[i]->Instructions_.size() > 0)
			InstructionSets_.push_back(std::vector<WaveInstruction>(Players_[i]->Instructions_));
		Players_[i]->Instructions_.clear();
		Players_[i]->CurrentStep_ = 0;
	}
}

void GaShipProcessor::processInput(BcU32 AsciiCode, InstructionState State)
{
	int player = 0;
	Instruction instr = Instruction::NONE;
	switch (AsciiCode)
	{
	case 'a':
	case 'A':
		instr = Instruction::MOVE_LEFT;
		break;
	case 'd':
	case 'D':
		instr = Instruction::MOVE_RIGHT;
		break;
	case 'w':
	case 'W':
		instr = Instruction::MOVE_UP;
		break;
	case 's':
	case 'S':
		instr = Instruction::MOVE_DOWN;
		break;
	case ' ':
		instr = Instruction::SHOOT;
		break;

	default:
		break;
	}
	if (player < Players_.size())
		Players_[player]->Instructions_.push_back(WaveInstruction(-0.0f, State, instr));

}

//////////////////////////////////////////////////////////////////////////
// shutdown
void GaShipProcessor::shutdown()
{

}

//////////////////////////////////////////////////////////////////////////
// addPlayer
void GaShipProcessor::addPlayer(GaShipComponent* Player)
{
	Players_.push_back(Player);
}

void GaShipProcessor::removePlayer(GaShipComponent * Player)
{
	for (int i = 0; i < Players_.size(); ++i) {
		if (Players_[i] == Player) {
			Players_[i] = nullptr;
		}
	}
}


//////////////////////////////////////////////////////////////////////////
// addPlayer
void GaShipProcessor::registerGame(GaGameComponent* Game) 
{
	GameComponent_ = Game;
	Players_.clear();
	MinConstraint_ = Game->getConstraintMin();
	MaxConstraint_ = Game->getConstraintMax();
}

//////////////////////////////////////////////////////////////////////////
// addPlayer
void GaShipProcessor::deregisterGame(GaGameComponent* Game)
{
	if ( Game == GameComponent_)
		GameComponent_ = nullptr;
}

//////////////////////////////////////////////////////////////////////////
// requestInstructions
void GaShipProcessor::requestInstructions(GaShipComponent* ShipComponent) {
	ShipComponent->InstructionSet_ = BcRandom::Global.rand() % InstructionSets_.size();
}

//////////////////////////////////////////////////////////////////////////
// StaticRegisterClass
REFLECTION_DEFINE_DERIVED( GaShipComponent );

void GaShipComponent::StaticRegisterClass()
{
	ReField* Fields[] = 
	{
		new ReField( "IsPlayer_", &GaShipComponent::IsPlayer_, bcRFF_IMPORTER ),
		new ReField("InstructionSet_", &GaShipComponent::InstructionSet_, bcRFF_IMPORTER),
		new ReField("CurrentStep_", &GaShipComponent::CurrentStep_, bcRFF_TRANSIENT),
		new ReField("CurrentInstructions_", &GaShipComponent::CurrentInstructions_, bcRFF_TRANSIENT),
		new ReField("Speed_", &GaShipComponent::Speed_, bcRFF_IMPORTER),
		new ReField("ZSpeed_", &GaShipComponent::ZSpeed_, bcRFF_IMPORTER),
		new ReField("TimeOffset_", &GaShipComponent::TimeOffset_, bcRFF_TRANSIENT),
		new ReField( "Model_", &GaShipComponent::Model_, bcRFF_TRANSIENT ),
		new ReField( "GunNodeIndices_", &GaShipComponent::GunNodeIndices_, bcRFF_TRANSIENT ),
		new ReField( "EngineNodeIndices_", &GaShipComponent::EngineNodeIndices_, bcRFF_TRANSIENT ),
		new ReField( "FireRate_", &GaShipComponent::FireRate_, bcRFF_IMPORTER),
		new ReField( "TimeToShoot_", &GaShipComponent::FireRate_, bcRFF_IMPORTER),
		new ReField( "EngineSound_", &GaShipComponent::EngineSound_, bcRFF_IMPORTER | bcRFF_SHALLOW_COPY),
		new ReField( "LaunchSounds_", &GaShipComponent::LaunchSounds_, bcRFF_IMPORTER | bcRFF_SHALLOW_COPY),
		new ReField( "Score_", &GaShipComponent::Score_, bcRFF_IMPORTER),
	};
	
	using namespace std::placeholders;
	ReRegisterClass< GaShipComponent, Super >( Fields )
		.addAttribute( new GaShipProcessor() );
}

//////////////////////////////////////////////////////////////////////////
// Ctor
GaShipComponent::GaShipComponent()
	: InstructionSet_(-1),
	CurrentInstructions_(Instruction::NONE),
	CurrentStep_(0),
	ZSpeed_(0),
	Speed_(15),
	TimeOffset_(0),
	TimeToShoot_(0)
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
	if (IsPlayer_)
		GaShipProcessor::pImpl()->addPlayer(this);
	if (!IsPlayer_) {
		GaShipProcessor::pImpl()->requestInstructions(this);
		ZSpeed_ = -20.0f;
	}
	GaBulletProcessor::pImpl()->addShip(this);
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
	TimeToShoot_ = BcRandom::Global.randRealRange(0.0f, 0.5f);

	if( EngineSound_ )
	{
		auto Emitter = getComponentByType< ScnSoundEmitterComponent >();
		BcAssert( Emitter );
		Emitter->play( EngineSound_, true );
	}
}

//////////////////////////////////////////////////////////////////////////
// onDetach
void GaShipComponent::onDetach( ScnEntityWeakRef Parent )
{
	Super::onDetach( Parent );
	GaBulletProcessor::pImpl()->removeShip(this);
	GaShipDestroyedEvent Event;
	Event.IsPlayer_ = false;
	OsCore::pImpl()->publish(gaEVT_SHIP_DESTROYED, Event);

}

void GaShipComponent::addScore(float Score)
{
	Score_ += Score;
}

BcBool GaShipComponent::IsPlayer()
{
	return IsPlayer_;
}

