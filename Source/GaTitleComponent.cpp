#include "GaTitleComponent.h"

#include "Base/BcRandom.h"
#include "System/Scene/ScnEntity.h"
#include "System/Scene/Rendering/ScnModel.h"
#include "System/Scene/Rendering/ScnViewComponent.h"

#include "System/Scene/Rendering/ScnDebugRenderComponent.h"

#include "System/Scene/Rendering/ScnCanvasComponent.h"
#include "System/Scene/Rendering/ScnFont.h"

#include "System/Os/OsCore.h"
#include "System/Os/OsEvents.h"
#include "System/SysKernel.h"
#include "GaGameComponent.h"
#include "GaShipComponent.h"
//////////////////////////////////////////////////////////////////////////
// Ctor
GaTitleProcessor::GaTitleProcessor():
	ScnComponentProcessor( {
	ScnComponentProcessFuncEntry(
		"Update Update",
		ScnComponentPriority::DEFAULT_UPDATE,
		std::bind(&GaTitleProcessor::updateTitle, this, std::placeholders::_1)),
	} ),
	Score_(0),
	ShowTitle_(BcFalse)
{

}

//////////////////////////////////////////////////////////////////////////
// Dtor
GaTitleProcessor::~GaTitleProcessor()
{
}


//////////////////////////////////////////////////////////////////////////
// updateBullets
void GaTitleProcessor::updateTitle(const ScnComponentList& Components)
{
	float dt = SysKernel::pImpl()->getFrameTime();
	static float Time = 0.0f;
	Time += dt;
	// Iterate over all the ships.
	for (BcU32 Idx = 0; Idx < Components.size(); ++Idx)
	{
		auto Component = Components[Idx];
		BcAssert(Component->isTypeOf< GaTitleComponent >());
		auto* TitleComponent = static_cast< GaTitleComponent* >(Component.get());
		if (ShowTitle_) {
			TitleComponent->TitleClosed_ = false;
			ShowTitle_ = false;
		}
		if (TitleComponent->TitleClosed_)
		{
			return; 
		}
		if (TitleComponent->StartGame_)
		{ 
			TitleComponent->StartGame_ = BcFalse;
			TitleComponent->TitleClosed_ = BcTrue;
			ScnCore::pImpl()->spawnEntity(
				ScnEntitySpawnParams(
					"MainEntity_0", "default", "MainEntity",
					MaMat4d(), nullptr));

			// CAUSES A CRASH AAAAHHH: ScnCore::pImpl()->removeEntity(TitleComponent->getParentEntity());
			return;
		}


		ScnFontComponent* Font = Component->getComponentAnyParentByType< ScnFontComponent >();
		ScnCanvasComponent* Canvas = Component->getComponentAnyParentByType< ScnCanvasComponent >();

		OsClient* Client = OsCore::pImpl()->getClient(0);
		BcF32 HalfWidth = static_cast< BcF32 >(Client->getWidth() / 2);
		BcF32 HalfHeight = static_cast< BcF32 >(Client->getHeight() / 2);

		MaMat4d Projection;
		Projection.orthoProjection(-HalfWidth, HalfWidth, HalfHeight, -HalfHeight, -1.0f, 1.0f);
		// Hack to fix nullptr exception


		Canvas->clear();
		Canvas->pushMatrix(Projection);

		ScnFontDrawParams DrawParams = ScnFontDrawParams()
			.setSize(40.0f)
			.setMargin(8.0f)
			.setAlignment(ScnFontAlignment::HCENTRE | ScnFontAlignment::TOP)
			.setWrappingEnabled(BcTrue)
			.setTextColour(RsColour::WHITE)
			.setBorderColour(RsColour::BLACK)
			.setTextSettings(MaVec4d(0.4f, 0.5f, 0.0f, 0.0f))
			.setBorderSettings(MaVec4d(1.0f, 0.0f, 0.0f, 0.0f));
		ScnFontDrawParams TitleDrawParams = ScnFontDrawParams()
			.setSize(128.0f)
			.setMargin(8.0f)
			.setAlignment(ScnFontAlignment::HCENTRE | ScnFontAlignment::VCENTRE)
			.setWrappingEnabled(BcTrue)
			.setTextColour(RsColour::WHITE)
			.setBorderColour(RsColour::BLACK)
			.setTextSettings(MaVec4d(0.4f, 0.5f, 0.0f, 0.0f))
			.setBorderSettings(MaVec4d(1.0f, 0.0f, 0.0f, 0.0f));
		ScnFontDrawParams TeamDrawParams = ScnFontDrawParams()
			.setSize(40.0f)
			.setMargin(8.0f)
			.setAlignment(ScnFontAlignment::HCENTRE | ScnFontAlignment::BOTTOM)
			.setWrappingEnabled(BcTrue)
			.setTextColour(RsColour::WHITE)
			.setBorderColour(RsColour::BLACK)
			.setTextSettings(MaVec4d(0.4f, 0.5f, 0.0f, 0.0f))
			.setBorderSettings(MaVec4d(1.0f, 0.0f, 0.0f, 0.0f));
		char buffer[256];
		Font->drawText(
			Canvas,
			TitleDrawParams,
			MaVec2d(-HalfWidth, -HalfHeight),
			MaVec2d(HalfWidth, HalfHeight) * 2.0f,
			L"Potato DogFight");
		if (Score_ > 0) 
		{
			Font->drawText(
				Canvas,
				DrawParams,
				MaVec2d(-HalfWidth, -HalfHeight),
				MaVec2d(HalfWidth, HalfHeight) * 2.0f,
				buffer);
		}
		Font->drawText(
			Canvas,
			DrawParams,
			MaVec2d(-HalfWidth, HalfHeight / 3),
			MaVec2d(HalfWidth, HalfHeight) * 2.0f,
			L"Press e to Play\nWASD to move\nSpace to Shoot");
		Font->drawText(
			Canvas,
			TeamDrawParams,
			MaVec2d(-HalfWidth, -HalfHeight),
			MaVec2d(HalfWidth, HalfHeight) * 2.0f,
			"Team Didja Putit In Jira\nDezzles - NeiloGD");
		Canvas->popMatrix();
	}
}

