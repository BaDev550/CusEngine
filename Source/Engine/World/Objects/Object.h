#pragma once
#include <Engine/Core/Core.h>
#include <string_view>

#include <Runtime/Definitions/UUID.h>
#include <Runtime/Reflection/ReflectObject.h>

namespace Tourqe::Engine {
	class WorldSubsystem;

	enum class ObjectState : u8 {
		NotSpawned = 0,
		Spawned,
		PendingDestroy
	};

	TCLASS()
    class ENGINE_API TObject : public Runtime::Reflection::ReflectObject {
		GENERATE_CLASS(TObject)
    public:
		TObject() = default;
		virtual ~TObject() = default;
		TObject(const TObject&&) = delete;
		TObject& operator=(const TObject&&) = delete;
		TObject(TObject&&) = delete;
		TObject& operator=(TObject&&) = delete;

		const Runtime::UUID& GetObjectID() const { return _objectID; }
	protected:
		virtual WorldSubsystem* GetWorld() const { return _worldContext; }
		ObjectState _objectState = ObjectState::NotSpawned;
	private:
		WorldSubsystem* _worldContext;
		Runtime::UUID _objectID = 0;

		friend class Scene;
		friend class GarbageCollector;
    };
}