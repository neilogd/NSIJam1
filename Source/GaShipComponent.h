#pragma once

#include "System/Renderer/RsCore.h"
#include "System/Renderer/RsBuffer.h"
#include "System/Renderer/RsRenderNode.h"
#include "System/Scene/ScnComponent.h"
#include "System/Scene/ScnComponentProcessor.h"

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
};
