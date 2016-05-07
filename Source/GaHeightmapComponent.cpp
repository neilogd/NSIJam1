#include "GaHeightmapComponent.h"

#include "System/Scene/Rendering/ScnShaderFileData.h"
#include "System/Scene/Rendering/ScnViewComponent.h"
#include "System/Scene/Rendering/ScnViewRenderData.h"

#include "System/Content/CsPackage.h"
#include "System/Content/CsCore.h"

#include "System/Debug/DsImGui.h"

#include "Base/BcProfiler.h"
#include "Base/BcRandom.h"

//////////////////////////////////////////////////////////////////////////
// ScnShaderMaterialUniformBlockData
REFLECTION_DEFINE_BASIC( GaHeightmapUniformBlockData );

void GaHeightmapUniformBlockData::StaticRegisterClass()
{
	ReField* Fields[] = 
	{
		new ReField( "HeightmapUVScale_", &GaHeightmapUniformBlockData::HeightmapUVScale_ ),
		new ReField( "HeightmapDimensions_", &GaHeightmapUniformBlockData::HeightmapDimensions_ ),
		new ReField( "HeightmapHeight_", &GaHeightmapUniformBlockData::HeightmapHeight_ ),
		new ReField( "HeightmapOffset_", &GaHeightmapUniformBlockData::HeightmapOffset_ ),
		new ReField( "HeightmapPositionOffset_", &GaHeightmapUniformBlockData::HeightmapPositionOffset_ ),
		new ReField( "HeightmapUVOffset_", &GaHeightmapUniformBlockData::HeightmapUVOffset_ ),
		new ReField( "HeightmapLayer0Height_", &GaHeightmapUniformBlockData::HeightmapLayer0Height_ ),
		new ReField( "HeightmapLayer1Height_", &GaHeightmapUniformBlockData::HeightmapLayer1Height_ ),
		new ReField( "HeightmapLayer2Height_", &GaHeightmapUniformBlockData::HeightmapLayer2Height_ ),
		new ReField( "HeightmapLayer3Height_", &GaHeightmapUniformBlockData::HeightmapLayer3Height_ ),
		new ReField( "HeightmapNormalTweak_", &GaHeightmapUniformBlockData::HeightmapNormalTweak_ ),
	};
		
	auto& Class = ReRegisterClass< GaHeightmapUniformBlockData >( Fields );
	Class.addAttribute( new ScnShaderDataAttribute( "Heightmap", BcFalse ) );
	Class.setFlags( bcRFF_POD );
}

//////////////////////////////////////////////////////////////////////////
// View render data.
class GaHeightmapViewRenderData : 
	public ScnViewRenderData
{
public:
	GaHeightmapViewRenderData()
	{
	}

	virtual ~GaHeightmapViewRenderData()
	{
	}

	RsProgramBindingUPtr ProgramBinding_;
	RsRenderState* RenderState_;
};

//////////////////////////////////////////////////////////////////////////
// Define resource internals.
REFLECTION_DEFINE_DERIVED( GaHeightmapComponent );

void GaHeightmapComponent::StaticRegisterClass()
{
	ReField* Fields[] = 
	{
		new ReField( "Texture_", &GaHeightmapComponent::Texture_, bcRFF_IMPORTER | bcRFF_SHALLOW_COPY ),
		new ReField( "Material_", &GaHeightmapComponent::Material_, bcRFF_IMPORTER | bcRFF_SHALLOW_COPY ),
		new ReField( "ScrollSpeed_", &GaHeightmapComponent::ScrollSpeed_, bcRFF_IMPORTER ),
		new ReField( "Size_", &GaHeightmapComponent::Size_, bcRFF_IMPORTER ),
		new ReField( "HeightmapUniformBlock_", &GaHeightmapComponent::HeightmapUniformBlock_, bcRFF_IMPORTER ),

		new ReField( "ObjectUniformBlock_", &GaHeightmapComponent::ObjectUniformBlock_, bcRFF_IMPORTER ),
	};

	ReRegisterClass< GaHeightmapComponent, Super >( Fields )
		.addAttribute( new ScnComponentProcessor( 
			{
				ScnComponentProcessFuncEntry::Update< GaHeightmapComponent >()
			} ) );
}

//////////////////////////////////////////////////////////////////////////
// Ctor
GaHeightmapComponent::GaHeightmapComponent()
{

}

//////////////////////////////////////////////////////////////////////////
// Dtor
//virtual
GaHeightmapComponent::~GaHeightmapComponent()
{

}

