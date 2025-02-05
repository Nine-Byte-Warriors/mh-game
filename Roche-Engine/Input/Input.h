#pragma once
#ifndef INPUT_H
#define INPUT_H

#include "Listener.h"
#include "WindowContainer.h"

class Input : public WindowContainer, public Listener
{
public:
	~Input() { RemoveFromEvent(); }
	void Initialize( RenderWindow& window );
	void Update( const float dt );

	inline Mouse& GetMouse() noexcept { return m_mouse; }
	inline Keyboard& GetKeyboard() noexcept { return m_keyboard; }

private:
	enum class Key
	{
		Up,
		Left,
		Down,
		Right,
		Dash,
		Interact
	};
	void UpdateMouse( const float dt );
	void UpdateKeyboard( const float dt );

	void HandleEvent( Event* event ) override;
	void RemoveFromEvent() noexcept;
	void AddToEvent() noexcept;

	std::map<Key, std::string> m_keyInputs;
	bool m_bReadCharInput;
	std::string m_sKeys;
};

#endif