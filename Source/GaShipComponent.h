#pragma once

#include "System/Renderer/RsCore.h"
#include "System/Renderer/RsBuffer.h"
#include "System/Renderer/RsRenderNode.h"
#include "System/Scene/ScnComponent.h"
#include "System/Scene/ScnComponentProcessor.h"

#include "System/Scene/Sound/ScnSound.h"

#include "GaEvents.h"

#include <vector>

enum class InstructionState 
{
	SWITCH_ON,
	SWITCH_OFF
};

enum class Instruction 
{
	NONE			= 0x0000,
	MOVE_LEFT		= 0x0001,
	MOVE_RIGHT		= 0x0002,
	MOVE_UP			= 0x0004,
	MOVE_DOWN		= 0x0008,
	SHOOT			= 0x0010,


	ALL				= 0xFFFF
};

DEFINE_ENUM_CLASS_FLAG_OPERATOR(Instruction, |);
DEFINE_ENUM_CLASS_FLAG_OPERATOR(Instruction, &);
DEFINE_ENUM_CLASS_FLAG_OPERATOR(Instruction, ^);

class WaveInstruction 
{
	public:
	WaveInstruction(double Offset, InstructionState State, Instruction Instruction) 
		:Offset_(Offset), State_(State), Instruction_(Instruction)
	{
	}
	double Offset_;
	InstructionState State_;
	Instruction Instruction_;
};

class GaShipComponent;
class GaGameComponent;
//////////////////////////////////////////////////////////////////////////
// GaShipProcessor
class GaShipProcessor : 
	public BcGlobal< GaShipProcessor >,
	public ScnComponentProcessor
{
public:
	GaShipProcessor();
	virtual ~GaShipProcessor();

	void updatePlayers(const ScnComponentList& Components);
	void updateEnemies( const ScnComponentList& Components );
	void updateShipPositions(const ScnComponentList& Components);
	void fireWeapons(const ScnComponentList& Components);
	void processInput(BcU32 AsciiCode, InstructionState State);

	void addPlayer(GaShipComponent* Player);
	void removePlayer(GaShipComponent* Player);
	void requestInstructions(GaShipComponent* ShipComponent);
	void startWave();
	void endWave();

	void registerGame(GaGameComponent* Game);
	void deregisterGame(GaGameComponent* Game);

protected:
	void initialise() override;
	void shutdown() override;

private:
	std::vector<std::vector<WaveInstruction>> InstructionSets_;
	std::vector<GaShipComponent*> Players_;
	MaVec3d MinConstraint_;
	MaVec3d MaxConstraint_;
	GaGameComponent* GameComponent_;
};

//////////////////////////////////////////////////////////////////////////
// GaShipComponent
class GaShipComponent:
	public ScnComponent
{
public:
	REFLECTION_DECLARE_DERIVED( GaShipComponent, ScnComponent );

	GaShipComponent();
	virtual ~GaShipComponent();
	
	void onAttach( ScnEntityWeakRef Parent ) override;
	void onDetach( ScnEntityWeakRef Parent ) override;

private:
	friend class GaShipProcessor;

	BcBool IsPlayer_ = BcFalse;
	int InstructionSet_;
	int CurrentStep_;
	Instruction CurrentInstructions_;

	float Speed_;
	float ZSpeed_;

	float TimeOffset_;
	float FireRate_;
	float TimeToShoot_;
	float Score_;

	class ScnModelComponent* Model_ = nullptr;
	std::vector< BcU32 > GunNodeIndices_;
	std::vector< BcU32 > EngineNodeIndices_;
	std::vector< WaveInstruction > Instructions_;

	ScnSoundRef EngineSound_;
	std::vector< ScnSoundRef > LaunchSounds_;
};
