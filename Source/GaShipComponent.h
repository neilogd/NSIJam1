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
	int InstructionSet_;
	int CurrentStep_;
	Instruction CurrentInstructions_;
};
