#include "GaShipComponent.h"
#include "System/Scene/ScnEntity.h"
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
void GaShipProcessor::updateShips( const ScnComponentList& Components )
{
	static float Time = 0.0f;
	static float dt = 1 / 60.0f;
	Time += dt;
	// Iterate over all the ships.
	for( BcU32 Idx = 0; Idx < Components.size(); ++Idx )
	{
		auto Component = Components[ Idx ];
		BcAssert( Component->isTypeOf< GaShipComponent >() );
		auto* ShipComponent = static_cast< GaShipComponent* >( Component.get() );
		int Set = ShipComponent->InstructionSet_;
		int Step = ShipComponent->CurrentStep_;
		// Update AI Ships setting. This will currently eventually throw an error
		// After about 12 seconds
		if (ShipComponent->IsPlayer_ == BcFalse) {
			ShipComponent->TimeOffset_ += dt;
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
					ShipComponent->TimeOffset_ = -dt;
					ShipComponent->CurrentInstructions_ = Instruction::NONE;
				}
				Step = ShipComponent->CurrentStep_;
			}
		}
		
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
		MaVec3d newPos = ShipComponent->getParentEntity()->getWorldPosition() + movement * dt;
		ShipComponent->getParentEntity()->setWorldPosition(newPos);
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
		new ReField("InstructionSet_", &GaShipComponent::InstructionSet_, bcRFF_IMPORTER),
		new ReField("CurrentStep_", &GaShipComponent::CurrentStep_, bcRFF_TRANSIENT),
		new ReField("CurrentInstructions_", &GaShipComponent::CurrentInstructions_, bcRFF_TRANSIENT),
		new ReField("Speed_", &GaShipComponent::Speed_, bcRFF_TRANSIENT),
		new ReField("ZSpeed_", &GaShipComponent::ZSpeed_, bcRFF_TRANSIENT),
		new ReField("TimeOffset_", &GaShipComponent::TimeOffset_, bcRFF_TRANSIENT),
	};
	
	using namespace std::placeholders;
	ReRegisterClass< GaShipComponent, Super >( Fields )
		.addAttribute( new GaShipProcessor() );
}

//////////////////////////////////////////////////////////////////////////
// Ctor
GaShipComponent::GaShipComponent()
	: InstructionSet_(0),
	CurrentInstructions_(Instruction::NONE),
	CurrentStep_(0),
	ZSpeed_(0),
	Speed_(5),
	TimeOffset_(0)
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
}

//////////////////////////////////////////////////////////////////////////
// onDetach
void GaShipComponent::onDetach( ScnEntityWeakRef Parent )
{
	Super::onDetach( Parent );
}

