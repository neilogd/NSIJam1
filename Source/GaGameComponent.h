#pragma once

#include "Psybrus.h"
#include "System/Scene/ScnComponent.h"

#include "System/Scene/Rendering/ScnModel.h"

//////////////////////////////////////////////////////////////////////////
// GaGameComponent
class GaGameComponent:
	public ScnComponent
{
public:
	REFLECTION_DECLARE_DERIVED( GaGameComponent, ScnComponent );

	GaGameComponent();
	virtual ~GaGameComponent();
	
	void update( BcF32 Tick );

	void onAttach( ScnEntityWeakRef Parent ) override;
	void onDetach( ScnEntityWeakRef Parent ) override;
	
private:
};