//////////////////////////////////////////////////////////////////////////
// update
void GaHeightmapComponent::update( BcF32 Tick )
{
	if( ImGui::Begin( "Game Debug Menu" ) )
	{
		ImGui::Separator();
		ImGui::BeginGroup();
		ImGui::LabelText( "Heightmap settings:", "" );
		ImGui::SliderFloat( "Dimensions", &HeightmapUniformBlock_.HeightmapDimensions_, 1.0f, 256.0f, "%.3f", 2.0f );
		ImGui::SliderFloat( "Height", &HeightmapUniformBlock_.HeightmapHeight_, 0.0f, 256.0f, "%.3f", 2.0f );
		ImGui::SliderFloat( "Offset", &HeightmapUniformBlock_.HeightmapOffset_, -256.0f, 256.0f, "%.3f", 2.0f );
		ImGui::SliderFloat( "Layer 0 Height", &HeightmapUniformBlock_.HeightmapLayer0Height_, 1.0f, 256.0f, "%.3f", 2.0f );
		ImGui::SliderFloat( "Layer 1 Height", &HeightmapUniformBlock_.HeightmapLayer1Height_, 1.0f, 256.0f, "%.3f", 2.0f );
		ImGui::SliderFloat( "Layer 2 Height", &HeightmapUniformBlock_.HeightmapLayer2Height_, 1.0f, 256.0f, "%.3f", 2.0f );
		ImGui::SliderFloat( "Layer 3 Height", &HeightmapUniformBlock_.HeightmapLayer3Height_, 1.0f, 256.0f, "%.3f", 2.0f );
		ImGui::SliderFloat( "Normal Tweak", &HeightmapUniformBlock_.HeightmapNormalTweak_, 0.0001f, 1.0f, "%.3f", 4.0f );
		ImGui::SliderFloat( "Scroll Speed", &ScrollSpeed_, -256.0f, 256.0f, "%.3f", 2.0f );

		ImGui::EndGroup();
		ImGui::Separator();
	}
	ImGui::End();

	ObjectUniformBlock_.WorldTransform_.identity();
	ObjectUniformBlock_.NormalTransform_.identity();
	RsCore::pImpl()->updateBuffer( ObjectUniformBuffer_.get(), 0, sizeof( ObjectUniformBlock_ ), 
		RsResourceUpdateFlags::ASYNC,
		[ UniformBlock = ObjectUniformBlock_ ]( class RsBuffer*, const RsBufferLock& Lock )
		{
			memcpy( Lock.Buffer_, &UniformBlock, sizeof( UniformBlock ) );
		} );


	const BcF32 PositionOffsetAmount = ( HeightmapUniformBlock_.HeightmapDimensions_ / static_cast< float >( Size_ ) ) * 2.0f;

	HeightmapUniformBlock_.HeightmapPositionOffset_ += Tick * ScrollSpeed_;
	while( HeightmapUniformBlock_.HeightmapPositionOffset_ > PositionOffsetAmount )
	{
		HeightmapUniformBlock_.HeightmapPositionOffset_ -= PositionOffsetAmount;
		HeightmapUniformBlock_.HeightmapUVOffset_ -= HeightmapUniformBlock_.HeightmapUVScale_.y();
		if( HeightmapUniformBlock_.HeightmapUVOffset_ < -256.0f )
		{
			HeightmapUniformBlock_.HeightmapUVOffset_ += 256.0f;
		}
	}
	while( HeightmapUniformBlock_.HeightmapPositionOffset_ < -PositionOffsetAmount )
	{
		HeightmapUniformBlock_.HeightmapPositionOffset_ += PositionOffsetAmount;
		HeightmapUniformBlock_.HeightmapUVOffset_ += HeightmapUniformBlock_.HeightmapUVScale_.y();
		if( HeightmapUniformBlock_.HeightmapUVOffset_ > 256.0f )
		{
			HeightmapUniformBlock_.HeightmapUVOffset_ -= 256.0f;
		}
	}

	RsCore::pImpl()->updateBuffer( HeightmapUniformBuffer_.get(), 0, sizeof( HeightmapUniformBlock_ ), 
		RsResourceUpdateFlags::ASYNC,
		[ UniformBlock = HeightmapUniformBlock_ ]( class RsBuffer*, const RsBufferLock& Lock )
		{
			memcpy( Lock.Buffer_, &UniformBlock, sizeof( UniformBlock ) );
		} );
}

//////////////////////////////////////////////////////////////////////////
// onAttach
//virtual
void GaHeightmapComponent::onAttach( ScnEntityWeakRef Parent )
{
	Super::onAttach( Parent );

	generateBuffers();
}

//////////////////////////////////////////////////////////////////////////
// onDetach
//virtual
void GaHeightmapComponent::onDetach( ScnEntityWeakRef Parent )
{
	Super::onDetach( Parent );
}

