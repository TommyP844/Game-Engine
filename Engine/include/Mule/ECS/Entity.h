
#include <vector>
#include <entt/entt.hpp>
#include <memory>

class Scene;

class Entity
{
public:


	template<typename T, typename ... Args>
	T& AddComponent(Args&&... args);

	template<typename T>
	bool HasComponent();

	template<typename T>
	T& GetComponent();

	template<typename T>
	void RemoveComponent();

	Entity GetParent();

	std::vector<Entity> GetChildren();

	void AddChild(Entity e);
	void RemoveChild(Entity e);

private:
	entt::entity mID;
	std::weak_ptr<Scene> mScene;
};
