#pragma once

#include "System/Scene/ScnComponent.h"
#include "System/Scene/ScnComponentProcessor.h"

#include <vector>

class GaGameComponent;
//////////////////////////////////////////////////////////////////////////
// GaWaveComponent
class GaWaveComponent:
	public ScnComponent
{
public:
	REFLECTION_DECLARE_DERIVED( GaWaveComponent, ScnComponent );

	GaWaveComponent();
	virtual ~GaWaveComponent();
	
	void onAttach( ScnEntityWeakRef Parent ) override;
	void onDetach( ScnEntityWeakRef Parent ) override;

	int getEnemySpawnCount();
	float getShipOffset(int Ship);

	void update(float Tick);

	void StartWave();
	void EndWave();

	void removeShip();
private:
	friend class GaWaveProcessor;
	int CurrentWave_;
	GaGameComponent* GameComponent_;

	int ShipCount_;
	float NewWaveCountdown_;
	BcBool CountdownWave_;
	float WaveCountdownTime_;
	float getShipOffsetY(int Ship);
	float getRandomOffset(float R);
};