//////////////////////////////////////////////////////////////////////////
// generateBuffers
void GaHeightmapComponent::generateBuffers()
{
	BcU32 NoofVertices = Size_ * Size_;
	BcU32 NoofIndices = ( ( Size_ - 1 ) * ( Size_ - 1 ) ) * 6;

#if !PSY_PRODUCTION
	const std::string DebugNameStr = getFullName();
	const char* DebugName = DebugNameStr.c_str();
#else
	const char* DebugName = "";
#endif

	VertexDeclaration_ = RsCore::pImpl()->createVertexDeclaration( 
		RsVertexDeclarationDesc( 2 )
			.addElement( RsVertexElement( 0, 0, 2, RsVertexDataType::FLOAT32, RsVertexUsage::POSITION, 0 ) )
			.addElement( RsVertexElement( 0, 8, 2, RsVertexDataType::FLOAT32, RsVertexUsage::TEXCOORD, 0 ) ),
		DebugName );

	VertexBuffer_ = RsCore::pImpl()->createBuffer(
		RsBufferDesc( RsResourceBindFlags::VERTEX_BUFFER, RsResourceCreationFlags::STATIC,
			sizeof( Vertex ) * NoofVertices, sizeof( Vertex ) ), DebugName );

	IndexBuffer_ = RsCore::pImpl()->createBuffer(
		RsBufferDesc( RsResourceBindFlags::INDEX_BUFFER, RsResourceCreationFlags::STATIC, 
			NoofIndices * sizeof( Index ), sizeof( Index ) ), DebugName );

	ObjectUniformBuffer_ = Material_->createUniformBuffer< ScnShaderObjectUniformBlockData >( DebugName );
	HeightmapUniformBuffer_ = Material_->createUniformBuffer< GaHeightmapUniformBlockData >( DebugName );

	RsCore::pImpl()->updateBuffer( VertexBuffer_.get(), 0, sizeof( Vertex ) * NoofVertices, 
		RsResourceUpdateFlags::ASYNC,
		[ Size = Size_ ]( class RsBuffer*, const RsBufferLock& Lock )
		{
			auto* OutVertices = reinterpret_cast< Vertex* >( Lock.Buffer_ );
			BcF32 FullSize = static_cast< BcF32 >( Size );
			BcF32 HalfSize = static_cast< BcF32 >( Size ) * 0.5f;
			for( BcU32 Y = 0; Y < Size; ++Y )
			{
				for( BcU32 X = 0; X < Size; ++X )
				{
					OutVertices->Position_ = 
						( MaVec2d( static_cast< BcF32 >( X ), static_cast< BcF32 >( Y ) ) -  
						MaVec2d( HalfSize, HalfSize ) ) / MaVec2d( HalfSize, HalfSize );
					OutVertices->UV_ =
						( MaVec2d( static_cast< BcF32 >( X ), static_cast< BcF32 >( Y ) ) /  
						MaVec2d( FullSize, FullSize ) );
					++OutVertices;
				}
			}
		} );

	RsCore::pImpl()->updateBuffer( IndexBuffer_.get(), 0, sizeof( Index ) * NoofIndices, 
		RsResourceUpdateFlags::ASYNC,
		[ Size = Size_ ]( class RsBuffer*, const RsBufferLock& Lock )
		{
			auto* OutIndices = reinterpret_cast< Index* >( Lock.Buffer_ );
			BcU32 BaseIndex = 0;
			for( BcU32 Y = 0; Y < Size - 1; ++Y )
			{
				BaseIndex = Y * Size;
				for( BcU32 X = 0; X < Size - 1; ++X )
				{
					*OutIndices++ = BaseIndex + 0;
					*OutIndices++ = BaseIndex + 1;
					*OutIndices++ = BaseIndex + Size;
					*OutIndices++ = BaseIndex + Size;
					*OutIndices++ = BaseIndex + 1;
					*OutIndices++ = BaseIndex + Size + 1;

					BaseIndex++;
				}
			}
		} );

	RsCore::pImpl()->updateBuffer( ObjectUniformBuffer_.get(), 0, sizeof( ObjectUniformBlock_ ), 
		RsResourceUpdateFlags::ASYNC,
		[ UniformBlock = ObjectUniformBlock_ ]( class RsBuffer*, const RsBufferLock& Lock )
		{
			memcpy( Lock.Buffer_, &UniformBlock, sizeof( UniformBlock ) );
		} );

	HeightmapUniformBlock_.HeightmapUVScale_ = MaVec2d( 1.0f, 1.0f ) / MaVec2d( static_cast< BcF32 >( Size_ ), static_cast< BcF32 >( Size_ ) );

	RsCore::pImpl()->updateBuffer( HeightmapUniformBuffer_.get(), 0, sizeof( HeightmapUniformBlock_ ), 
		RsResourceUpdateFlags::ASYNC,
		[ UniformBlock = HeightmapUniformBlock_ ]( class RsBuffer*, const RsBufferLock& Lock )
		{
			memcpy( Lock.Buffer_, &UniformBlock, sizeof( UniformBlock ) );
		} );

	RsGeometryBindingDesc GeometryBindingDesc;
	GeometryBindingDesc.setVertexDeclaration( VertexDeclaration_.get() );
	GeometryBindingDesc.setVertexBuffer( 0, VertexBuffer_.get(), sizeof( Vertex ), 0 );
	GeometryBindingDesc.setIndexBuffer( IndexBuffer_.get(), sizeof( Index ), 0 );
	GeometryBinding_ = RsCore::pImpl()->createGeometryBinding( GeometryBindingDesc, DebugName );
}

