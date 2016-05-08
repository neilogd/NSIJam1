#include "Psybrus.h"

#include "System/Scene/ScnCore.h"

#include "System/Os/OsCore.h"
#include "System/Os/OsClient.h"

//////////////////////////////////////////////////////////////////////////
// PsyGameInit
void PsyGameInit()
{

}

//////////////////////////////////////////////////////////////////////////
// PsyLaunchGame
void PsyLaunchGame()
{
	// Maximise window.
	OsCore::pImpl()->getClient( 0 )->maximise();
	
	// Spawn entity called "MainEntity" from Dist/Content/default.pkg,
	// and name it "MainEntity_0", and place it in the root of the scene.
	ScnCore::pImpl()->spawnEntity(
		ScnEntitySpawnParams(
			"CameraEntity_0", "default", "CameraEntity",
			MaMat4d(), nullptr));
	ScnCore::pImpl()->spawnEntity(
		ScnEntitySpawnParams(
			"MainEntity_0", "default", "MainEntity",
			MaMat4d(), nullptr));
}
