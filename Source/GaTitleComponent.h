#pragma once

#include "System/Renderer/RsCore.h"
#include "System/Renderer/RsBuffer.h"
#include "System/Renderer/RsRenderNode.h"
#include "System/Scene/ScnComponent.h"
#include "System/Scene/ScnComponentProcessor.h"

#include "GaEvents.h"

#include <vector>

class GaTitleComponent;
class GaGameComponent;
class GaShipComponent;
//////////////////////////////////////////////////////////////////////////
// GaTitleProcessor
class GaTitleProcessor : 
	public BcGlobal< GaTitleProcessor >,
	public ScnComponentProcessor
{
public:
	GaTitleProcessor();
	virtual ~GaTitleProcessor();

	void updateTitle(const ScnComponentList& Components);
	void setScore(float Score);
	void showTitle();
protected:
	void initialise() override;
	void shutdown() override;

private:

	int Score_;
	BcBool ShowTitle_;
};

//////////////////////////////////////////////////////////////////////////
// GaTitleComponent
class GaTitleComponent:
	public ScnComponent
{
public:
	REFLECTION_DECLARE_DERIVED( GaTitleComponent, ScnComponent );

	GaTitleComponent();
	virtual ~GaTitleComponent();
	
	void onAttach( ScnEntityWeakRef Parent ) override;
	void onDetach( ScnEntityWeakRef Parent ) override;

private:
	int Number_;
	BcBool StartGame_ = BcFalse;
	BcBool TitleClosed_ = BcFalse;
	friend class GaTitleProcessor;
};
