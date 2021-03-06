#include "PlayerFps.hpp"

#include <Inputs/Input.hpp>
#include <Scenes/Scenes.hpp>
#include <Scenes/Entity.hpp>
#include <Physics/KinematicCharacter.hpp>
#include <Uis/Uis.hpp>

namespace test {
bool PlayerFps::registered = Register("playerFps");

const float WALK_SPEED = 3.1f;
const float RUN_SPEED = 5.7f;
const float CROUCH_SPEED = 1.2f;
const float JUMP_SPEED = 4.1f;
const float NOCLIP_SPEED = 3.0f;

PlayerFps::PlayerFps() {
}

void PlayerFps::Start() {
	//auto collisionObject = GetParent()->GetComponent<CollisionObject>();
	//collisionObject->GetCollisionEvents().Subscribe([&](CollisionObject *other){ Log::Out("Player collided with ", std::quoted(other->GetParent()->GetName()), '\n';}));
	//collisionObject->GetSeparationEvents().Subscribe([&](CollisionObject *other){ Log::Out("Player seperated with ", std::quoted(other->GetParent()->GetName()), '\n';}));
}

void PlayerFps::Update() {
	auto character = GetEntity()->GetComponent<KinematicCharacter>();

	if (!character || !character->IsShapeCreated()) {
		return;
	}

	Vector3f direction;

	if (!Scenes::Get()->IsPaused()) {
		direction.m_x = Input::Get()->GetAxis("strafe")->GetAmount();
		direction.m_z = Input::Get()->GetAxis("forward")->GetAmount();

		if (m_noclipEnabled) {
			if (Input::Get()->GetButton("jump")->IsDown()) {
				direction.m_y = 1.0f;
			} else if (Input::Get()->GetButton("crouch")->IsDown()) {
				direction.m_y = -1.0f;
			}
		} else {
			if (Input::Get()->GetButton("jump")->WasDown() && character->IsOnGround()) {
				character->Jump({0.0f, JUMP_SPEED, 0.0f});
			}
		}

		if (Input::Get()->GetButton("noclip")->WasDown()) {
			m_noclipEnabled = !m_noclipEnabled;

			if (m_noclipEnabled) {
				character->SetGravity({});
				character->SetLinearVelocity({});
			} else {
				character->SetGravity(Scenes::Get()->GetPhysics()->GetGravity());
			}

			Log::Out("Player Noclip: ", std::boolalpha, m_noclipEnabled, '\n');
		}
	}

	auto cameraRotation = Scenes::Get()->GetCamera()->GetRotation();

	if (auto transform = GetEntity()->GetComponent<Transform>()) {
		transform->SetLocalRotation({0.0f, cameraRotation.m_y, 0.0f});
	}

	auto walkDirection = direction;
	walkDirection.m_x = -(direction.m_z * std::sin(cameraRotation.m_y) + direction.m_x * std::cos(cameraRotation.m_y));
	walkDirection.m_z = direction.m_z * std::cos(cameraRotation.m_y) - direction.m_x * std::sin(cameraRotation.m_y);

	//walkDirection = walkDirection.Normalize();
	walkDirection *= Input::Get()->GetButton("sprint")->IsDown() ? RUN_SPEED : Input::Get()->GetButton("crouch")->IsDown() ? CROUCH_SPEED : WALK_SPEED;
	walkDirection *= m_noclipEnabled ? NOCLIP_SPEED : 1.0f;
	character->SetWalkDirection(0.02f * walkDirection);
}

const Node &operator>>(const Node &node, PlayerFps &player) {
	return node;
}

Node &operator<<(Node &node, const PlayerFps &player) {
	return node;
}
}
