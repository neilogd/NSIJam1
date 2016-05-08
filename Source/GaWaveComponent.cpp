#include "GaWaveComponent.h"
#include "Base/BcRandom.h"
#include "System/Scene/ScnEntity.h"
#include "System/Scene/Rendering/ScnModel.h"

#include "System/Os/OsCore.h"
#include "System/Os/OsEvents.h"

#include "System/Debug/DsImGui.h"

#include "GaEvents.h"
#include "GaGameComponent.h"

//////////////////////////////////////////////////////////////////////////
// StaticRegisterClass
REFLECTION_DEFINE_DERIVED( GaWaveComponent );

void GaWaveComponent::StaticRegisterClass()
{
	ReField* Fields[] = 
	{
		new ReField("CurrentWave_", &GaWaveComponent::CurrentWave_, bcRFF_IMPORTER),
		new ReField( "WaveCountdownTime_", &GaWaveComponent::WaveCountdownTime_, bcRFF_IMPORTER ),
	};
	
	using namespace std::placeholders;
	ReRegisterClass< GaWaveComponent, Super >( Fields )
		.addAttribute(new ScnComponentProcessor(
		{
			ScnComponentProcessFuncEntry::Update< GaWaveComponent >()
		}));
}

//////////////////////////////////////////////////////////////////////////
// Ctor
GaWaveComponent::GaWaveComponent()
	: CurrentWave_(1),
	ShipCount_(0),
	NewWaveCountdown_(0),
	CountdownWave_(BcTrue),
	WaveCountdownTime_(5.0f)
{
}

//////////////////////////////////////////////////////////////////////////
// Dtor
GaWaveComponent::~GaWaveComponent()
{
}
	
//////////////////////////////////////////////////////////////////////////
// onAttach
void GaWaveComponent::onAttach( ScnEntityWeakRef Parent )
{
	Super::onAttach( Parent );
	GameComponent_ = ParentEntity_->getComponentByType<GaGameComponent>();

	OsCore::pImpl()->subscribe(gaEVT_SHIP_DESTROYED, this,
		[this](EvtID ID, const EvtBaseEvent& InEvent) {
		const auto &Event = InEvent.get<GaShipDestroyedEvent>();
		if (!Event.IsPlayer_)
			this->removeShip();
		return evtRET_PASS;
	}
	);
}

//////////////////////////////////////////////////////////////////////////
// onDetach
void GaWaveComponent::onDetach( ScnEntityWeakRef Parent )
{
	Super::onDetach( Parent );
}

int GaWaveComponent::getEnemySpawnCount()
{
	return CurrentWave_;
}

float GaWaveComponent::getShipOffset(int Ship)
{
	float min = GameComponent_->getConstraintMin().x();
	float max = GameComponent_->getConstraintMax().x();
	float enemies = getEnemySpawnCount();
	float d = ((float)Ship + 1.0f) / (enemies + 1.0f);

	return min * (1.0f - d) + max * d;
}

float GaWaveComponent::getShipOffsetY(int Ship)
{
	float min = -0.3f;
	float max = 0.3f;
	float d = BcRandom::Global.randReal();

	return min * (1.0f - d) + max * d;
}

float GaWaveComponent::getRandomOffset(float R)
{
	float min = 0.0f;
	float max = R;
	float d = BcRandom::Global.randReal();

	return min * (1.0f - d) + max * d;
}

void GaWaveComponent::update(float Tick)
{
	if (CountdownWave_)
	{
		NewWaveCountdown_ -= Tick;
		if (NewWaveCountdown_ <= 0.0f)
		{
			CountdownWave_ = false;
			StartWave();
		}
	}

#if !PSY_PRODUCTION
	if (ImGui::Begin("Wave Debug Menu"))
	{
		ImGui::Separator();
		ImGui::BeginGroup();

		if (ImGui::Button("Start New Wave"))
		{
			StartWave();
		}
		if (ImGui::Button("End Wave"))
		{
			EndWave();
		}
		ImGui::EndGroup();
		ImGui::Separator();
	}
	ImGui::End();
#endif
}

void GaWaveComponent::StartWave()
{
	GaEventWave Event;
	Event.StartWave_ = true;
	OsCore::pImpl()->publish(gaEVT_START_WAVE, Event);
	ShipCount_ += getEnemySpawnCount();
	for (int i = 0; i < getEnemySpawnCount(); ++i)
	{
		int ship = BcRandom::Global.rand() % GameComponent_->EnemyShipTemplates_.size();
		MaMat4d mat;
		mat.translation(MaVec3d(getShipOffset(i), getRandomOffset(1.0f), GameComponent_->getConstraintMax().z() + getRandomOffset(5.0f)));
		ScnCore::pImpl()->spawnEntity(
			ScnEntitySpawnParams(
				"EnemyShip_0", GameComponent_->EnemyShipTemplates_[ship],
				mat, getParentEntity()));
	}
}

void GaWaveComponent::EndWave()
{
	GaEventWave Event;
	Event.StartWave_ = false;
	OsCore::pImpl()->publish(gaEVT_END_WAVE, Event);
	++CurrentWave_;
	CountdownWave_ = BcTrue;
	NewWaveCountdown_ = WaveCountdownTime_;
}

void GaWaveComponent::removeShip()
{
	ShipCount_ -= 1;
	if (ShipCount_ <= 0) {
		ShipCount_ = 0; // Because I'm hella paranoid
		EndWave();
		NewWaveCountdown_ = 5.0f;
	}
}