void GaTitleProcessor::setScore(float Score)
{
	Score_ = Score;
}

void GaTitleProcessor::showTitle()
{
	ShowTitle_ = BcTrue;
}

//////////////////////////////////////////////////////////////////////////
// initialise
void GaTitleProcessor::initialise()
{
}


//////////////////////////////////////////////////////////////////////////
// shutdown
void GaTitleProcessor::shutdown()
{

}

//////////////////////////////////////////////////////////////////////////
// StaticRegisterClass
REFLECTION_DEFINE_DERIVED( GaTitleComponent );

void GaTitleComponent::StaticRegisterClass()
{
	ReField* Fields[] = 
	{
		new ReField( "Number_", &GaTitleComponent::Number_, bcRFF_IMPORTER ),
		new ReField( "StartGame_", &GaTitleComponent::StartGame_, bcRFF_IMPORTER ),
	};
	
	using namespace std::placeholders;
	ReRegisterClass< GaTitleComponent, Super >( Fields )
		.addAttribute( new GaTitleProcessor() );
}

//////////////////////////////////////////////////////////////////////////
// Ctor
GaTitleComponent::GaTitleComponent()
	: Number_(15), StartGame_(BcFalse)
{
}

//////////////////////////////////////////////////////////////////////////
// Dtor
GaTitleComponent::~GaTitleComponent()
{
}
	
//////////////////////////////////////////////////////////////////////////
// onAttach
void GaTitleComponent::onAttach( ScnEntityWeakRef Parent )
{
	Super::onAttach( Parent );
	OsCore::pImpl()->subscribe(osEVT_INPUT_KEYDOWN, this,
		[this](EvtID ID, const EvtBaseEvent& InEvent) {
		const auto &Event = InEvent.get<OsEventInputKeyboard>();
		if ((Event.AsciiCode_ == 'E') || (Event.AsciiCode_ == 'e'))
		{
			this->StartGame_ = BcTrue;
		}
		return evtRET_PASS;
	});

}

//////////////////////////////////////////////////////////////////////////
// onDetach
void GaTitleComponent::onDetach( ScnEntityWeakRef Parent )
{
	Super::onDetach( Parent );
}

