#pragma once
#ifndef HEALTH_H
#define HEALTH_H

class Collider;

class Health : public Listener
{
public:
	Health(std::string type, int entityNum, std::shared_ptr<Collider> collider,std::string name);
	~Health() { RemoveFromEvent(); }

	inline float GetCurrentHealth() const noexcept { return m_fCurrentHealth; }
	inline float GetMaxHealth() const noexcept { return m_fMaxHealth; }

	void SetHealth( float maxHealth );
	void TakeDamage( float damageAmount );
	void Heal( float healAmount );
	void Hit(Collider& collider);

	void SetEntityNum(int num);

private:
	void FilterName(std::string name);
	void AddToEvent() noexcept;
	void RemoveFromEvent() noexcept;
	void HandleEvent( Event* event ) override;

	int m_iEntityNum;
	std::string m_sType;
	std::shared_ptr<Collider> m_collider;
	std::string m_enemyName;
	int m_iEnemyMoneyReward;
	int m_iEnemyScoreReward;
	float m_fCurrentHealth;
	float m_fMaxHealth;
	bool m_bIsDead = false;
	std::string m_sName;
};

#endif