#pragma once

#include "System/Renderer/RsCore.h"
#include "System/Renderer/RsBuffer.h"
#include "System/Renderer/RsRenderNode.h"
#include "System/Scene/ScnComponent.h"
#include "System/Scene/ScnComponentProcessor.h"

#include "GaEvents.h"

#include <vector>

class GaBulletComponent;
class GaGameComponent;
//////////////////////////////////////////////////////////////////////////
// GaBulletProcessor
class GaBulletProcessor : 
	public BcGlobal< GaBulletProcessor >,
	public ScnComponentProcessor
{
public:
	GaBulletProcessor();
	virtual ~GaBulletProcessor();

	void updateBullets(const ScnComponentList& Components);
	void registerGame(GaGameComponent* Game);
	void deregisterGame(GaGameComponent* Game);
protected:
	void initialise() override;
	void shutdown() override;

private:
	MaVec3d MinConstraint_;
	MaVec3d MaxConstraint_;
	GaGameComponent* GameComponent_;
};

//////////////////////////////////////////////////////////////////////////
// GaBulletComponent
class GaBulletComponent:
	public ScnComponent
{
public:
	REFLECTION_DECLARE_DERIVED( GaBulletComponent, ScnComponent );

	GaBulletComponent();
	virtual ~GaBulletComponent();
	
	void onAttach( ScnEntityWeakRef Parent ) override;
	void onDetach( ScnEntityWeakRef Parent ) override;
	void setDirection(MaVec3d Direction);


private:
	friend class GaBulletProcessor;

	float Speed_;
	MaVec3d Direction_;
	class ScnModelComponent* Model_ = nullptr;
};
