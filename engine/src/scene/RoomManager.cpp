// Created by Joey Driedger

#include "engine/scene/RoomManager.h"

#include <algorithm>
#include <stdexcept>

namespace engine
{
    void RoomManager::AddRoom(const int roomId)
    {
        if (HasRoom(roomId))
        {
            return;
        }

        m_Rooms.push_back(roomId);

        if (m_CurrentRoomId < 0)
        {
            m_CurrentRoomId = roomId;
        }
    }

    bool RoomManager::HasRoom(const int roomId) const
    {
        return std::find(m_Rooms.begin(), m_Rooms.end(), roomId) != m_Rooms.end();
    }

    void RoomManager::SetCurrentRoom(const int roomId)
    {
        if (!HasRoom(roomId))
        {
            throw std::runtime_error("Room does not exist.");
        }

        m_CurrentRoomId = roomId;
    }

    int RoomManager::GetCurrentRoom() const
    {
        return m_CurrentRoomId;
    }

    void RoomManager::SetTransitionTarget(const int roomId)
    {
        if (!HasRoom(roomId))
        {
            throw std::runtime_error("Transition target does not exist.");
        }

        m_TransitionTarget = roomId;
    }

    bool RoomManager::HasTransitionTarget() const
    {
        return m_TransitionTarget >= 0;
    }

    int RoomManager::GetTransitionTarget() const
    {
        return m_TransitionTarget;
    }

    void RoomManager::ClearTransitionTarget()
    {
        m_TransitionTarget = -1;
    }
}