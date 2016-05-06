#pragma once

#include "System/Renderer/RsCore.h"
#include "System/Renderer/RsBuffer.h"
#include "System/Renderer/RsRenderNode.h"
#include "System/Scene/ScnComponent.h"
#include "System/Scene/ScnComponentProcessor.h"

#include <vector>

enum class InstructionState 
{
	SWITCH_ON,
	SWITCH_OFF
};

enum class Instruction 
{
	MOVE_LEFT,
	MOVE_RIGHT,
	MOVE_UP,
	MOVE_DOWN,
	SHOOT
};

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

//////////////////////////////////////////////////////////////////////////
// GaShipProcessor
class GaShipProcessor : 
	public BcGlobal< GaShipProcessor >,
	public ScnComponentProcessor
{
public:
	GaShipProcessor();
	virtual ~GaShipProcessor();

	void updateShips( const ScnComponentList& Components );	

protected:
	void initialise() override;
	void shutdown() override;

private:
	std::vector<std::vector<WaveInstruction>> InstructionSets_;
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

	int InstructionSet_ = 0;

	class ScnModelComponent* Model_ = nullptr;
	std::vector< BcU32 > GunNodeIndices_;
	std::vector< BcU32 > EngineNodeIndices_;
};