//////////////////////////////////////////////////////////////////////////
// render
void GaHeightmapComponent::render( ScnRenderContext & RenderContext )
{
	auto* ViewRenderData = reinterpret_cast< GaHeightmapViewRenderData* >( RenderContext.ViewRenderData_ );

	BcU32 NoofIndices = ( ( Size_ - 1 ) * ( Size_ - 1 ) ) * 6;

	// Queue render node.
	auto Sort = RenderContext.Sort_;
	RenderContext.pFrame_->queueRenderNode( Sort,
		[
			GeometryBinding = GeometryBinding_.get(),
			ProgramBinding = ViewRenderData->ProgramBinding_.get(),
			RenderState = ViewRenderData->RenderState_,
			FrameBuffer = RenderContext.pViewComponent_->getFrameBuffer(),
			Viewport = RenderContext.pViewComponent_->getViewport(),
			NoofIndices
		]
		( RsContext* Context )
		{
			Context->drawIndexedPrimitives( GeometryBinding, ProgramBinding,
				RenderState, FrameBuffer, &Viewport, nullptr,
				RsTopologyType::TRIANGLE_LIST, 0, NoofIndices, 0, 0, 1 );
		} );

}

//////////////////////////////////////////////////////////////////////////
// render
class ScnViewRenderData* GaHeightmapComponent::createViewRenderData( class ScnViewComponent* View )
{
	auto SortPassType = View->getSortPassType( getPasses(), getRenderPermutations() );
	if( SortPassType == RsRenderSortPassType::INVALID )
	{
		return nullptr;
	}

#if !PSY_PRODUCTION
	const std::string DebugName = getFullName();
	const char* DebugNameCStr = DebugName.c_str();
#else
	const char* DebugNameCStr = nullptr;
#endif

	auto ViewRenderData = new GaHeightmapViewRenderData();
	ViewRenderData->setSortPassType( SortPassType );

	// Create view component.
	ScnShaderPermutationFlags ShaderPermutation = ScnShaderPermutationFlags::MESH_STATIC_3D;
	ShaderPermutation |= View->getRenderPermutation();

	auto Program = Material_->getProgram( ShaderPermutation );
	BcAssert( Program );
	auto ProgramBindingDesc = Material_->getProgramBinding( ShaderPermutation );
	
	BcU32 HeightSlot = Program->findShaderResourceSlot( "aHeightTex" );
	if( HeightSlot != BcErrorCode )
	{
		ProgramBindingDesc.setShaderResourceView( HeightSlot, Texture_->getTexture() );
	}

	BcU32 ObjectUniformSlot = Program->findUniformBufferSlot( "ScnShaderObjectUniformBlockData" );
	if( ObjectUniformSlot != BcErrorCode)
	{
		ProgramBindingDesc.setUniformBuffer( ObjectUniformSlot, ObjectUniformBuffer_.get(), 0, sizeof( ObjectUniformBlock_ ) );
	}
	BcU32 HeightmapUniformSlot = Program->findUniformBufferSlot( "GaHeightmapUniformBlockData" );
	if( HeightmapUniformSlot != BcErrorCode )
	{
		ProgramBindingDesc.setUniformBuffer( HeightmapUniformSlot, HeightmapUniformBuffer_.get(), 0, sizeof( HeightmapUniformBlock_ ) );
	}
	BcU32 ViewUniformSlot = Program->findUniformBufferSlot( "ScnShaderViewUniformBlockData" );
	if( ViewUniformSlot != BcErrorCode )
	{
		ProgramBindingDesc.setUniformBuffer( ViewUniformSlot, View->getViewUniformBuffer(), 0, sizeof( ScnShaderViewUniformBlockData ) );
	}


	ViewRenderData->ProgramBinding_ = RsCore::pImpl()->createProgramBinding( Program, ProgramBindingDesc, DebugNameCStr );
	ViewRenderData->RenderState_ = Material_->getRenderState();

	return ViewRenderData;
}

//////////////////////////////////////////////////////////////////////////
// render
void GaHeightmapComponent::destroyViewRenderData( class ScnViewComponent* View, ScnViewRenderData* ViewRenderData )
{
	delete ViewRenderData;
}

