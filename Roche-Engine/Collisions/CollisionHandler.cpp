#include "stdafx.h"
#include "CollisionHandler.h"

CollisionHandler::CollisionHandler(CollisionMatrix collisionMatrix)
{
    m_collisionMatrix.SetMatrix(collisionMatrix);
}

void CollisionHandler::AddCollider(std::shared_ptr<Collider> collider)
{
    m_colliders.push_back(collider);
}

void CollisionHandler::AddCollider(std::vector<std::shared_ptr<Collider>> colliders)
{
    for (int i = 0; i < colliders.size(); i++)
    {
        m_colliders.push_back(colliders[i]);
    }
}

void CollisionHandler::SetMatrix(bool dD, bool dP, bool dE, bool dPj,
    bool pD, bool pP, bool pE, bool pPj,
    bool eD, bool eP, bool eE, bool ePj,
    bool pjD, bool pjP, bool pjE, bool pjPj)
{
    m_collisionMatrix.SetMatrix(dD, dP, dE, dPj,
        pD, pP, pE, pPj,
        eD, eP, eE, ePj,
        pjD, pjP, pjE, pjPj);
}

void CollisionHandler::RemoveAllColliders()
{
    m_colliders.clear();
}

void CollisionHandler::CollisionCheckAll()
{
    int startIndex = 0;
    bool isCollision;
    bool layersInteract;
    bool collidersInteract;
    for (int i = 0; i < m_colliders.size(); i++)
    {
        isCollision = false;
        for (int n = 0; n < m_colliders.size(); n++)
        {
            layersInteract = m_collisionMatrix.GetElement((int)m_colliders[i]->GetLayer(), (int)m_colliders[n]->GetLayer());
            collidersInteract = m_colliders[i]->GetCollisionMask().m_mask[(int)m_colliders[n]->GetLayer()];

            if (n == i || layersInteract == false || collidersInteract == false)
            {
                continue;
            }

            bool isIntersection = m_colliders[i]->CollisionCheck(m_colliders[n]);

            if (isIntersection)
            {
                isCollision = true;
                std::shared_ptr<Collider> collider1 = std::make_shared<Collider>(*m_colliders[i]);
                std::shared_ptr<Collider> collider2 = std::make_shared<Collider>(*m_colliders[n]);
                m_colliders[i]->LogCollision(collider2);
                m_colliders[n]->LogCollision(collider1);

                m_colliders[i]->Resolution(m_colliders[n]);
            }
        }
        if (!isCollision)
        {
            m_colliders[i]->UpdateLastValidPosition();
        }
    }
}

void CollisionHandler::Update()
{
    CollisionCheckAll();
    for (auto& col : m_colliders)
    {
        col->Update();
    }
}
