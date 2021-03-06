#pragma once

#include "Psybrus.h"

#include "System/Scene/Rendering/ScnModel.h"
#include "System/Scene/Rendering/ScnRenderableComponent.h"
#include "System/Scene/Rendering/ScnTexture.h"

//////////////////////////////////////////////////////////////////////////
// GaHeightmapUniformBlockData
struct GaHeightmapUniformBlockData
{
	REFLECTION_DECLARE_BASIC( GaHeightmapUniformBlockData );
	GaHeightmapUniformBlockData(){};

	MaVec2d HeightmapUVScale_ = MaVec2d( 0.0f, 0.0f );
	BcF32 HeightmapDimensions_ = 1.0f;
	BcF32 HeightmapHeight_ = 1.0f;
	BcF32 HeightmapOffset_ = 0.0f;
	BcF32 HeightmapPositionOffset_ = 0.0f;
	BcF32 HeightmapUVOffset_ = 0.0f;
	BcF32 HeightmapLayer0Height_ = 1.0f;
	BcF32 HeightmapLayer1Height_ = 4.0f;
	BcF32 HeightmapLayer2Height_ = 8.0f;
	BcF32 HeightmapLayer3Height_ = 16.0f;
	BcF32 HeightmapNormalTweak_ = 0.1f;
};

//////////////////////////////////////////////////////////////////////////
// GaHeightmapComponent
class GaHeightmapComponent:
	public ScnRenderableComponent
{
public:
	REFLECTION_DECLARE_DERIVED( GaHeightmapComponent, ScnRenderableComponent );

	GaHeightmapComponent();
	virtual ~GaHeightmapComponent();
	
	void update( BcF32 Tick );

	void onAttach( ScnEntityWeakRef Parent ) override;
	void onDetach( ScnEntityWeakRef Parent ) override;
	
	void generateBuffers();

	void render( ScnRenderContext & RenderContext ) override;
	class ScnViewRenderData* createViewRenderData( class ScnViewComponent* View ) override;
	void destroyViewRenderData( class ScnViewComponent* View, ScnViewRenderData* ViewRenderData ) override;

private:
	struct Vertex
	{
		MaVec2d Position_;
		MaVec2d UV_;
	};

	typedef BcU32 Index;

	ScnTextureRef Texture_;
	ScnMaterialRef Material_;

	BcF32 ScrollSpeed_ = -32.0f;
	BcU32 Size_ = 128;
	ScnShaderObjectUniformBlockData ObjectUniformBlock_;
	GaHeightmapUniformBlockData HeightmapUniformBlock_;


	RsVertexDeclarationUPtr VertexDeclaration_;
	RsBufferUPtr VertexBuffer_;
	RsBufferUPtr IndexBuffer_;
	RsBufferUPtr ObjectUniformBuffer_;
	RsBufferUPtr HeightmapUniformBuffer_;
	RsGeometryBindingUPtr GeometryBinding_;

};
